// aesnet.rs
use openssl::{symm::{Cipher, Crypter, Mode}, rand::rand_bytes};
use base64::{Engine as _, engine::general_purpose};
use serde_json::Value;

#[derive(Debug)]
pub struct EncryptionResult {
    pub data: String,
    pub tag: String,
    pub iv: String,
}

pub fn encrypt_json(data: &Value, key: &[u8]) -> Result<EncryptionResult, String> {
    let cipher = Cipher::aes_256_gcm();
    let mut iv = [0u8; 12];
    rand_bytes(&mut iv).map_err(|e| e.to_string())?;

    let mut crypter = Crypter::new(cipher, Mode::Encrypt, key, Some(&iv))
        .map_err(|e| e.to_string())?;
    crypter.pad(false);

    let json_str = data.to_string();
    let mut encrypted = vec![0; json_str.len() + cipher.block_size()];
    let count = crypter.update(json_str.as_bytes(), &mut encrypted)
        .map_err(|e| e.to_string())?;
    let rest = crypter.finalize(&mut encrypted[count..])
        .map_err(|e| e.to_string())?;
    encrypted.truncate(count + rest);

    let mut tag = vec![0u8; 16];
    crypter.get_tag(&mut tag).map_err(|e| e.to_string())?;

    Ok(EncryptionResult {
        data: general_purpose::STANDARD.encode(&encrypted),
        tag: general_purpose::STANDARD.encode(tag),
        iv: general_purpose::STANDARD.encode(iv),
    })
}

pub fn decrypt_data(ciphertext: &str, key: &[u8], iv: &str, tag: &str) -> Result<Value, String> {
    let cipher = Cipher::aes_256_gcm();
    let iv_bytes = general_purpose::STANDARD.decode(iv).map_err(|e| e.to_string())?;
    let tag_bytes = general_purpose::STANDARD.decode(tag).map_err(|e| e.to_string())?;
    let data_bytes = general_purpose::STANDARD.decode(ciphertext).map_err(|e| e.to_string())?;

    let mut crypter = Crypter::new(cipher, Mode::Decrypt, key, Some(&iv_bytes))
        .map_err(|e| e.to_string())?;
    crypter.pad(false);
    crypter.set_tag(&tag_bytes).map_err(|e| e.to_string())?;

    let mut decrypted = vec![0; data_bytes.len() + cipher.block_size()];
    let count = crypter.update(&data_bytes, &mut decrypted)
        .map_err(|e| e.to_string())?;
    let rest = crypter.finalize(&mut decrypted[count..])
        .map_err(|e| e.to_string())?;
    decrypted.truncate(count + rest);

    serde_json::from_slice(&decrypted).map_err(|e| e.to_string())
}