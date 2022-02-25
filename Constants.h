#pragma once

#include "JukeboxTypes.h"

#include <stddef.h>

const std::uint8_t kNoteVelocity = 100;
const std::uint16_t kDefaultNumeratorNote = 68;
const std::uint16_t kDefaultDenominatorNote = 70;
const std::size_t kBatchSize = 64;
const std::uint64_t kPPQResolution = 15360;
const std::uint8_t kInvalidNoteNumber = 128;

const TJBox_Tag kOnOffTag = 1;
const TJBox_Tag kNumeratorTag = 101;
const TJBox_Tag kDenominatorTag = 102;

const TJBox_Tag kProgramTag = 1000;
