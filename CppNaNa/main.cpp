// Welcome to qq group: 1030115250
#include <Windows.h>
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/group.hpp>
#include "register.hpp"
#include "recharge.hpp"
#include "change.hpp"
// 定义UTF-8转换宏
#define TR(str) nana::charset(str).to_bytes(nana::unicode::utf8)

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    using namespace nana;

    // 创建主窗口
    form fm(API::make_center(620, 370), {});
    fm.caption(TR("FreeClient"));
 
    // 标签导航
    panel<true> tab_nav(fm, rectangle(10, 10, 600, 30));
    tab_nav.bgcolor(color(240, 240, 240));

    button btn_login(tab_nav, TR("登录"));
    btn_login.move(rectangle(10, 5, 100, 20));
    btn_login.bgcolor(color(0, 128, 255));
    btn_login.fgcolor(color_rgb(0xFFFFFF));

    button btn_register(tab_nav, TR("注册"));
    btn_register.move(rectangle(120, 5, 100, 20));
    btn_register.bgcolor(color(0, 128, 255));
    btn_register.fgcolor(color_rgb(0xFFFFFF));

    button btn_charge(tab_nav, TR("充值"));
    btn_charge.move(rectangle(230, 5, 100, 20));
    btn_charge.bgcolor(color(0, 128, 255));
    btn_charge.fgcolor(color_rgb(0xFFFFFF));

    button btn_change_password(tab_nav, TR("更改密码"));
    btn_change_password.move(rectangle(340, 5, 100, 20));
    btn_change_password.bgcolor(color(0, 128, 255));
    btn_change_password.fgcolor(color_rgb(0xFFFFFF));

    // 标签内容容器
    group tab_content(fm, rectangle(10, 50, 600, 300));
    tab_content.borderless(false);

    // 登录面板
    group login_panel(tab_content, TR("登录"));
    login_panel.move(rectangle(10, 10, 580, 280));
    textbox login_user(login_panel, rectangle(20, 30, 400, 25));
    login_user.tip_string(TR("用户名"));
    textbox login_pass(login_panel, rectangle(20, 70, 400, 25));
    login_pass.tip_string(TR("密码"));
    login_pass.mask('*');
    button login_btn(login_panel, TR("登录"));
    login_btn.move(rectangle(180, 110, 100, 25));

    // 注册面板
    group register_panel(tab_content, TR("注册"));
    register_panel.move(rectangle(10, 10, 580, 280));
    textbox reg_user(register_panel, rectangle(20, 30, 200, 25));
    reg_user.tip_string(TR("用户名"));
    textbox reg_pass(register_panel, rectangle(20, 70, 200, 25));
    reg_pass.tip_string(TR("密码"));
    reg_pass.mask('*');
    textbox reg_question(register_panel, rectangle(20, 110, 200, 25));
    reg_question.tip_string(TR("安全问题"));
    textbox reg_answer(register_panel, rectangle(20, 150, 200, 25));
    reg_answer.tip_string(TR("安全答案"));
    textbox reg_cards(register_panel, rectangle(240, 30, 200, 145));
    reg_cards.multi_lines(true);
    reg_cards.tip_string(TR("卡密（每行一个,可选）"));
    button reg_btn(register_panel, TR("注册"));
    reg_btn.move(rectangle(180, 180, 100, 25));

    // 充值面板
    group charge_panel(tab_content, TR("充值"));
    charge_panel.move(rectangle(10, 10, 580, 280));
    textbox charge_user(charge_panel, rectangle(20, 30, 400, 25));
    charge_user.tip_string(TR("用户名"));
    textbox charge_cards(charge_panel, rectangle(20, 70, 400, 100));
    charge_cards.multi_lines(true);
    charge_cards.tip_string(TR("卡密（每行一个）"));
    button charge_btn(charge_panel, TR("充值"));
    charge_btn.move(rectangle(180, 180, 100, 25));

    // 更改密码面板
    group change_password_panel(tab_content, TR("更改密码"));
    change_password_panel.move(rectangle(10, 10, 580, 280));
    textbox change_user(change_password_panel, rectangle(20, 30, 400, 25));
    change_user.tip_string(TR("用户名"));
    textbox change_new_password(change_password_panel, rectangle(20, 70, 400, 25));
    change_new_password.tip_string(TR("新密码"));
    change_new_password.mask('*');
    textbox change_question(change_password_panel, rectangle(20, 110, 400, 25));
    change_question.tip_string(TR("安全问题"));
    textbox change_answer(change_password_panel, rectangle(20, 150, 400, 25));
    change_answer.tip_string(TR("安全答案"));
    button change_btn(change_password_panel, TR("确认更改"));
    change_btn.move(rectangle(180, 190, 100, 25));

    // 初始显示登录面板
    login_panel.show();
    register_panel.hide();
    charge_panel.hide();
    change_password_panel.hide();

    // 标签切换事件处理
    btn_login.events().click([&]
        {
            login_panel.show();
            register_panel.hide();
            charge_panel.hide();
            change_password_panel.hide();
        });

    btn_register.events().click([&]
        {
            login_panel.hide();
            register_panel.show();
            charge_panel.hide();
            change_password_panel.hide();
        });

    btn_charge.events().click([&]
        {
            login_panel.hide();
            register_panel.hide();
            charge_panel.show();
            change_password_panel.hide();
        });

    btn_change_password.events().click([&]
        {
            login_panel.hide();
            register_panel.hide();
            charge_panel.hide();
            change_password_panel.show();
        });


    // 登录按钮
    login_btn.events().click([&]
        {
            try {
                std::string username = login_user.caption();
                std::string password = login_pass.caption();
                if (username == "" || password == "")
                    throw std::runtime_error(TR("用户名或密码为空"));
                LoginManager lm;
                auto r = lm.login(username, password);
                // 这里添加登录逻辑，例如：
                msgbox mb(fm, TR("登录信息"));
                mb << (r ? TR("登录成功") : TR("登录失败"));
                mb.show();
            }
            catch (const std::exception& e)
            {
                msgbox mb(fm, TR("登录错误"), msgbox::ok);
                mb.icon(msgbox::icon_error) << TR("错误信息:\n ") << e.what();
                mb.show();
            }
        });

    // 注册按钮
    reg_btn.events().click([&]
        {
            try
            {
                std::string user = reg_user.caption();
                std::string pass = reg_pass.caption();
                std::string question = reg_question.caption();
                std::string answer = reg_answer.caption();
                std::string cards = reg_cards.text();

                // 分割卡密
                std::vector<std::string> card_list;
                std::istringstream iss(cards);
                std::string line;
                while (std::getline(iss, line)) {
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
                    if (!line.empty() && line != "") card_list.push_back(line);
                }

                RegisterManager rm;
                json jcards = json::array(); // 确保默认是空数组
                if (!card_list.empty())
                    jcards = card_list;

                auto response = rm.registerUser(user, pass, question, answer, jcards);
                bool success = response["success"];
                std::string message = response["message"];
   
                msgbox mb(fm, TR("注册成功"));
                mb << TR("注册信息已提交\n") << message;
                mb.show();
            }
            catch (const std::exception& e)
            {
                msgbox mb(fm, TR("注册错误"), msgbox::ok);
                mb.icon(msgbox::icon_error) << TR("错误信息: ") << e.what();
                mb.show();
            }
        });

    // 充值按钮
    charge_btn.events().click([&]
        {
            try {
                std::string user = charge_user.caption();
                std::string cards = charge_cards.text();

                // 分割卡密
                std::vector<std::string> card_list;
                std::istringstream iss(cards);
                std::string line;
                while (std::getline(iss, line)) {
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
                    if (!line.empty() && line != "") card_list.push_back(line);
                }


                RechargeManager rm;
                json jcards = json::array(); // 确保默认是空数组
                if (!card_list.empty())
                    jcards = card_list;
                else {
                    msgbox mb(fm, TR("充值失败"));
                    mb << TR("请填写卡密！");
                    mb.show();
                    return;
                }

                auto response = rm.recharge(user, card_list);
                bool success = response["success"];
                std::string message = response["message"];
                // 这里添加充值逻辑，例如：
                msgbox mb(fm, TR("充值成功"));
                mb << TR("用户名: ") << user << "\n"
                    << TR("充值时长:\n") << message;
                mb.show();
            }
            catch (const std::exception& e)
            {
                msgbox mb(fm, TR("充值错误"), msgbox::ok);
                mb.icon(msgbox::icon_error) << TR("错误信息: ") << e.what();
                mb.show();
            }            
        });

    // 修改密码按钮
    change_btn.events().click([&]
        {
            try {
                std::string user = change_user.caption();
                std::string new_pass = change_new_password.caption();
                std::string question = change_question.caption();
                std::string answer = change_answer.caption();

                if (user == "" || new_pass == "" || question == "" || answer == "")
                    throw std::runtime_error(TR("请填写所有必要信息！"));
                ChangeManager cm;
                auto r = cm.changepassword(user, new_pass, question, answer);
                // 这里添加修改密码逻辑，例如：
                msgbox mb(fm, TR("修改密码"));
                mb << (r ?TR("修改密码成功"): TR("修改密码失败"));
                mb.show();
            }
            catch (const std::exception& e)
            {
                msgbox mb(fm, TR("修改密码错误"), msgbox::ok);
                mb.icon(msgbox::icon_error) << TR("错误信息:\n ") << e.what();
                mb.show();
            }     
        });

    // 显示主窗口
    fm.show();

    // 运行消息循环
    exec();

    return 0;
}