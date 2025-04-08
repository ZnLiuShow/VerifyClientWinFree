#include "register.hpp"
#include <openssl/sha.h>

json RegisterManager::registerUser(
    const std::string& user,
    const std::string& password,
    const std::string& question,
    const std::string& answer,
    const json& cards)
{
    try {
        // 生成哈希值
        std::string passwordHash = sha512(password);
        std::string questionHash = sha512(question);
        std::string answerHash = sha512(answer);

        // 构建请求体
        json requestBody;
        requestBody["user"] = user;
        requestBody["password"] = passwordHash;
        requestBody["question"] = questionHash;
        requestBody["answer"] = answerHash;

        // 条件添加cards字段
        if (!cards.is_null()) {
            requestBody["cards"] = cards;
        }

        // 发送请求
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/users/register").c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            throw std::runtime_error("Registration failed: " +
                std::string(curl_easy_strerror(res)));
        }

        // 处理响应
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            json errorData = json::parse(readBuffer);
            throw std::runtime_error("Registration failed: " +
                errorData["error"].get<std::string>() +
                " (原因: " + errorData["reason"].get<std::string>() + ")");
        }

        return json::parse(readBuffer);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("User registration failed: " + std::string(e.what()));
    }
}