// recharge.rs
use anyhow::{Context, Result};
use serde_json::json;
use reqwest::Client;
use crate::network::host::get_host;

pub async fn recharge(user: &str, cards: &[&str]) -> Result<serde_json::Value> {
    let client = Client::new();
    let request_body = json!({ "user": user, "cards": cards });

    let response = client
        .post(&format!("{}/api/v1/users/addexpiry", get_host()))
        .header("Content-Type","application/json")
        .json(&request_body)
        .send()
        .await
        .context("充值请求发送失败")?; // 添加上下文

    response
        .json()
        .await
        .context("响应解析失败")
}