#ifndef CHANGE_HPP
#define CHANGE_HPP

#include "login.hpp"

class ChangeManager : public LoginManager {
public:
    bool changepassword(const std::string& username,
        const std::string& newpassword,
        const std::string& question,
        const std::string& answer);

private:
    using LoginManager::sendEncryptRequest;
    using LoginManager::sha512;
};

#endif