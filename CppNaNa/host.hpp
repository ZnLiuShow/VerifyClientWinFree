#include <vector>
#include <string>

struct NetData {
    std::string mytoken;
    std::vector<unsigned char> aeskey;
};


extern std::vector<unsigned char> keyBuffer;
extern NetData netdata;
extern std::string hostaddr;