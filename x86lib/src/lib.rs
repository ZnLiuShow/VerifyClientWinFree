use libc::c_char;
use std::ffi::CStr;
use lazy_static::lazy_static;

lazy_static! {
    static ref RT: tokio::runtime::Runtime = tokio::runtime::Builder::new_multi_thread()
        .enable_all()
        .build()
        .expect("Failed to create Tokio runtime");
}

mod network{
    pub mod aesnet;
    pub mod host;
    pub mod register;
    pub mod recharge;
    pub mod login;
    pub mod change;
}

#[unsafe(no_mangle)]
pub extern "C" fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[unsafe(no_mangle)]
pub extern "C" fn login86(user: *const c_char, password: *const c_char) -> u64 {
    let _user = unsafe { CStr::from_ptr(user) };
    let _password = unsafe { CStr::from_ptr(password) };
    let username = _user.to_str().unwrap();
    let password = _password.to_str().unwrap();
   
    let result = RT.block_on(async {
        network::login::login(&username, &password).await
    });
    
    let mut r: u64 = 0;
    match result {
        Ok(success) => {
            r = if success { 200 } else { 500 };
        }
        Err(_) => {
            r = 501;
        }
    }
    
    r
}

#[unsafe(no_mangle)]
pub extern "C" fn sethost86(host: *const c_char) -> bool {
    let _host = unsafe { CStr::from_ptr(host) };
    let host = _host.to_str().unwrap();

    network::host::set_host(&host);
    network::host::get_host() == host
}

#[unsafe(no_mangle)]
pub extern "C" fn changepassword86(
    user: *const c_char,
    newpass: *const c_char,
    question: *const c_char,
    answer: *const c_char,
) -> u64 {
    let _user = unsafe { CStr::from_ptr(user) };
    let _newpass = unsafe { CStr::from_ptr(newpass) };
    let _question = unsafe { CStr::from_ptr(question) };
    let _answer = unsafe { CStr::from_ptr(answer) };
    
    let username = _user.to_str().unwrap();
    let newpassword = _newpass.to_str().unwrap();
    let question = _question.to_str().unwrap();
    let answer = _answer.to_str().unwrap();

    let result = RT.block_on(async {
        network::change::changepassword(&username, &newpassword, &question, &answer).await
    });
    
    match result {
        Ok(true) => 200,
        Ok(false) => 500,
        Err(_) => 502,
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn recharge86(
    user: *const c_char,
    cards_ptr: *const *const c_char,
    cards_len: usize,
) -> u64 {
    let _user = unsafe { CStr::from_ptr(user) };
    let username = _user.to_str().unwrap();
    
    let cards = unsafe {
        std::slice::from_raw_parts(cards_ptr, cards_len)
            .iter()
            .map(|&ptr| CStr::from_ptr(ptr).to_str().unwrap())
            .collect::<Vec<&str>>()
    };

    let result = RT.block_on(async {
        network::recharge::recharge(&username, &cards).await
    });
    
    match result {
        Ok(resp) if resp["success"].as_bool().unwrap_or(false) => 200,
        Ok(_) => 500,
        Err(_) => 503,
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn register86(
    user: *const c_char,
    password: *const c_char,
    question: *const c_char,
    answer: *const c_char,
    cards_ptr: *const *const c_char,
    cards_len: usize,
) -> u64 {
    let _user = unsafe { CStr::from_ptr(user) };
    let _password = unsafe { CStr::from_ptr(password) };
    let _question = unsafe { CStr::from_ptr(question) };
    let _answer = unsafe { CStr::from_ptr(answer) };
    
    let username = _user.to_str().unwrap();
    let password = _password.to_str().unwrap();
    let question = _question.to_str().unwrap();
    let answer = _answer.to_str().unwrap();
    
    let cards = if !cards_ptr.is_null() {
        Some(unsafe {
            std::slice::from_raw_parts(cards_ptr, cards_len)
                .iter()
                .map(|&ptr| CStr::from_ptr(ptr).to_str().unwrap())
                .collect::<Vec<&str>>()
        })
    } else {
        None
    };

    let result = RT.block_on(async {
        network::register::register(&username, &password, &question, &answer, cards.as_deref()).await
    });
    
    match result {
        Ok(resp) if resp["success"].as_bool().unwrap_or(false) => 200,
        Ok(_) => 500,
        Err(_) => 504,
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CString;
    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn test_sethost86() {
        let hostsever =CString::new("http://127.0.0.1").unwrap();

        assert_eq!(sethost86(hostsever.as_ptr()), true);  
    }

    #[test]
    fn test_login_success() { 
        let user = CString::new("baba").unwrap();
        let pass = CString::new("123456").unwrap();

        assert_eq!(login86(user.as_ptr(), pass.as_ptr()), 200);  
    }

    #[test]
    fn test_register_success() {       
        let user = CString::new("baba2").unwrap();
        let pass = CString::new("123456").unwrap();
        let question = CString::new("123456").unwrap();
        let answer = CString::new("123456").unwrap();

        assert_eq!(register86(user.as_ptr(), pass.as_ptr(), question.as_ptr(), answer.as_ptr(), std::ptr::null(), 0), 200);
    }
}
