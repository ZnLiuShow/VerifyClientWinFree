use eframe::egui::{self, FontDefinitions, FontFamily};
mod network{
    pub mod aesnet;
    pub mod host;
    pub mod register;
    pub mod recharge;
    pub mod login;
    pub mod change;
}
struct MyApp {
    tabs: Vec<String>,
    current_tab: usize,
    login_user: String,
    login_pass: String,
    reg_user: String,
    reg_pass: String,
    reg_question: String,
    reg_answer: String,
    reg_cards: String,
    charge_user: String,
    charge_cards: String,
    change_user: String,
    change_new_pass: String,
    change_question: String,
    change_answer: String,
    show_register_popup: bool,      // 控制弹窗显示
    register_message: String,       // 存储弹窗内容
    show_charge_popup: bool,      // 新增：控制充值弹窗显示
    charge_message: String,       // 新增：存储充值结果信息
    show_login_popup: bool,      // 新增：控制登录弹窗显示
    login_message: String,       // 新增：存储登录结果信息
    show_change_popup: bool,
    change_message: String,
}

impl Default for MyApp {
    fn default() -> Self {
        Self {
            tabs: vec!["登录".into(), "注册".into(), "充值".into(), "改密".into()],
            current_tab: 0,
            login_user: String::new(),
            login_pass: String::new(),
            reg_user: String::new(),
            reg_pass: String::new(),
            reg_question: String::new(),
            reg_answer: String::new(),
            reg_cards: String::new(),
            charge_user: String::new(),
            charge_cards: String::new(),
            change_user: String::new(),
            change_new_pass: String::new(),
            change_question: String::new(),
            change_answer: String::new(),
            show_register_popup: false,
            register_message: String::new(),
            show_charge_popup: false,
            charge_message: String::new(),
            show_login_popup: false,
            login_message: String::new(),
            show_change_popup: false,
            change_message: String::new(),
        }
    }
}

impl eframe::App for MyApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        egui::SidePanel::left("side_panel").show(ctx, |ui| {
            ui.vertical_centered(|ui| {
                for (i, tab) in self.tabs.iter().enumerate() {
                    let button = egui::Button::new(tab)
                       .fill(if i == self.current_tab {
                            egui::Color32::from_rgb(50, 150, 250)
                        } else {
                            egui::Color32::from_rgb(30, 30, 30)
                        })   
                       .corner_radius(5.0)
                       .stroke(egui::Stroke::new(1.0, egui::Color32::from_rgb(100, 100, 100)));
                    if ui.add(button).clicked() {
                        self.current_tab = i;
                    }
                }
            });
        });

        egui::CentralPanel::default().show(ctx, |ui| {
            ui.spacing_mut().item_spacing = egui::vec2(0.0, 10.0);
            ui.set_min_size(egui::vec2(300.0, 0.0));

            // 标签内容
            match self.current_tab {
                0 => self.login_tab(ui),
                1 => self.register_tab(ui),
                2 => self.charge_tab(ui),
                3 => self.change_pw_tab(ui),
                _ => unreachable!(),
            }
        });
    }
}

