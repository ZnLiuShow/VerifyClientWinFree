import requests
import json
import base64
import hashlib
import time
from .host import hostaddr, netdata, keyBuffer
from .aesnet import decrypt_data, encrypt_json

def send_encrypt_request():
    key_base64 = base64.b64encode(keyBuffer).decode('utf-8')
    try:
        response = requests.post(
            f"{hostaddr}/api/v1/users",
            headers={'Content-Type': 'application/json'},
            json={'key': key_base64}
        )
        response.raise_for_status()
        response_data = response.json()
        print('链接成功:', response_data)
        return response_data
    except requests.exceptions.HTTPError as err:
        try:
            error_data = response.json()
            error_msg = error_data.get('error', '未知错误')
            reason = error_data.get('reason', '未知原因')
            raise Exception(f"请求失败: {error_msg} (原因: {reason})")
        except json.JSONDecodeError:
            raise Exception(f"HTTP错误: {err.response.status_code} {err.response.reason}")
    except requests.exceptions.RequestException as err:
        raise Exception(f"网络请求出错: {err}")
    except Exception as err:
        raise Exception(f"发生错误: {err}")

def login(username, password):
    try:
        # 发送初始化请求并获取加密数据
        init = send_encrypt_request()
        
        # 解密初始化响应数据以获取AES密钥
        decrypted = decrypt_data(
            ciphertext=init['data'],
            current_key=keyBuffer,
            iv=init['iv'],
            tag=init['authTag']
        )
        netdata['aeskey'] = base64.b64decode(decrypted['data'])  # 假设解密后的数据中有'data'字段存储base64密钥
        
        # 准备登录数据
        password_hash = hashlib.sha512(password.encode()).hexdigest()
        data = {
            'user': username,
            'password': password_hash,
            'timestamp': int(time.time() * 1000)
        }
        
        # 加密登录数据
        encrypted_data = encrypt_json(data, netdata['aeskey'])
        
        # 发送登录请求
        response = requests.post(
            f"{hostaddr}/api/v1/users/login",  # 注意根据实际API调整端点
            headers={'Content-Type': 'application/json'},
            json=encrypted_data
        )
        response.raise_for_status()
        response_data = response.json()
        
        # 解密登录响应
        de_data = decrypt_data(
            ciphertext=response_data['data'],
            current_key=netdata['aeskey'],
            iv=response_data['iv'],
            tag=response_data['tag']
        )
        
        # 处理解密后的数据
        if de_data.get('success', False):
            netdata['mytoken'] = de_data.get('token', '')
        if 'newkey' in de_data:
            netdata['aeskey'] = base64.b64decode(de_data['newkey'])
        
        print('登录成功:', de_data)
        return de_data.get('success', False)
    except Exception as err:
        print(f'登录失败: {err}')
        raise