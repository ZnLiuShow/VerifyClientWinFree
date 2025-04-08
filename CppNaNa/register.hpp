#ifndef REGISTER_HPP
#define REGISTER_HPP

#include "login.hpp"

class RegisterManager : public LoginManager {
public:
    json registerUser(
        const std::string& user,
        const std::string& password,
        const std::string& question,
        const std::string& answer,
        const json& cards = nullptr
    );

private:
    using LoginManager::sha512;
};

#endif