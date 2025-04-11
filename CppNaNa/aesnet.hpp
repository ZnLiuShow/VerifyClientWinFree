// Welcome to qq group: 1030115250
#ifndef AESNET_HPP
#define AESNET_HPP

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstring>

struct EncryptedData {
    std::string data;
    std::string tag;
    std::string iv;
};

class AES256GCM {
public:
    static EncryptedData encryptJSON(const std::string& jsonData, const std::vector<unsigned char>& key) {
        if (key.size() != 32) {
            throw std::runtime_error("Invalid key size. Key must be 32 bytes.");
        }

        // Generate random IV (12 bytes)
        std::vector<unsigned char> iv(12);
        if (RAND_bytes(iv.data(), iv.size()) != 1) {
            throw std::runtime_error("IV generation failed");
        }

        // Create and initialize context
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX creation failed");

        try {
            // Initialize encryption
            if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key.data(), iv.data()) != 1) {
                throw std::runtime_error("Encryption initialization failed");
            }

            // Encrypt data
            std::vector<unsigned char> ciphertext(jsonData.size() + EVP_MAX_BLOCK_LENGTH);
            int len;
            if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                reinterpret_cast<const unsigned char*>(jsonData.data()), jsonData.size()) != 1) {
                throw std::runtime_error("Encryption failed");
            }
            int ciphertext_len = len;

            // Finalize encryption
            if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
                throw std::runtime_error("Encryption finalization failed");
            }
            ciphertext_len += len;

            // Get authentication tag
            std::vector<unsigned char> tag(16);
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, tag.size(), tag.data()) != 1) {
                throw std::runtime_error("Failed to get authentication tag");
            }

            // Build result
            EncryptedData result;
            result.data = base64_encode(ciphertext.data(), ciphertext_len);
            result.tag = base64_encode(tag.data(), tag.size());
            result.iv = base64_encode(iv.data(), iv.size());

            EVP_CIPHER_CTX_free(ctx);
            return result;
        }
        catch (...) {
            EVP_CIPHER_CTX_free(ctx);
            throw;
        }
    }

    static std::string decryptData(const std::string& ciphertext,
        const std::vector<unsigned char>& key,
        const std::string& ivBase64,
        const std::string& tagBase64) {
        // Decode base64
        std::vector<unsigned char> iv = base64_decode(ivBase64);
        std::vector<unsigned char> tag = base64_decode(tagBase64);
        std::vector<unsigned char> ciphertextBytes = base64_decode(ciphertext);

        if (iv.size() != 12) throw std::runtime_error("Invalid IV size");
        if (tag.size() != 16) throw std::runtime_error("Invalid tag size");
        if (key.size() != 32) throw std::runtime_error("Invalid key size");

        // Create and initialize context
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX creation failed");

        try {
            // Initialize decryption
            if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key.data(), iv.data()) != 1) {
                throw std::runtime_error("Decryption initialization failed");
            }

            // Set authentication tag
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag.size(), tag.data()) != 1) {
                throw std::runtime_error("Failed to set authentication tag");
            }

            // Decrypt data
            std::vector<unsigned char> plaintext(ciphertextBytes.size() + EVP_MAX_BLOCK_LENGTH);
            int len;
            if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertextBytes.data(), ciphertextBytes.size()) != 1) {
                throw std::runtime_error("Decryption failed");
            }
            int plaintext_len = len;

            // Finalize decryption
            if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
                throw std::runtime_error("Authentication failed");
            }
            plaintext_len += len;

            EVP_CIPHER_CTX_free(ctx);
            return std::string(plaintext.begin(), plaintext.begin() + plaintext_len);
        }
        catch (...) {
            EVP_CIPHER_CTX_free(ctx);
            throw;
        }
    }

    static std::vector<unsigned char> base64_decode(const std::string& encoded) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO* mem = BIO_new_mem_buf(encoded.data(), encoded.size());
        BIO_push(b64, mem);
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        std::vector<unsigned char> decoded(encoded.size());
        int len = BIO_read(b64, decoded.data(), encoded.size());
        if (len <= 0) throw std::runtime_error("Base64 decode failed");

        BIO_free_all(b64);
        decoded.resize(len);
        return decoded;
    }

    static std::string base64_encode(const unsigned char* data, size_t length) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO* mem = BIO_new(BIO_s_mem());
        BIO_push(b64, mem);
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO_write(b64, data, length);
        BIO_flush(b64);

        char* output;
        long outLen = BIO_get_mem_data(mem, &output);
        std::string result(output, outLen);

        BIO_free_all(b64);
        return result;
    }
};

#endif