impl MyApp {
    fn login_tab(&mut self, ui: &mut egui::Ui) {
        ui.heading("登录");
        ui.add(
            egui::TextEdit::singleline(&mut self.login_user)
               .hint_text("用户名")
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
               
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.login_pass)
               .password(true)
               .hint_text("密码")
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))          
        );
        if ui
           .add(
                egui::Button::new("登录")
                   .fill(egui::Color32::from_rgb(50, 150, 250))          
                   .corner_radius(5.0)
                   .stroke(egui::Stroke::new(1.0, egui::Color32::from_rgb(100, 100, 100))),
            )
           .clicked()
        {
            let username = self.login_user.clone();
            let password = self.login_pass.clone();
    
            use futures::executor::block_on;
            let result = block_on(network::login::login(
                &username, 
                &password
            ));
    
            // 处理登录结果
            match result {
                Ok(success) => {
                    if success {
                        self.login_message = "登录成功！".to_string();
                    } else {
                        self.login_message = "登录失败：用户名或密码错误".to_string();
                    }
                },
                Err(e) => {
                    self.login_message = format!("登录错误: {}", e);
                }
            }
            self.show_login_popup = true;
        }

        // 登录结果弹窗
        if self.show_login_popup {
            egui::Window::new("登录结果")
                .collapsible(false)
                .resizable(false)
                .anchor(egui::Align2::CENTER_CENTER, [0.0, 0.0])
                .show(ui.ctx(), |ui| {
                    ui.vertical_centered(|ui| {
                        ui.label(&self.login_message);
                        if ui.button("确定").clicked() {
                            self.show_login_popup = false;
                        }
                    });
                });
        }
    }

    fn register_tab(&mut self, ui: &mut egui::Ui) {
        ui.heading("注册");
        ui.add(
            egui::TextEdit::singleline(&mut self.reg_user)
               .hint_text("用户名")          
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
             
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.reg_pass)
               .password(true)
               .hint_text("密码")      
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
       
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.reg_question)
               .hint_text("安全问题")
            
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
              
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.reg_answer)
               .hint_text("安全答案")
             
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
              
        );
        ui.label("卡密（每行一个，可选）");
        ui.add(
            egui::TextEdit::multiline(&mut self.reg_cards)
               .desired_rows(5)
            
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
              
        );
        if ui
           .add(
                egui::Button::new("注册")
                   .fill(egui::Color32::from_rgb(50, 150, 250))                  
                   .corner_radius(5.0)
                   .stroke(egui::Stroke::new(1.0, egui::Color32::from_rgb(100, 100, 100))),
            )
           .clicked()
        {
            let user = self.reg_user.clone();
            let password = self.reg_pass.clone();
            let question = self.reg_question.clone();
            let answer = self.reg_answer.clone();
            let cards: Vec<&str> = self.reg_cards
                .split('\n')
                .map(|s| s.trim())
                .filter(|s| !s.is_empty())
                .collect();

            let cards_option = if cards.is_empty() { None } else { Some(cards.as_slice()) };
            
            use futures::executor::block_on;
            let result = block_on(network::register::register(
                &user, &password, &question, &answer, cards_option
            ));
        
            // 更新弹窗状态
            match result {
                Ok(response) => {
                    self.register_message = format!(
                        "注册成功:\n{}", 
                        serde_json::to_string_pretty(&response).unwrap()
                    );
                },
                Err(e) => {
                    self.register_message = format!("注册失败: {}", e);
                }
            }
            self.show_register_popup = true;  // 显示弹窗
        
            println!("用户注册: {}", user);
        }

        if self.show_register_popup {
            egui::Window::new("注册结果")
                .collapsible(false)
                .resizable(false)
                .anchor(egui::Align2::CENTER_CENTER, [0.0, 0.0])
                .show(ui.ctx(), |ui| {
                    ui.vertical_centered(|ui| {
                        ui.label(&self.register_message);
                        if ui.button("确定").clicked() {
                            self.show_register_popup = false;
                        }
                    });
                });
        }
    }

    fn charge_tab(&mut self, ui: &mut egui::Ui) {
        ui.heading("充值");
        ui.add(
            egui::TextEdit::singleline(&mut self.charge_user)
               .hint_text("用户名")      
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))               
        );
        ui.label("卡密（每行一个）");
        ui.add(
            egui::TextEdit::multiline(&mut self.charge_cards)
               .desired_rows(5)            
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
               
        );
        if ui
           .add(
                egui::Button::new("充值")
                   .fill(egui::Color32::from_rgb(50, 150, 250))          
                   .corner_radius(5.0)
                   .stroke(egui::Stroke::new(1.0, egui::Color32::from_rgb(100, 100, 100))),
            )
           .clicked()
        {
            let user = self.charge_user.clone();
            let cards: Vec<&str> = self.charge_cards       
                .split('\n')
                .map(|s| s.trim())
                .filter(|s| !s.is_empty())
                .collect();

            use futures::executor::block_on;
            let result = block_on(network::recharge::recharge(&user, &cards));
            
            // 处理充值结果
            match result {
                Ok(response) => {
                    self.charge_message = format!(
                        "充值成功:\n{}", 
                        serde_json::to_string_pretty(&response).unwrap()
                    );
                },
                Err(e) => {
                    self.charge_message = format!("充值失败: {}", e);
                }
            }
            self.show_charge_popup = true;
            
            println!("用户充值: {}", self.charge_user);
        }

        // 充值结果弹窗
        if self.show_charge_popup {
            egui::Window::new("充值结果")
                .collapsible(false)
                .resizable(false)
                .anchor(egui::Align2::CENTER_CENTER, [0.0, 0.0])
                .show(ui.ctx(), |ui| {
                    ui.vertical_centered(|ui| {
                        ui.label(&self.charge_message);
                        if ui.button("确定").clicked() {
                            self.show_charge_popup = false;
                        }
                    });
                });
        }
    }

    fn change_pw_tab(&mut self, ui: &mut egui::Ui) {
        ui.heading("更改密码");
        ui.add(
            egui::TextEdit::singleline(&mut self.change_user)
               .hint_text("用户名")   
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
              
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.change_new_pass)
               .password(true)
               .hint_text("新密码")
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
               
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.change_question)
               .hint_text("安全问题")

               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
              
        );
        ui.add(
            egui::TextEdit::singleline(&mut self.change_answer)
               .hint_text("安全答案")
       
               .text_color(egui::Color32::WHITE)
               .background_color(egui::Color32::from_rgb(30, 30, 30))
             
        );
        if ui
           .add(
                egui::Button::new("确认更改")
                   .fill(egui::Color32::from_rgb(50, 150, 250))
                  
                   .corner_radius(5.0)
                   .stroke(egui::Stroke::new(1.0, egui::Color32::from_rgb(100, 100, 100))),
            )
           .clicked()
        {
            let username = self.change_user.clone();
            let new_password = self.change_new_pass.clone();
            let question = self.change_question.clone();
            let answer = self.change_answer.clone();

            use futures::executor::block_on;
            let result = block_on(network::change::changepassword(
                &username,
                &new_password,
                &question,
                &answer,
            ));

            match result {
                Ok(success) => {
                    if success {
                        self.change_message = "密码修改成功！".to_string();
                    } else {
                        self.change_message = "密码修改失败：验证信息错误".to_string();
                    }
                },
                Err(e) => {
                    self.change_message = format!("修改错误: {}", e);
                }
            }
            self.show_change_popup = true;
        }

        if self.show_change_popup {
            egui::Window::new("修改结果")
                .collapsible(false)
                .resizable(false)
                .anchor(egui::Align2::CENTER_CENTER, [0.0, 0.0])
                .show(ui.ctx(), |ui| {
                    ui.vertical_centered(|ui| {
                        ui.label(&self.change_message);
                        if ui.button("确定").clicked() {
                            self.show_change_popup = false;
                        }
                    });
                });
        }
    }
}

