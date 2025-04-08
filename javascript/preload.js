/**
 * The preload script runs before `index.html` is loaded
 * in the renderer. It has access to web APIs as well as
 * Electron's renderer process modules and some polyfilled
 * Node.js functions.
 *
 * https://www.electronjs.org/docs/latest/tutorial/sandbox
 * Welcome to qq group: 1030115250
 */
const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('electronAPI', {
  sendAction: (actionType, data) => {
    if (['login', 'register', 'charge', 'change-password'].includes(actionType)) {  // 校验合法操作类型
      ipcRenderer.send('app-action', actionType, data)
    }
  },
  onResponse: (callback) => ipcRenderer.on('action-response', callback)
})


window.addEventListener('DOMContentLoaded', () => {
  const replaceText = (selector, text) => {
    const element = document.getElementById(selector)
    if (element) element.innerText = text
  }

  for (const type of ['chrome', 'node', 'electron']) {
    replaceText(`${type}-version`, process.versions[type])
  }
})
