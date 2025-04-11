import requests
import json
from typing import Dict, Any
from host import hostaddr

def recharge(user: str, cards: list) -> Dict[str, Any]:
    """为用户充值卡密（Python翻译版）"""
    try:
        requestbody = {
            "user": user,
            "cards": cards,
        }
        # 发送POST请求（使用同步requests库）
        response = requests.post(
            f"{hostaddr}/api/v1/users/addexpiry",
            headers={"Content-Type": "application/json"},
            data=json.dumps(requestbody)
        )
        
        # 处理HTTP错误状态码
        if not response.ok:
            error_data = response.json()
            raise requests.exceptions.HTTPError(
                f"请求失败: {error_data.get('error', '未知错误')} "
                f"(原因: {error_data.get('reason', '未知原因')})",
                response=response
            )
            
        return response.json()
        
    except requests.exceptions.RequestException as e:
        print(f'[ERROR] 调用加密接口出错: {str(e)}')
        raise  # 重新抛出异常给调用者处理
    except Exception as e:
        print(f'[ERROR] 意外错误: {str(e)}')
        raise