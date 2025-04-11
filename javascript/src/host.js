// Welcome to qq group: 1030115250
const crypto = require('crypto');


module.exports = { 
  hostaddr:"http://101.43.17.19", //这里后端需要自己配反向代理，去掉/api/和转发端口
  netdata:{
    mytoken: "",
    aeskey: null,
  },
  keyBuffer: crypto.randomBytes(32)
};