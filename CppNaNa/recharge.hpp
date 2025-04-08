// Welcome to qq group: 1030115250
#ifndef RECHARGE_HPP
#define RECHARGE_HPP

#include "login.hpp"

class RechargeManager : public LoginManager {
public:
    json recharge(const std::string& user, const json& cards);
};

#endif