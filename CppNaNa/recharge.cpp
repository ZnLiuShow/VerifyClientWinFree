// Welcome to qq group: 1030115250
#include "recharge.hpp"

json RechargeManager::recharge(const std::string& user, const json& cards) {
    try {
        // ����������
        json requestBody;
        requestBody["user"] = user;
        requestBody["cards"] = cards;

        // ����CURL
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/users/addexpiry").c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // ��������ͷ
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // ִ������
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            throw std::runtime_error("Recharge request failed: " +
                std::string(curl_easy_strerror(res)));
        }

        // �����Ӧ״̬��
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            json errorData = json::parse(readBuffer);
            throw std::runtime_error("Recharge failed: " +
                errorData["error"].get<std::string>() +
                " (ԭ��: " + errorData["reason"].get<std::string>() + ")");
        }

        return json::parse(readBuffer);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Card recharge failed: " + std::string(e.what()));
    }
}