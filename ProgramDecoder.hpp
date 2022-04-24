#pragma once
#include <cstdint>
#include <vector>

#include "Jukebox.h"

// Decodes a property string to a uint8_t vector
inline std::vector<uint8_t> DecodeParameterString(TJBox_Value iString) {
    // REPLACE THIS WITH BETTER ENCODING FORMAT LATER
    auto stringLength = JBox_GetStringLength(iString);
    JBOX_ASSERT(stringLength % 2 == 0);

    std::vector<char> buf(stringLength + 1);
    JBox_GetSubstring(iString, 0, stringLength, buf.data());

    const auto parse = [&](int index) {
        const char c = buf[index];

        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return 10 + c - 'A';
        if (c >= 'a' && c <= 'f') return 10 + c - 'a';

        return 0;
    };

    std::vector<uint8_t> ret;
    for (int i = 0; i < stringLength; i += 2) {
        uint8_t val = (parse(i) << 4) | parse(i + 1);
        ret.push_back(val);
    }

    return ret;
}
