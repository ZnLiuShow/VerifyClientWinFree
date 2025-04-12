// register.rs
use anyhow::{Context, Result};
use openssl::hash::{hash, MessageDigest};
use reqwest::Client;
use serde_json::json;
use crate::network::host::get_host;


pub async fn register(
    user: &str,
    password: &str,
    question: &str,
    answer: &str,
    cards: Option<&[&str]>,
) -> Result<serde_json::Value> {
    let client = Client::new();
    
    // 使用 ? 替代 unwrap() 并添加错误上下文
    let password_hash = hash(MessageDigest::sha512(), password.as_bytes())
        .context("密码哈希计算失败")?;
    let question_hash = hash(MessageDigest::sha512(), question.as_bytes())
        .context("问题哈希计算失败")?;
    let answer_hash = hash(MessageDigest::sha512(), answer.as_bytes())
        .context("答案哈希计算失败")?;

    let mut request_body = json!({
        "user": user,
        "password": hex::encode(password_hash),
        "question": hex::encode(question_hash),
        "answer": hex::encode(answer_hash),
    });

    if let Some(c) = cards {
        request_body["cards"] = json!(c);
    }

    let response = client
        .post(&format!("{}/api/v1/users/register", get_host()))
        .header("Content-Type", "application/json")
        .json(&request_body)
        .send()
        .await
        .context("注册请求发送失败")?;

    response
        .json()
        .await
        .context("响应解析失败")
}