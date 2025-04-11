// Welcome to qq group: 1030115250
#ifndef LOGIN_HPP
#define LOGIN_HPP

#include "aesnet.hpp"
#include "host.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <memory>

using json = nlohmann::json;


class LoginManager {
public:
    LoginManager();
    ~LoginManager();

    json sendEncryptRequest();
    bool login(const std::string& username, const std::string& password);
    std::string sha512(const std::string& input);
    long long timestamp_millis();
    CURL* curl;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
};

#endif