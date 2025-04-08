// Welcome to qq group: 1030115250
// const {decryptData,encryptJSON} = require('./aesnet.js');
const {hostaddr, netdata,keyBuffer} = require('./host.js');
const crypto = require('crypto');

async function register(user, password,question,answer,cards) {
    try {
        const passwordHash = crypto.createHash('sha512').update(password).digest('hex')
        const questionHash = crypto.createHash('sha512').update(question).digest('hex')
        const answerHash = crypto.createHash('sha512').update(answer).digest('hex')
        const requestbody={
            user:user,
            password: passwordHash,
            question: questionHash,
            answer: answerHash,
            ...(cards && { cards:cards })
        }
        const response = await fetch(`${hostaddr}/api/users/register`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(requestbody),
        }); 
        if (!response.ok) {
            const errorData = await response.json();
            throw new Error(`请求失败: ${errorData.error} (原因: ${errorData.reason})`); 
        }
        const responseData = await response.json();
        return responseData;
    }
    catch (error) {
        console.error('调用加密接口出错:', error);
        throw error; 
    }
}

module.exports = {register};