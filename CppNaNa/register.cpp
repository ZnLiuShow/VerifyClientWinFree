// Welcome to qq group: 1030115250
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
        // ���ɹ�ϣֵ
        std::string passwordHash = sha512(password);
        std::string questionHash = sha512(question);
        std::string answerHash = sha512(answer);

        // ����������
        json requestBody;
        requestBody["user"] = user;
        requestBody["password"] = passwordHash;
        requestBody["question"] = questionHash;
        requestBody["answer"] = answerHash;

        // �������cards�ֶ�
        if (!cards.is_null()) {
            requestBody["cards"] = cards;
        }

        // ��������
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, (hostaddr + "/api/users/register").c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L); // ��ȷ����ΪPOST����
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.dump().c_str());
        // ������ȷ�Ļص�����������ָ��
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LoginManager::WriteCallback);
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

        // ������Ӧ
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            try {
                json errorData = json::parse(readBuffer);
                MessageBoxA(0, errorData.dump().c_str(), 0, 0);
                // ��ȫ��ȡ�ֶΣ�����Ĭ��ֵ
                std::string errorMsg = errorData.value("error", "unknown error");
                std::string reason = errorData.value("reason", "unknown reason");
                throw std::runtime_error("Registration failed: " + errorMsg +
                    " (ԭ��: " + reason + ")");
            }
            catch (const json::exception& e) {
                // ����JSON�����쳣�����ԭʼ��Ӧ
                throw std::runtime_error("Invalid error response: " + std::string(e.what())
                    + "\nResponse Body: " + readBuffer);
            }
        }

        return json::parse(readBuffer);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("User registration failed: " + std::string(e.what()));
    }
}