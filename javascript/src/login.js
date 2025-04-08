// Welcome to qq group: 1030115250
const {decryptData,encryptJSON} = require('./aesnet.js');
const {hostaddr, netdata,keyBuffer} = require('./host.js');
const crypto = require('crypto');

async function sendEncryptRequest() {  
  // 转换为Base64字符串
  const keyBase64 = keyBuffer.toString('base64');

  try {
    const response = await fetch(`${hostaddr}/api/users`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ key: keyBase64 }),
    });

    if (!response.ok) {
      const errorData = await response.json();
      throw new Error(`请求失败: ${errorData.error} (原因: ${errorData.reason})`);
    }

    const responseData = await response.json();
    console.log('链接成功:', responseData);
    return responseData;
  } catch (error) {
    console.error('调用加密接口出错:', error);
    throw error;
  }
}


async function login(username, password) {
  try {
      const init = await sendEncryptRequest();
      const decrypted = decryptData(init.data, keyBuffer, init.iv, init.authTag);
      netdata.aeskey = Buffer.from(decrypted.data, 'base64'); // 明确转换为 Buffer
      const passwordHash = crypto.createHash('sha512').update(password).digest('hex');
      const data = { 
          user:username, 
          password:passwordHash,
          timestamp:Date.now(),
      };
      const encryptedData = encryptJSON(data, netdata.aeskey);
      const response = await fetch(`${hostaddr}/api/users/login`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body:JSON.stringify(encryptedData)
        });

      if (!response.ok) {
          const errorData = await response.json();
          throw new Error(`请求失败: ${errorData.error} (原因: ${errorData.reason})`);
      }
  
      const responseData = await response.json();
      const deData = decryptData(responseData.data, netdata.aeskey, responseData.iv, responseData.tag);
      if (deData.success) {
          netdata.mytoken = deData.token; // 存储 token
      }
      if (deData?.newkey){
          netdata.aeskey = Buffer.from(deData.newkey, 'base64'); // 明确转换为 Buffer
      }
      console.log('登录成功:', deData);
      return deData.success;
  }
  catch (error) {
    console.error('登录失败:', error); 
    throw error;
  }
}

module.exports = {sendEncryptRequest,login};



