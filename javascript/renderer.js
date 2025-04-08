// Welcome to qq group: 1030115250
// 标签切换逻辑
document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      // 移除所有激活状态
      document.querySelectorAll('.tab-btn, .tab-pane').forEach(el => {
        el.classList.remove('active')
      })
      
      // 设置当前激活
      const tabId = btn.dataset.tab
      btn.classList.add('active')
      document.getElementById(tabId).classList.add('active')
    })
  })
  
  const validateInput = (fields) => {
    for (const [selector, message] of fields) {
      const element = document.getElementById(selector)
      if (!element.value.trim()) {
        alert(message)
        return false
      }
    }
    return true
  }

  // 按钮事件绑定
  document.getElementById('loginBtn').addEventListener('click', () => {
    const isValid = validateInput([
        ['loginUser', '用户名不能为空'],
        ['loginPass', '密码不能为空']
      ]);
    if (!isValid) return;
    const data = {
      user: document.getElementById('loginUser').value,
      pass: document.getElementById('loginPass').value
    }
    window.electronAPI.sendAction('login', data)
  })
  
  document.getElementById('regBtn').addEventListener('click', () => {
    const data = {
      user: document.getElementById('regUser').value,
      pass: document.getElementById('regPass').value,
      safePass: document.getElementById('regSafePass').value,
      cards: document.getElementById('regCards').value.split('\n')
    }
    window.electronAPI.sendAction('register', data)
  })
  
  document.getElementById('chargeBtn').addEventListener('click', () => {
    const data = {
      user: document.getElementById('chargeUser').value,
      cards: document.getElementById('chargeCards').value.split('\n')
    }
    window.electronAPI.sendAction('charge', data)
  })
  
  // 响应处理
  window.electronAPI.onResponse((event, res) => {
    alert(`操作结果: ${res.success ? '成功' : '失败'}\n消息: ${res.message}`)
  })