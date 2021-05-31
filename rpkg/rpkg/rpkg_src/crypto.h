#pragma once
#include <stdint.h>

class crypto
{
public:
    static void xor_data(char* input_data, uint32_t input_data_length);
    static void xtea_encrypt_localization(uint32_t v[2]);
    static void xtea_decrypt_localization(uint32_t v[2]);
    static void xtea_encrypt_packagedefinition_thumbs(uint32_t v[2]);
    static void xtea_decrypt_packagedefinition_thumbs(uint32_t v[2]);
};
