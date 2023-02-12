// Retrieved on 2021-10-30 from https://gist.github.com/timepp/1f678e200d9e0f2a043a9ec6b3690635
// Modified for use in RPKG
#pragma once

#include <cstdint>

struct crc32 {
    static void generate_table(uint32_t(& table)[256]) {
        constexpr uint32_t polynomial = 0xEDB88320;
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (size_t j = 0; j < 8; j++) {
                if (c & 1) {
                    c = polynomial ^ (c >> 1);
                } else {
                    c >>= 1;
                }
            }
            table[i] = c;
        }
    }

    static uint32_t update(const uint32_t(& table)[256], const uint32_t initial, const void* buf, const size_t len) {
        uint32_t c = initial ^ 0xFFFFFFFF;
        const auto* u = static_cast<const uint8_t*>(buf);
        for (size_t i = 0; i < len; ++i) {
            c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
        }
        return c ^ 0xFFFFFFFF;
    }
};
