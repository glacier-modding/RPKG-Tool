#include "crypto.h"

constexpr unsigned int num_rounds = 32;
constexpr uint32_t l10n_key[4] = {0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268};
constexpr uint32_t thumbs_key[4] = {0x30F95282, 0x1F48C419, 0x295F8548, 0x2A78366D};
constexpr uint32_t thumbs_delta = 0x61C88647;
constexpr uint32_t l10n_delta = 0x9E3779B9;

void crypto::xor_data(char* input_data, const uint32_t input_data_length) {
    constexpr unsigned char xor_array[] = {0xDC, 0x45, 0xA6, 0x9C, 0xD3, 0x72, 0x4C, 0xAB};
    constexpr int xor_length = sizeof(xor_array);

    for (uint64_t i = 0; i < input_data_length; i++) {
        input_data[i] ^= xor_array[i % xor_length];
    }
}

void crypto::xtea_encrypt_localization(uint32_t v[2]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0;

    for (uint64_t i = 0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + l10n_key[sum & 3]);
        sum += l10n_delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + l10n_key[(sum >> 11) & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

void crypto::xtea_decrypt_localization(uint32_t v[2]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720;

    for (uint64_t i = 0; i < num_rounds; i++) {
        v1 -= (v0 << 4 ^ v0 >> 5) + v0 ^ sum + l10n_key[sum >> 11 & 3];
        sum -= l10n_delta;
        v0 -= (v1 << 4 ^ v1 >> 5) + v1 ^ sum + l10n_key[sum & 3];
    }

    v[0] = v0;
    v[1] = v1;
}

void crypto::xtea_encrypt_packagedefinition_thumbs(uint32_t v[2]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0;

    for (uint64_t i = 0; i < num_rounds; i++) {
        v0 += (v1 << 4 ^ v1 >> 5) + v1 ^ sum + thumbs_key[sum & 3];
        sum -= thumbs_delta;
        v1 += (v0 << 4 ^ v0 >> 5) + v0 ^ sum + thumbs_key[sum >> 11 & 3];
    }
    v[0] = v0;
    v[1] = v1;
}

void crypto::xtea_decrypt_packagedefinition_thumbs(uint32_t v[2]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720;

    for (uint64_t i = 0; i < num_rounds; i++) {
        v1 -= (v0 << 4 ^ v0 >> 5) + v0 ^ sum + thumbs_key[sum >> 11 & 3];
        sum += static_cast<uint32_t>(thumbs_delta);
        v0 -= (v1 << 4 ^ v1 >> 5) + v1 ^ sum + thumbs_key[sum & 3];
    }

    v[0] = v0;
    v[1] = v1;
}

int crypto::symmetric_key_encrypt_localization(char value) {
    value ^= 226;
    return (value & 0x81) | (value & 2) << 1 | (value & 4) << 2 | (value & 8) << 3 | (value & 0x10) >> 3 |
           (value & 0x20) >> 2 | (value & 0x40) >> 1;
}

int crypto::symmetric_key_decrypt_localization(const char value) {
    const int bits[] = {value & 1, (value & 2) >> 1, (value & 4) >> 2, (value & 8) >> 3, (value & 16) >> 4,
                        (value & 32) >> 5, (value & 64) >> 6, (value & 128) >> 7};
    const int output =
            (bits[0] << 0) | (bits[1] << 4) | (bits[2] << 1) | (bits[3] << 5) | (bits[4] << 2) | (bits[5] << 6) |
            (bits[6] << 3) | (bits[7] << 7);
    return output ^ 226;
}
