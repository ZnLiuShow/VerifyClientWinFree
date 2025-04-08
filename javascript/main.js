// Welcome to qq group: 1030115250
// Modules to control application life and create native browser window
const { app, BrowserWindow } = require('electron')
const path = require('node:path')

function createWindow () {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    autoHideMenuBar: true,      
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,  // 确保开启上下文隔离
      nodeIntegration: false    // 禁用Node集成
    }
  })

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })

  // 在 app.whenReady() 后添加
  ipcMain.on('app-action', (event, actionType, data) => {
    let result = { success: false, message: '' }
    
    switch(actionType) {
      case 'login':
        // 添加实际验证逻辑
        result = { success: true, message: '登录成功' }
        break
        
      case 'register':
        // 添加数据库存储逻辑
        result = { success: true, message: '注册成功' }
        break
        
      case 'charge':
        // 添加卡密验证逻辑
        result = { success: true, message: '充值成功' }
        break
    }
    
    event.sender.send('action-response', result)
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
