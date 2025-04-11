import json
import base64
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes

def encrypt_json(data, current_key):
    try:
        # 生成 12 字节随机 nonce (IV)
        nonce = get_random_bytes(12)
        cipher = AES.new(current_key, AES.MODE_GCM, nonce=nonce)
        
        # 加密并生成认证标签
        plaintext = json.dumps(data).encode('utf-8')
        ciphertext, tag = cipher.encrypt_and_digest(plaintext)
        
        return {
            'data': base64.b64encode(ciphertext).decode('utf-8'),
            'tag': base64.b64encode(tag).decode('utf-8'),
            'iv': base64.b64encode(nonce).decode('utf-8')
        }
    except Exception as e:
        raise Exception(f'加密失败: {str(e)}')

def decrypt_data(ciphertext, current_key, iv, tag):
    def try_decrypt(key):
        try:
            nonce = base64.b64decode(iv)
            ciphertext_bytes = base64.b64decode(ciphertext)
            tag_bytes = base64.b64decode(tag)
            
            cipher = AES.new(key, AES.MODE_GCM, nonce=nonce)
            plaintext = cipher.decrypt_and_verify(ciphertext_bytes, tag_bytes)
            
            return json.loads(plaintext.decode('utf-8'))
        except:
            return None

    # 尝试当前密钥解密
    result = try_decrypt(current_key)
    if result is not None:
        return result

    raise Exception('解密失败：密钥不匹配或数据损坏')


# 示例用法
# if __name__ == "__main__":
#     # AES-256 密钥需为 32 字节
#     key = get_random_bytes(32)  
#     data = {"message": "Hello World"}
    
#     # 加密
#     encrypted = encrypt_json(data, key)
#     print("加密结果:", encrypted)
    
#     # 解密
#     decrypted = decrypt_data(
#         ciphertext=encrypted['data'],
#         current_key=key,
#         iv=encrypted['iv'],
#         tag=encrypted['tag']
#     )
#     print("解密结果:", decrypted)