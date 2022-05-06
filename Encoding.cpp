#include "Encoding.h"

uint8_t ParseNibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';

    return 0;
};

std::vector<uint8_t> DecodeParameterString(TJBox_Value iString) {
    auto stringLength = JBox_GetStringLength(iString);
    JBOX_ASSERT(stringLength % 2 == 0);

    std::vector<char> buf(stringLength + 1);
    JBox_GetSubstring(iString, 0, stringLength, buf.data());

    std::vector<uint8_t> ret;
    for (int i = 0; i < stringLength; i += 2) {
        const uint8_t upper = ParseNibble(buf[i]) << 4;
        const uint8_t lower = ParseNibble(buf[i + 1]);
        ret.push_back(upper | lower);
    }

    return ret;
}

uint32_t DecodeParameterString(TJBox_Value iString, char* oBuffer,
                               uint32_t iSize) {
    uint32_t stringLength = JBox_GetStringLength(iString);
    JBOX_ASSERT(stringLength % 2 == 0);

    // Buffer too small
    stringLength = std::min(stringLength, iSize);
    JBox_GetSubstring(iString, 0, stringLength, oBuffer);

    // Reuse buffer for decoding
    for (int i = 0; i < stringLength / 2; i++) {
        const uint8_t upper = ParseNibble(oBuffer[2 * i]) << 4;
        const uint8_t lower = ParseNibble(oBuffer[2 * i + 1]);
        oBuffer[i] = upper | lower;
    }

    return stringLength / 2;
}