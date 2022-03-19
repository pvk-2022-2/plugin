#include "Terminal.h"

CTerminal::CTerminal() {
    TJBox_ObjectRef customPropertiesRef =
        JBox_GetMotherboardObjectRef("/custom_properties");
    fTextOutRef = JBox_MakePropertyRef(customPropertiesRef, "text_out_buffer");
}

void CTerminal::SendProperties() {
    if (!fDirty)
        return;

    if(fFramesSinceSend++ < fMinFrameWait)
        return;

    // Copy string to outbuffer
    TJBox_UInt64 textIndex = fOutIndex;
    TJBox_SizeT currentLength = 0;

    while (fTextBuffer[textIndex] && currentLength < kOutBufferLength) {
        fOutBuffer[currentLength] = fTextBuffer[textIndex];

        currentLength++;
        textIndex = (textIndex + 1) % kTextBufferLength;
    }

    JBox_SetRTStringData(fTextOutRef, currentLength, fOutBuffer);

    fDirty = false;
    fFramesSinceSend = 0;
}

void CTerminal::ScrollDown() {
    fDirty = true;

    while (fTextBuffer[fOutIndex] != '\n') {
        if (fTextBuffer[fOutIndex] == 0 || fOutIndex == fBufferIndex)
            return;

        fOutIndex = (fOutIndex + 1) % kTextBufferLength;
    }

    // Start at first character after newline
    fOutIndex++;
}

void CTerminal::Putch(const char c) {
    fDirty = true;

    fTextBuffer[fBufferIndex] = c;
    fBufferIndex = (fBufferIndex + 1) % kTextBufferLength;

    // Handle New Line
    if (c == '\n') {
        fTerminalY++;
        fTerminalX = 0;
    } else {
        fTerminalX++;
    }

    // Automatic Wrapping
    if (fTerminalX >= kTerminalWidth)
        Putch('\n');

    // Automatic Scrolling
    if (fTerminalY >= kTerminalHeight) {
        fTerminalY--;

        ScrollDown();
    }

    fTextBuffer[fBufferIndex] = 0;
}

void CTerminal::Puts(const char *s) {
    while (*s)
        Putch(*(s++));
}

void CTerminal::PutHexLen(uint64_t iValue, uint64_t iNibbles) {
    Putch('0');
    Putch('x');

    for (TJBox_UInt64 i = 0; i < iNibbles; i++) {
        const int k = (iNibbles - 1 - i) * 4;
        const TJBox_UInt8 num = (iValue & (0xFUL << k)) >> k;
        char c = '0' + num;

        if (num >= 10) c = 'A' + num - 10;

        Putch(c);
    }
}

void CTerminal::PutHex(uint64_t iValue) {
    // find largest nonzero nibble
    TJBox_UInt64 nibbles = 16;
    while (nibbles > 2 && !(iValue & (0xFUL << 4 * --nibbles)))
        ;

    PutHexLen(iValue, ++nibbles);
}


void CTerminal::PutUInt(uint64_t iValue) {
    if (iValue == 0) {
        Putch('0');
        return;
    }

    char buf[21];
    char* ptr = buf;

    while (iValue != 0) {
        TJBox_UInt8 rem = iValue % 10;
        *ptr = '0' + rem;
        ++ptr;
        iValue /= 10;
    }

    *ptr = '\0';

    // Reverse string
    char* begin = buf;
    char* end = ptr - 1;
    while (begin < end) {
        char tmp = *begin;
        *begin = *end;
        *end = tmp;
        ++begin;
        --end;
    }

    Puts(buf);
}

void CTerminal::PutInt(int64_t iValue) {
    if (iValue < 0) {
        Putch('-');
        PutUInt(-iValue);
        return;
    }

    PutUInt(iValue);
}