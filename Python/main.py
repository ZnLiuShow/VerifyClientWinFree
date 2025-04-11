import sys
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (QApplication, QWidget, QTabWidget, QLineEdit,
                             QPushButton, QTextEdit, QVBoxLayout, QHBoxLayout,
                             QLabel, QFormLayout, QSpacerItem, QSizePolicy, QMessageBox)
from src.register import register
from src.recharge import recharge
from src.login import login
from src.change import changepassword
class StyledTabWidget(QTabWidget):
    def __init__(self):
        super().__init__()
        self.setStyleSheet("""
            QTabWidget::pane {
                border: none;
                background: #f5f6fa;
            }
            QTabBar::tab {
                background: #dcdde1;
                color: #2f3640;
                padding: 10px 20px;
                border-top-left-radius: 5px;
                border-top-right-radius: 5px;
                margin-right: 2px;
                font-size: 14px;
            }
            QTabBar::tab:selected {
                background: #487eb0;
                color: white;
            }
            QTabBar::tab:hover {
                background: #7f8fa6;
                color: white;
            }
        """)

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.setWindowTitle('FreeClient')
        self.setMinimumSize(500, 500)
        self.setStyleSheet("background: #f5f6fa;")

        self.bind_buttons()
        
    def initUI(self):
        # 主布局
        main_layout = QVBoxLayout()
        main_layout.setContentsMargins(20, 20, 20, 20)
        main_layout.setSpacing(20)

        # 创建选项卡
        self.tabs = StyledTabWidget()
        
        # 创建各个页面
        self.login_tab = self.create_login_tab()
        self.register_tab = self.create_register_tab()
        self.charge_tab = self.create_charge_tab()
        self.change_pwd_tab = self.create_change_pwd_tab()

        # 添加选项卡
        self.tabs.addTab(self.login_tab, "登录")
        self.tabs.addTab(self.register_tab, "注册")
        self.tabs.addTab(self.charge_tab, "充值")
        self.tabs.addTab(self.change_pwd_tab, "更改密码")

        main_layout.addWidget(self.tabs)
        self.setLayout(main_layout)

    def create_input_field(self, placeholder, password=False):
        field = QLineEdit()
        field.setPlaceholderText(placeholder)
        field.setMinimumHeight(40)
        field.setStyleSheet("""
            QLineEdit {
                background: white;
                border: 2px solid #dcdde1;
                border-radius: 5px;
                padding: 8px;
                font-size: 14px;
            }
            QLineEdit:focus {
                border-color: #487eb0;
            }
        """)
        if password:
            field.setEchoMode(QLineEdit.Password)
        return field

    def create_button(self, text):
        btn = QPushButton(text)
        btn.setMinimumHeight(40)
        btn.setCursor(Qt.PointingHandCursor)
        btn.setStyleSheet("""
            QPushButton {
                background: #487eb0;
                color: white;
                border: none;
                border-radius: 5px;
                font-size: 14px;
                font-weight: bold;
            }
            QPushButton:hover {
                background: #40739e;
            }
            QPushButton:pressed {
                background: #487eb0;
            }
        """)
        return btn

    def create_login_tab(self):
        tab = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(30, 30, 30, 30)
        layout.setSpacing(20)

        self.login_user = self.create_input_field("用户名")
        self.login_pass = self.create_input_field("密码", password=True)
        self.login_btn = self.create_button("立即登录")

        layout.addWidget(QLabel("欢迎回来！"))
        layout.addWidget(self.login_user)
        layout.addWidget(self.login_pass)
        layout.addItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))
        layout.addWidget(self.login_btn)
        tab.setLayout(layout)
        return tab

    def create_register_tab(self):
        tab = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(30, 30, 30, 30)
        layout.setSpacing(15)

        form_layout = QFormLayout()
        form_layout.setLabelAlignment(Qt.AlignRight)
        form_layout.setVerticalSpacing(15)

        self.reg_user = self.create_input_field("用户名")
        self.reg_pass = self.create_input_field("密码", password=True)
        self.reg_question = self.create_input_field("安全问题")
        self.reg_answer = self.create_input_field("安全答案")
        
        form_layout.addRow("用户名：", self.reg_user)
        form_layout.addRow("密码：", self.reg_pass)
        form_layout.addRow("安全问题：", self.reg_question)
        form_layout.addRow("安全答案：", self.reg_answer)

        self.reg_cards = QTextEdit()
        self.reg_cards.setPlaceholderText("卡密（每行一个，可选）")
        self.reg_cards.setStyleSheet("""
            QTextEdit {
                background: white;
                border: 2px solid #dcdde1;
                border-radius: 5px;
                padding: 8px;
                font-size: 14px;
                min-height: 80px;
            }
        """)
        self.reg_btn = self.create_button("立即注册")

        layout.addLayout(form_layout)
        layout.addWidget(QLabel("卡密："))
        layout.addWidget(self.reg_cards)
        layout.addItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))
        layout.addWidget(self.reg_btn)
        tab.setLayout(layout)
        return tab

    def create_charge_tab(self):
        tab = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(30, 30, 30, 30)
        layout.setSpacing(20)

        self.charge_user = self.create_input_field("用户名")
        self.charge_cards = QTextEdit()
        self.charge_cards.setPlaceholderText("卡密（每行一个）")
        self.charge_cards.setStyleSheet("""
            QTextEdit {
                background: white;
                border: 2px solid #dcdde1;
                border-radius: 5px;
                padding: 8px;
                font-size: 14px;
                min-height: 100px;
            }
        """)
        self.charge_btn = self.create_button("立即充值")

        layout.addWidget(QLabel("充值信息"))
        layout.addWidget(self.charge_user)
        layout.addWidget(QLabel("卡密："))
        layout.addWidget(self.charge_cards)
        layout.addItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))
        layout.addWidget(self.charge_btn)
        tab.setLayout(layout)
        return tab

    def create_change_pwd_tab(self):
        tab = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(30, 30, 30, 30)
        layout.setSpacing(15)

        form_layout = QFormLayout()
        form_layout.setLabelAlignment(Qt.AlignRight)
        form_layout.setVerticalSpacing(15)

        self.change_user = self.create_input_field("用户名")
        self.change_new_pass = self.create_input_field("新密码", password=True)
        self.change_question = self.create_input_field("安全问题")
        self.change_answer = self.create_input_field("安全答案")

        form_layout.addRow("用户名：", self.change_user)
        form_layout.addRow("新密码：", self.change_new_pass)
        form_layout.addRow("安全问题：", self.change_question)
        form_layout.addRow("安全答案：", self.change_answer)

        self.change_btn = self.create_button("确认更改")

        layout.addLayout(form_layout)
        layout.addItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))
        layout.addWidget(self.change_btn)
        tab.setLayout(layout)
        return tab
    
    def bind_buttons(self):
        """绑定所有按钮的点击事件"""
        self.login_btn.clicked.connect(self.handle_login)
        self.reg_btn.clicked.connect(self.handle_register)
        self.charge_btn.clicked.connect(self.handle_charge)
        self.change_btn.clicked.connect(self.handle_change_password)

    def handle_login(self):
        """登录按钮处理逻辑"""
        username = self.login_user.text().strip()
        password = self.login_pass.text().strip()
        
        if not username or not password:
            self.show_warning("登录", "用户名和密码不能为空")
            return
        
        try: 
            r = login(username, password)
            if r:    
                self.show_success("登录成功", f"欢迎回来，{username}！")
                self.clear_login_fields()
            else:
                self.show_error("登录失败", "用户名或密码错误")
        except Exception as e:
            self.show_error("登录失败", str(e))

    def handle_register(self):
        """注册按钮处理逻辑"""
        data = {
            'username': self.reg_user.text().strip(),
            'password': self.reg_pass.text().strip(),
            'question': self.reg_question.text().strip(),
            'answer': self.reg_answer.text().strip(),
            'cards': self.reg_cards.toPlainText().split()
        }
        
        if not data['username'] or not data['password']:
            self.show_warning("注册", "用户名和密码为必填项")
            return
            
        if not data['question'] or not data['answer']:
            self.show_warning("注册", "安全问题和答案必须填写")
            return
        try: 
            response = register(data['username'], data['password'], data['question'], data['answer'], data['cards'])   
            
            if response.get('success', False):
                self.show_success("注册成功", "账号已创建，请妥善保管安全信息")
                self.clear_register_fields()
            else:
                error_msg = response.get('message', '未知错误')
                self.show_error("注册失败", error_msg)
        except Exception as e:
            self.show_error("注册失败", str(e))

    def handle_charge(self):
        """充值按钮处理逻辑"""
        username = self.charge_user.text().strip()
        cards = self.charge_cards.toPlainText().split()
        
        if not username:
            self.show_warning("充值", "请输入用户名")
            return
            
        if len(cards) < 1:
            self.show_warning("充值", "至少需要输入一个卡密")
            return
        try:
            response = recharge(username, cards)
            if not response.get('success', False):
                error_msg = response.get('message', '未知错误')
                self.show_error("充值失败", error_msg)
                return
            else:
                self.show_success("充值成功", f"卡密已成功充值\n{response.get('message', '0 hours')}")
                self.clear_register_fields()        
        except Exception as e:
            self.show_error("充值失败", str(e))         
                

    def handle_change_password(self):
        """修改密码处理逻辑"""
        username = self.change_user.text().strip()
        new_pass = self.change_new_pass.text().strip()
        question = self.change_question.text().strip()
        answer = self.change_answer.text().strip()
        
        if not all([username, new_pass, question, answer]):
            self.show_warning("修改密码", "所有字段均为必填项")
            return
        
        try:
            r = changepassword(username, new_pass, question, answer)
            if r:
                self.show_success("修改密码成功", "密码已成功修改")
                self.clear_change_fields()
            else:
                self.show_error("修改密码失败", "修改密码失败")
        except Exception as e:
            self.show_error("修改密码失败", str(e))


    # 辅助方法
    def show_warning(self, title, message):
        QMessageBox.warning(self, title, message)
        
    def show_error(self, title, message):
        QMessageBox.critical(self, title, message)
        
    def show_success(self, title, message):
        QMessageBox.information(self, title, message)

    # 清空字段方法
    def clear_login_fields(self):
        self.login_user.clear()
        self.login_pass.clear()
        
    def clear_register_fields(self):
        self.reg_user.clear()
        self.reg_pass.clear()
        self.reg_question.clear()
        self.reg_answer.clear()
        self.reg_cards.clear()
        
    def clear_charge_fields(self):
        self.charge_user.clear()
        self.charge_cards.clear()
        
    def clear_change_fields(self):
        self.change_user.clear()
        self.change_new_pass.clear()
        self.change_question.clear()
        self.change_answer.clear()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())