import os
from typing import Dict, Any

# 后端地址 (需要自行配置反向代理，去掉/api/和转发端口)
hostaddr = "http://localhost"

# 网络数据配置
netdata: Dict[str, Any] = {
    "mytoken": "",
    "aeskey": None
}

# 生成 32 字节的随机密钥 (Python 使用 os.urandom 替代 crypto.randomBytes)
keyBuffer = os.urandom(32)