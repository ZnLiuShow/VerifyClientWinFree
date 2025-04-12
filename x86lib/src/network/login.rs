// login.rs
use reqwest;
use serde_json::{Value, json};
use base64::{Engine as _, engine::general_purpose};
use openssl::hash::{hash, MessageDigest};
use crate::network::aesnet::{decrypt_data, encrypt_json};
use crate::network::host::{get_host, NETDATA, KEY_BUFFER};
use anyhow::{Context, Result};



pub async fn send_encrypt_request() -> Result<Value> {
    let key_base64 = general_purpose::STANDARD.encode(&*KEY_BUFFER);

    let client = reqwest::Client::new();
    let response = client
        .post(&format!("{}/api/v1/users", get_host()))
        .header("Content-Type", "application/json")
        .body(json!({ "key": key_base64 }).to_string())
        .send()
        .await
        .context("请求发送失败")?;

    if !response.status().is_success() {
        let error_data: Value = response.json().await?;
        anyhow::bail!("请求失败: {} (原因: {})", 
            error_data["error"].as_str().unwrap_or("未知错误"),
            error_data["reason"].as_str().unwrap_or("未知原因"));
    }

    let response_data: Value = response.json().await?;
    println!("链接成功: {:?}", response_data);
    Ok(response_data)
}

pub async fn login(username: &str, password: &str) -> Result<bool> {
    // 发送初始化请求
    let init_data = send_encrypt_request().await?;
    let ciphertext = init_data["data"].as_str().context("无效响应格式")?;
    let iv = init_data["iv"].as_str().context("无效响应格式")?;
    let auth_tag = init_data["authTag"].as_str().context("无效响应格式")?;

    // 解密数据
    let decrypted = decrypt_data(
        ciphertext,
        &KEY_BUFFER,
        iv,
        auth_tag
    ).map_err(|e| anyhow::anyhow!("解密失败: {}", e))?;

    // 更新 AES 密钥
    let new_key = general_purpose::STANDARD.decode(
        decrypted["data"].as_str().context("无效密钥格式")?
    );
    *NETDATA.aeskey.lock().unwrap() = new_key?;

    // 计算密码哈希
    let hasher = hash(MessageDigest::sha512(), password.as_bytes())?;
    let password_hash = hex::encode(hasher);

    // 构建请求数据
    let data = json!({
        "user": username,
        "password": password_hash,
        "timestamp": chrono::Utc::now().timestamp_millis(),
    });

    // 加密数据
    let encrypted_data = encrypt_json(&data, &NETDATA.aeskey.lock().unwrap())
        .map_err(|e| anyhow::anyhow!("加密失败: {}", e))?; // Remove one semicolon here

    // 发送登录请求
    let client = reqwest::Client::new();
    let response = client
        .post(&format!("{}/api/v1/users/login", get_host()))
        .header("Content-Type", "application/json")
        .body(json!({
            "data": encrypted_data.data,
            "tag": encrypted_data.tag,
            "iv": encrypted_data.iv
        }).to_string())
        .send()
        .await
        .context("登录请求发送失败")?;

    if !response.status().is_success() {
        let error_data: Value = response.json().await?;
        anyhow::bail!(
            "{} (原因: {})", 
            error_data["error"].as_str().unwrap_or("登录失败"),
            error_data["reason"].as_str().unwrap_or("未知原因")
        );
    }

    // 处理响应
    let response_data: Value = response.json().await?;
    let ciphertext = response_data["data"].as_str().context("无效响应格式")?;
    let iv = response_data["iv"].as_str().context("无效响应格式")?;
    let tag = response_data["tag"].as_str().context("无效响应格式")?;

    let de_data = decrypt_data(
        ciphertext,
        &*NETDATA.aeskey.lock().unwrap(), // Correct key access
        iv,
        tag
    ).map_err(|e| anyhow::anyhow!("响应解密失败: {}", e))?;

    // 更新 token 和密钥
    if let Some(success) = de_data["success"].as_bool() {
        if success {
            if let Some(token_str) = de_data["token"].as_str() {
                *NETDATA.mytoken.lock().unwrap() = token_str.to_string();
            }
        }
        if let Some(new_key) = de_data["newkey"].as_str() {
            let decoded_key = general_purpose::STANDARD.decode(new_key)?;
            *NETDATA.aeskey.lock().unwrap() = decoded_key;
        }
        println!("登录成功: {:?}", de_data);
        Ok(success)
    } else {
        anyhow::bail!("无效的响应结构")
    }    
}