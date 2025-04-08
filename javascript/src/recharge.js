// Welcome to qq group: 1030115250
// const {decryptData,encryptJSON} = require('./aesnet.js');
const {hostaddr, netdata,keyBuffer} = require('./host.js');
// const crypto = require('crypto');

async function recharge(user,cards) {
    try {
        const requestbody={
            user:user,     
            cards:cards,
        }
        const response = await fetch(`${hostaddr}/api/users/recharge`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(requestbody),
        }); 
        if (!response.ok) {
            const errorData = await response.json();
            throw new Error(`请求失败: ${errorData.error} (原因: ${errorData.reason})`); 
        }
        const responseData = await response.json();
        return responseData.details;
    }
    catch (error) {
        console.error('调用加密接口出错:', error);
        throw error; 
    }
}

module.exports = {recharge};