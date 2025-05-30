// Welcome to qq group: 1030115250
#include "recharge.hpp"

json RechargeManager::recharge(const std::string& user, const json& cards) {
    try {
        // 构建请求体
        json requestBody;
        requestBody["user"] = user;
        requestBody["cards"] = cards;

        // 将JSON数据序列化为字符串并保存到局部变量
        std::string postData = requestBody.dump();
        // 配置CURL
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/v1/users/addexpiry").c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L); // 明确设置为POST请求
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());
        // 设置正确的回调函数和数据指针
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LoginManager::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // 设置请求头
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            throw std::runtime_error("Recharge request failed: " +
                std::string(curl_easy_strerror(res)));
        }

        // 检查响应状态码
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            json errorData = json::parse(readBuffer);
            throw std::runtime_error("Recharge failed: " +
                errorData["error"].get<std::string>() +
                " (原因: " + errorData["reason"].get<std::string>() + ")");
        }

        return json::parse(readBuffer);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Card recharge failed: " + std::string(e.what()));
    }
}