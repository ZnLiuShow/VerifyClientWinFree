import hashlib
import json
from typing import Optional, Dict, Any
import requests
from .host import hostaddr, netdata, keyBuffer  

def register(user: str, password: str, question: str, answer: str, cards: Optional[str] = None) -> Dict[str, Any]:
    """
    用户注册函数
    :param user: 用户名
    :param password: 密码
    :param question: 安全问题
    :param answer: 安全答案
    :param cards: 卡号（可选）
    :return: 响应数据字典
    """
    try:
        # 计算 SHA512 哈希
        password_hash = hashlib.sha512(password.encode()).hexdigest()
        question_hash = hashlib.sha512(question.encode()).hexdigest()
        answer_hash = hashlib.sha512(answer.encode()).hexdigest()

        # 构建请求体
        request_body = {
            "user": user,
            "password": password_hash,
            "question": question_hash,
            "answer": answer_hash
        }
        if cards:
            request_body["cards"] = cards

        # 发送 POST 请求
        response = requests.post(
            f"{hostaddr}/api/v1/users/register",
            headers={"Content-Type": "application/json"},
            json=request_body
        )
        response.raise_for_status()  # 自动处理 HTTP 错误状态码

        return response.json()

    except requests.exceptions.RequestException as e:
        error_msg = f"请求失败: {str(e)}"
        if e.response is not None:
            try:
                error_data = e.response.json()
                error_msg = f"{error_data.get('error', '')} (原因: {error_data.get('reason', '未知')})"
            except json.JSONDecodeError:
                error_msg = f"HTTP错误: {e.response.status_code}"
        raise RuntimeError(error_msg) from e