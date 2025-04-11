// Welcome to qq group: 1030115250
#include "change.hpp"
#include <ctime>

bool ChangeManager::changepassword(const std::string& username,
    const std::string& newpassword,
    const std::string& question,
    const std::string& answer) {
    try {
        // ���ͳ�ʼ��������
        json initResponse = sendEncryptRequest();

        // ������Ӧ����
        std::string decrypted = AES256GCM::decryptData(
            initResponse["data"].get<std::string>(),
            keyBuffer,
            initResponse["iv"].get<std::string>(),
            initResponse["authTag"].get<std::string>()
        );
        json decryptedData = json::parse(decrypted);
        netdata.aeskey = AES256GCM::base64_decode(decryptedData["data"].get<std::string>());

        // ������������
        json data = {
            {"name", username},
            {"newpassword", sha512(newpassword)},
            {"question", sha512(question)},
            {"answer", sha512(answer)},
            {"timestamp", LoginManager::timestamp_millis()}
        };

        // ��������
        EncryptedData encryptedData = AES256GCM::encryptJSON(data.dump(), netdata.aeskey);

        // ����������
        json requestBody = {
            {"data", encryptedData.data},
            {"tag", encryptedData.tag},
            {"iv", encryptedData.iv}
        };

        std::string postData = requestBody.dump();
        // �����޸���������
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/v1/users/change").c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            throw std::runtime_error("Change password request failed: " +
                std::string(curl_easy_strerror(res)));
        }

        // ������Ӧ
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            json errorData = json::parse(readBuffer);
            throw std::runtime_error("Change password failed: " +
                errorData["error"].get<std::string>() +
                " (ԭ��: " + errorData["reason"].get<std::string>() + ")");
        }

        // ������Ӧ����
        json responseData = json::parse(readBuffer);
        std::string deDataStr = AES256GCM::decryptData(
            responseData["data"].get<std::string>(),
            netdata.aeskey,
            responseData["iv"].get<std::string>(),
            responseData["tag"].get<std::string>()
        );

        json deData = json::parse(deDataStr);
        if (deData.contains("newkey")) {
            netdata.aeskey = AES256GCM::base64_decode(deData["newkey"].get<std::string>());
        }

        return deData["success"].get<bool>();
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Password change failed: " + std::string(e.what()));
    }
}