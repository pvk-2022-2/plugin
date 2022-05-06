#pragma once
#include <cstdint>
#include <vector>

#include "Jukebox.h"

std::vector<uint8_t> DecodeParameterString(TJBox_Value iString);
uint32_t DecodeParameterString(TJBox_Value iString, char* oBuffer,
                               uint32_t iSize);
uint8_t ParseNibble(char c);