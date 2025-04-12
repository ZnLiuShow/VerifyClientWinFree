// host.rs
use std::sync::Mutex; // Add this import
use std::sync::atomic::{AtomicPtr, Ordering};
use openssl::rand::rand_bytes;
use once_cell::sync::Lazy;

static HOST_ADDR: AtomicPtr<String> = AtomicPtr::new(std::ptr::null_mut());

static DEFAULT_HOST: Lazy<String> = Lazy::new(|| {
    "http://localhost".to_string()
});

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


pub fn set_host(addr: &str) {
    let boxed = Box::new(addr.to_string());
    let ptr = Box::into_raw(boxed);
    
    // 原子交换指针
    let old_ptr = HOST_ADDR.swap(ptr, Ordering::Release);
    
    // 释放旧指针内存（安全处理多次设置）
    if !old_ptr.is_null() {
        unsafe { Box::from_raw(old_ptr) };
    }
}

pub fn get_host() -> &'static str {
    let ptr = HOST_ADDR.load(Ordering::Acquire);
    if ptr.is_null() {
        &DEFAULT_HOST
    } else {
        unsafe { &*ptr }
    }
}