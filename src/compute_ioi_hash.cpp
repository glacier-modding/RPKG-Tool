#include "generic_function.h"
#include <string>
#include <iomanip>
#include <sstream>
#include "thirdparty/md5/md5.h"

extern "C" void MD5Init(struct MD5Context* ctx);
extern "C" void MD5Update(struct MD5Context* ctx, unsigned char* buf, unsigned len);
extern "C" void MD5Final(unsigned char digest[16], struct MD5Context* ctx);
extern "C" void MD5Transform(uint32 buf[4], uint32 in[16]);

std::string generic_function::compute_ioi_hash(const std::string& input_to_ioi_hash) {
    unsigned char signature[16];
    struct MD5Context md5c;

    std::string lowercase;

    for (const char i : input_to_ioi_hash) {
        lowercase += std::tolower(i);
    }

    //LOG(main_data->console_prefix << "Input: " << main_data->input_to_ioi_hash << ", " << lowercase);

    MD5Init(&md5c);
    MD5Update(&md5c, (unsigned char*) lowercase.c_str(), static_cast<unsigned int>(lowercase.length()));
    MD5Final(signature, &md5c);

    std::stringstream ss;

    ss << "00";

    for (uint64_t m = 1; m < 8; m++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(signature[m]);
    }

    return ss.str();
}
