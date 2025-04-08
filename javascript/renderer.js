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
  const isValid = validateInput([
    ['regUser', '用户名不能为空'],
    ['regPass', '密码不能为空'],
    ['regQuestion', '安全问题不能为空'],
    ['regAnswer', '安全答案不能为空']
  ])
  if (!isValid) return
  const rawCards = document.getElementById('regCards').value.trim()
  const cards = rawCards ? rawCards.split('\n') : null // 空值处理
  const data = {
    user: document.getElementById('regUser').value,
    pass: document.getElementById('regPass').value,
    question: document.getElementById('regQuestion').value,
    answer: document.getElementById('regAnswer').value,
    cards: cards // 使用处理后的值
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
  
// 新增修改密码按钮事件
document.getElementById('changeBtn').addEventListener('click', () => {
  const isValid = validateInput([
    ['changeUser', '用户名不能为空'],
    ['changeNewPassword', '新密码不能为空'],
    ['changeQuestion', '安全问题不能为空'],
    ['changeAnswer', '安全答案不能为空']
  ])
  
  if (!isValid) return
  
  const data = {
    user: document.getElementById('changeUser').value,
    newPassword: document.getElementById('changeNewPassword').value,
    question: document.getElementById('changeQuestion').value,
    answer: document.getElementById('changeAnswer').value
  }
  
  window.electronAPI.sendAction('change-password', data)
})

// 修改后的响应处理（renderer.js）
window.electronAPI.onResponse((event, res) => {
  // 使用异步方式解除阻塞
  setTimeout(() => {
    const modelessAlert = document.createElement('div')
    modelessAlert.className = 'custom-alert'
    modelessAlert.innerHTML = `
      <p>操作结果: ${res.success ? '成功' : '失败'}</p>
      <p>消息: ${res.message}</p>
      <button onclick="this.parentElement.remove()">确定</button>
    `
    document.body.appendChild(modelessAlert)
  }, 10)
})