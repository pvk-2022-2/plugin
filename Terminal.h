#pragma once

#include "Jukebox.h"
#include <cstddef>
#include <cstdint>

class CTerminal {
public:
    CTerminal();
    void Putch(const char c);
    void Puts(const char* s);
    void PutHexLen(uint64_t iValue, uint64_t iNibbles);
    void PutHex(uint64_t iValue);
    void PutUInt(uint64_t iValue);
    void PutInt(int64_t iValue);

    void SendProperties();
    void ScrollDown();

private:
    static const size_t kTextBufferLength = 2048;
    static const size_t kOutBufferLength = 1024;

    static const size_t kTerminalWidth = 50;
    static const size_t kTerminalHeight = 20;

    // Make sure entire terminal can be printed.
    static_assert(kOutBufferLength >= kTerminalWidth * kTerminalHeight);

    TJBox_PropertyRef fTextOutRef;
    uint64_t fBufferIndex = 0;
    uint64_t fOutIndex = 0;

    bool fDirty = false;

    // The maximum supported rate of updates to a single value is once every 25
    // milliseconds. The precision doesnt matter that much since normally you
    // wont be writing text every frame.
    static const uint64_t fMinFrameWait =
        static_cast<uint64_t>(25.0 * 96.0 / 64.0) + 1;
    uint64_t fFramesSinceSend = 0;

    uint64_t fTerminalY = 0;
    uint64_t fTerminalX = 0;

    // Potential Optimization, use two mom buffers to skip outbuffer entirely
    uint8_t fTextBuffer[kTextBufferLength];
    uint8_t fOutBuffer[kOutBufferLength];
};
