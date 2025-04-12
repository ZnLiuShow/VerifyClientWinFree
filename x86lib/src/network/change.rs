// change.rs
use reqwest;
use serde_json::{Value, json};
use base64::{Engine as _, engine::general_purpose};
use openssl::hash::{hash, MessageDigest};
use crate::network::aesnet::{decrypt_data, encrypt_json};
use crate::network::host::{get_host, NETDATA, KEY_BUFFER};
use crate::network::login::send_encrypt_request;
use anyhow::{Context, Result};

pub async fn changepassword(
    username: &str,
    newpassword: &str,
    question: &str,
    answer: &str,
) -> Result<bool> {
    // 发送初始化请求
    let init_data = send_encrypt_request().await?;
    let ciphertext = init_data["data"].as_str().context("无效响应格式")?;
    let iv = init_data["iv"].as_str().context("无效响应格式")?;
    let auth_tag = init_data["authTag"].as_str().context("无效响应格式")?;

    // 解密并更新 AES 密钥
    let decrypted = decrypt_data(ciphertext, &KEY_BUFFER, iv, auth_tag)
        .map_err(|e| anyhow::anyhow!("初始解密失败: {}", e))?;

    *NETDATA.aeskey.lock().unwrap() = general_purpose::STANDARD
        .decode(decrypted["data"].as_str().context("无效密钥格式")?)?;

    // 生成哈希
    let newpassword_hash = hex::encode(hash(MessageDigest::sha512(), newpassword.as_bytes())?);
    let question_hash = hex::encode(hash(MessageDigest::sha512(), question.as_bytes())?);
    let answer_hash = hex::encode(hash(MessageDigest::sha512(), answer.as_bytes())?);

    // 构建请求数据
    let data = json!({
        "name": username,
        "newpassword": newpassword_hash,
        "question": question_hash,
        "answer": answer_hash,
        "timestamp": chrono::Utc::now().timestamp_millis(),
    });

    // 修改加密部分
    let encrypted_data = {
        let aeskey_guard = NETDATA.aeskey.lock().unwrap();
        encrypt_json(&data, &aeskey_guard)
    }.map_err(|e| anyhow::anyhow!("加密失败: {}", e))?;

    // 发送修改请求
    let client = reqwest::Client::new();
    let response = client
        .post(&format!("{}/api/v1/users/change", get_host()))
        .header("Content-Type", "application/json")
        .body(json!({
            "data": encrypted_data.data,
            "tag": encrypted_data.tag,
            "iv": encrypted_data.iv
        }).to_string())
        .send()
        .await
        .context("密码修改请求发送失败")?;

    if !response.status().is_success() {
        let error_data: Value = response.json().await?;
        anyhow::bail!(
            "请求失败: {} (原因: {})",
            error_data["error"].as_str().unwrap_or("未知错误"),
            error_data["reason"].as_str().unwrap_or("未知原因")
        );
    }

    // 处理响应
    let response_data: Value = response.json().await?;
    let ciphertext = response_data["data"].as_str().context("无效响应格式")?;
    let iv = response_data["iv"].as_str().context("无效响应格式")?;
    let tag = response_data["tag"].as_str().context("无效响应格式")?;

    // 修改解密部分
    let de_data = {
        let aeskey = NETDATA.aeskey.lock().unwrap();
        decrypt_data(ciphertext, &aeskey, iv, tag)
    }.map_err(|e| anyhow::anyhow!("响应解密失败: {}", e))?;

    // 更新密钥
    if let Some(new_key) = de_data["newkey"].as_str() {
        let decoded_key = general_purpose::STANDARD.decode(new_key)?;
        *NETDATA.aeskey.lock().unwrap() = decoded_key;
    }

    println!("修改密码成功: {:?}", de_data);
    de_data["success"]
        .as_bool()
        .context("无效的成功状态标识")
}