fn load_fonts() -> FontDefinitions {
    let mut fonts = FontDefinitions::default();

    // 添加Windows系统字体
    #[cfg(target_os = "windows")]
    fonts
       .families
       .get_mut(&FontFamily::Proportional)
       .unwrap()
       .insert(0, "Microsoft YaHei".to_owned());

    // 添加macOS系统字体
    #[cfg(target_os = "macos")]
    fonts
       .families
       .get_mut(&FontFamily::Proportional)
       .unwrap()
       .insert(0, "PingFang SC".to_owned());

    // 添加Linux系统字体
    #[cfg(target_os = "linux")]
    fonts
       .families
       .get_mut(&FontFamily::Proportional)
       .unwrap()
       .insert(0, "Noto Sans CJK SC".to_owned());

    fonts
}

#[tokio::main]
async fn main() -> eframe::Result<()> {  // 修改返回类型

    let options = eframe::NativeOptions::default();
    
    eframe::run_native(
        "FreeClient",
        options,
        Box::new(|cc| {
            // 1. 加载字体数据
            let mut fonts = egui::FontDefinitions::default();

            // 2. 从字节数据加载字体（需要实际字体文件）
            fonts.font_data.insert(
                "chinese_font".to_owned(),
                egui::FontData::from_static(include_bytes!("../fonts/SourceHanSansCN-Bold.otf")).into(),
            );

            // 3. 设置默认字体
            fonts
               .families
               .entry(egui::FontFamily::Proportional)
               .or_default()
               .insert(0, "chinese_font".to_owned());

            cc.egui_ctx.set_fonts(fonts);

            Ok(Box::new(MyApp::default()))
        }),
    )
}