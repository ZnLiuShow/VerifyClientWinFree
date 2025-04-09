// Welcome to qq group: 1030115250
#include "login.hpp"
#include <openssl/sha.h>
#include <sstream>
#include <stdexcept>

LoginManager::LoginManager() : curl(curl_easy_init()) {
    if (!curl) throw std::runtime_error("CURL initialization failed");

    // 生成32字节随机密钥
    keyBuffer.resize(32);
    if (RAND_bytes(keyBuffer.data(), 32) != 1) {
        throw std::runtime_error("Key generation failed");
    }
}

LoginManager::~LoginManager() {
    if (curl) curl_easy_cleanup(curl);
}

size_t LoginManager::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    }
    catch (...) {
        return 0;
    }
}

json LoginManager::sendEncryptRequest() {
    std::string readBuffer;
    std::string keyBase64 = AES256GCM::base64_encode(keyBuffer.data(), keyBuffer.size());

    json requestBody = { {"key", keyBase64} };
    std::string postData = requestBody.dump();

    curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/users").c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        throw std::runtime_error("Request failed: " + std::string(curl_easy_strerror(res)));
    }

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code != 200) {
        json errorData = json::parse(readBuffer);
        throw std::runtime_error("Request failed: " + errorData["error"].get<std::string>()
            + " (原因: " + errorData["reason"].get<std::string>() + ")");
    }

    return json::parse(readBuffer);
}

std::string LoginManager::sha512(const std::string& input) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        throw std::runtime_error("Failed to create EVP context");
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha512(), nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to initialize SHA512 digest");
    }

    if (EVP_DigestUpdate(mdctx, input.c_str(), input.size()) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to update SHA512 digest");
    }

    unsigned int hash_len;
    if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1 || hash_len != SHA512_DIGEST_LENGTH) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to finalize SHA512 digest");
    }
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    return ss.str();
}

bool LoginManager::login(const std::string& username, const std::string& password) {
    try {
        json initResponse = sendEncryptRequest();

        // 解密响应数据
        std::string decrypted = AES256GCM::decryptData(
            initResponse["data"].get<std::string>(),
            keyBuffer,
            initResponse["iv"].get<std::string>(),
            initResponse["authTag"].get<std::string>()
        );

        json decryptedData = json::parse(decrypted);
        netdata.aeskey = AES256GCM::base64_decode(decryptedData["data"].get<std::string>());

        // 构造登录数据
        json data = {
            {"user", username},
            {"password", sha512(password)},
            {"timestamp", time(nullptr)}
        };

        EncryptedData encryptedData = AES256GCM::encryptJSON(data.dump(), netdata.aeskey);

        // 发送登录请求
        json loginRequestBody = {
            {"data", encryptedData.data},
            {"tag", encryptedData.tag},
            {"iv", encryptedData.iv}
        };

        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/users/login").c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, loginRequestBody.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            throw std::runtime_error("Login request failed: " + std::string(curl_easy_strerror(res)));
        }

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            json errorData = json::parse(readBuffer);
            throw std::runtime_error("Login failed: " + errorData["error"].get<std::string>()
                + " (原因: " + errorData["reason"].get<std::string>() + ")");
        }

        json responseData = json::parse(readBuffer);
        std::string deDataStr = AES256GCM::decryptData(
            responseData["data"].get<std::string>(),
            netdata.aeskey,
            responseData["iv"].get<std::string>(),
            responseData["tag"].get<std::string>()
        );

        json deData = json::parse(deDataStr);
        if (deData["success"].get<bool>()) {
            netdata.mytoken = deData["token"].get<std::string>();
        }
        if (deData.contains("newkey")) {
            netdata.aeskey = AES256GCM::base64_decode(deData["newkey"].get<std::string>());
        }

        return deData["success"].get<bool>();
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Login failed: " + std::string(e.what()));
    }
}