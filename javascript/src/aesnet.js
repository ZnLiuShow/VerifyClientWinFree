// Welcome to qq group: 1030115250
const crypto = require('crypto');

const encryptJSON = (data) => {
    try {
        // 每次加密生成新的 12 字节随机 IV（符合 GCM 最佳实践）[1,5](@ref)
        const iv = crypto.randomBytes(12); 
        const cipher = crypto.createCipheriv('aes-256-gcm', currentKey, iv);
        
        const encrypted = Buffer.concat([
            cipher.update(JSON.stringify(data), 'utf8'),
            cipher.final()
        ]);

        return {
            data: encrypted.toString('base64'),
            tag: cipher.getAuthTag().toString('base64'), // 认证标签
            iv: iv.toString('base64') // 返回本次加密的 IV
        };
    } catch (error) {
        throw new Error(`加密失败: ${error.message}`);
    }
};

const decryptData = (ciphertext,currentKey, iv, tag) => {
    const tryDecrypt = (key) => {
        try {
            const ivBuffer = Buffer.from(iv, 'base64');
            const tagBuffer = Buffer.from(tag, 'base64');
            
            const decipher = crypto.createDecipheriv('aes-256-gcm', key, ivBuffer);
            decipher.setAuthTag(tagBuffer);
            
            const decrypted = Buffer.concat([
                decipher.update(Buffer.from(ciphertext, 'base64')),
                decipher.final()
            ]);
            
            return JSON.parse(decrypted.toString('utf8'))
        } catch (error) {
            return null;
        }
    };

    // 尝试用当前密钥解密
    let result = tryDecrypt(currentKey);
    if (result) return result;

    throw new Error('解密失败：密钥不匹配或数据损坏');
};


module.exports = { encryptJSON,decryptData };