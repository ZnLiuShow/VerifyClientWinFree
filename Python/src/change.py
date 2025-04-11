import base64
import hashlib
import time
import requests
from .host import hostaddr, netdata, keyBuffer
from .aesnet import decrypt_data, encrypt_json
from .login import send_encrypt_request

def changepassword(username, newpassword, question, answer):
    try:
        # 发送初始化请求获取加密参数
        init = send_encrypt_request()
        
        # 解密初始化响应获取AES密钥
        decrypted = decrypt_data(
            ciphertext=init['data'],
            current_key=keyBuffer,
            iv=init['iv'],
            tag=init['authTag']
        )
        netdata['aeskey'] = base64.b64decode(decrypted['data'])  # 解码base64密钥
        
        # 生成SHA-512哈希
        newpassword_hash = hashlib.sha512(newpassword.encode()).hexdigest()
        question_hash = hashlib.sha512(question.encode()).hexdigest()
        answer_hash = hashlib.sha512(answer.encode()).hexdigest()
        
        # 构造请求数据
        payload = {
            'name': username,
            'newpassword': newpassword_hash,
            'question': question_hash,
            'answer': answer_hash,
            'timestamp': int(time.time() * 1000)  # 毫秒级时间戳
        }
        
        # AES-GCM加密数据
        encrypted_data = encrypt_json(payload, netdata['aeskey'])
        
        # 发送修改密码请求
        response = requests.post(
            f"{hostaddr}/api/v1/users/change",
            headers={'Content-Type': 'application/json'},
            json=encrypted_data
        )
        response.raise_for_status()  # 检查HTTP错误
        
        # 解密响应数据
        response_data = response.json()
        de_data = decrypt_data(
            ciphertext=response_data['data'],
            current_key=netdata['aeskey'],
            iv=response_data['iv'],
            tag=response_data['tag']
        )
        
        # 轮换新密钥（如果有）
        if 'newkey' in de_data:
            netdata['aeskey'] = base64.b64decode(de_data['newkey'])
        
        print('密码修改成功:', de_data)
        return de_data.get('success', False)
    
    except requests.exceptions.RequestException as e:
        print(f'网络请求异常: {e}')
        raise
    except Exception as e:
        print(f'操作失败: {e}')
        raise