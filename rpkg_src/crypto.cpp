#include "crypto.h"

void crypto::xor_data(char *input_data, uint32_t input_data_length)
{
    unsigned char xor_array[] = {0xDC, 0x45, 0xA6, 0x9C, 0xD3, 0x72, 0x4C, 0xAB};
    int xor_length = sizeof(xor_array);

    for (uint64_t i = 0; i < input_data_length; i++)
    {
        input_data[i] ^= xor_array[i % xor_length];
    }
}

void crypto::xtea_encrypt_localization(uint32_t v[2])
{
    const uint32_t key[4] = {0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268};
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
    for (uint64_t i = 0; i < num_rounds; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

void crypto::xtea_decrypt_localization(uint32_t v[2])
{
    const uint32_t key[4] = {0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268};
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = 0xC6EF3720;
    for (uint64_t i = 0; i < num_rounds; i++)
    {
        v1 -= (uint32_t)((uint32_t)((v0 << 4) ^ (v0 >> 5)) + v0 ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
        sum -= (uint32_t)delta;
        v0 -= (uint32_t)((uint32_t)((v1 << 4) ^ (v1 >> 5)) + v1 ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
    }
    v[0] = v0;
    v[1] = v1;
}

void crypto::xtea_encrypt_packagedefinition_thumbs(uint32_t v[2])
{
    const uint32_t key[4] = {0x30F95282, 0x1F48C419, 0x295F8548, 0x2A78366D};
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x61C88647;
    for (uint64_t i = 0; i < num_rounds; i++)
    {
        v0 += (uint32_t)((uint32_t)(((v1 << 4) ^ (v1 >> 5)) + v1) ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
        sum -= delta;
        v1 += (uint32_t)((uint32_t)(((v0 << 4) ^ (v0 >> 5)) + v0) ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
    }
    v[0] = v0;
    v[1] = v1;
}

void crypto::xtea_decrypt_packagedefinition_thumbs(uint32_t v[2])
{
    const uint32_t key[4] = {0x30F95282, 0x1F48C419, 0x295F8548, 0x2A78366D};
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x61C88647, sum = 0xC6EF3720;
    for (uint64_t i = 0; i < num_rounds; i++)
    {
        v1 -= (uint32_t)((uint32_t)(((v0 << 4) ^ (v0 >> 5)) + v0) ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
        sum += (uint32_t)delta;
        v0 -= (uint32_t)((uint32_t)(((v1 << 4) ^ (v1 >> 5)) + v1) ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
    }
    v[0] = v0;
    v[1] = v1;
}

int crypto::symmetric_key_encrypt_localization(char value)
{
    value ^= 226;
    return (value & 0x81) | (value & 2) << 1 | (value & 4) << 2 | (value & 8) << 3 | (value & 0x10) >> 3 | (value & 0x20) >> 2 | (value & 0x40) >> 1;
}

int crypto::symmetric_key_decrypt_localization(char value)
{
    int bits[] = {value & 1, (value & 2) >> 1, (value & 4) >> 2, (value & 8) >> 3, (value & 16) >> 4, (value & 32) >> 5, (value & 64) >> 6, (value & 128) >> 7};
    int output = (bits[0] << 0) | (bits[1] << 4) | (bits[2] << 1) | (bits[3] << 5) | (bits[4] << 2) | (bits[5] << 6) | (bits[6] << 3) | (bits[7] << 7);
    return output ^ 226;
}
