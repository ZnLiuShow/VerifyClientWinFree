// Welcome to qq group: 1030115250
// Modules to control application life and create native browser window
const { app, BrowserWindow , ipcMain} = require('electron')
const path = require('node:path')

const { login } = require('./src/login.js')
const { register } = require('./src/register.js')
const { recharge } = require('./src/recharge.js')
const { changepassword } = require('./src/change.js')

function createWindow () {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    autoHideMenuBar: true,      
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,  // 确保开启上下文隔离
      nodeIntegration: true   //Node集成
    }
  })

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  // Open the DevTools.
  mainWindow.webContents.openDevTools()
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
  ipcMain.on('app-action', async (event, actionType, data) => {
    let result = { success: false, message: '' }
    
    try {
      switch(actionType) {
        case 'login':
          result.success = await login(data.user, data.pass)
          result.message = result.success ? '登录成功' : '登录失败'
          break
          
        case 'register':
          result = await register(
            data.user,
            data.pass,
            data.question,
            data.answer,
            data.cards
          )
          break
          
        case 'charge':
          result = await recharge(data.user, data.cards)
          break
          
        case 'change-password':
          result.success = await changepassword(
            data.user,
            data.newPassword,
            data.question,
            data.answer
          )
          result.message = result.success ? '密码修改成功' : '密码修改失败'
          break
          
        default:
          result.message = '未知操作类型'
      }
    } catch (error) {
      result.message = error.message
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
