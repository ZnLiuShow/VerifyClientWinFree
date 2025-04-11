// host.rs
use std::sync::Mutex; // Add this import
use openssl::rand::rand_bytes;
use once_cell::sync::Lazy;

pub const HOST_ADDR: &str = "http://localhost";

pub fn generate_key() -> Vec<u8> {
    let mut key = [0u8; 32];
    rand_bytes(&mut key).unwrap();
    key.to_vec()
}

// 与 login.rs 共享的全局状态
pub struct NetData {
    pub mytoken: Mutex<String>,    // Wrap in Mutex
    pub aeskey: Mutex<Vec<u8>>,   // Wrap in Mutex
}

pub static NETDATA: Lazy<NetData> = Lazy::new(|| NetData {
    mytoken: Mutex::new(String::new()),
    aeskey: Mutex::new(generate_key()),
});

pub static KEY_BUFFER: Lazy<Vec<u8>> = Lazy::new(generate_key);