#include "Grid.h"
#include "Constants.h"
#include "Encoding.h"
#include "UserIO.h"

#include <cstring>

CGrid::CGrid() {
    fCustomPropertiesRef = JBox_GetMotherboardObjectRef("/custom_properties");
    fGridDataRef = JBox_MakePropertyRef(fCustomPropertiesRef, "grid_data");

    // Set entire hex string to 00
    std::memset(fGridData, '0', sizeof(fGridData));
}

void CGrid::WriteEncodedGridData(uint32_t index, const uint8_t* bytes,
                                 uint32_t length) {
    auto get_nibble_hex = [](uint8_t nibble) {
        if (nibble >= 10) return (uint8_t)'A' + nibble - 10;

        return (uint8_t)'0' + nibble;
    };

    for (int n = 0; n < length; n++) {
        const uint8_t byte = bytes[n];

        const int i = index + n;
        fGridData[2 * i] = get_nibble_hex(byte >> 4);
        fGridData[2 * i + 1] = get_nibble_hex(byte & 0b1111);
    }
}

void CGrid::SetTile(uint32_t color, char c) {
    uint32_t index = fCursorXPos + fCursorYPos * kGridWidth;
    index *= kGridDataSize;

    // R G B Char
    const uint8_t data[4] = {(uint8_t)((color >> 16) & 0xFF),
                             (uint8_t)((color >> 8) & 0xFF),
                             (uint8_t)((color >> 0) & 0xFF), (uint8_t)c};

    WriteEncodedGridData(index, data, kGridDataSize);
    fDirty = true;
}

void CGrid::MoveCursor(uint32_t x, uint32_t y) {
    if (x >= kGridWidth || y >= kGridWidth) return;

    fCursorXPos = x;
    fCursorYPos = y;
}

void CGrid::HandleDiffs(CEventManager& iEventManager,
                        const TJBox_PropertyDiff iPropertyDiffs[],
                        TJBox_UInt32 iDiffCount) {
    for (TJBox_UInt32 i = 0; i < iDiffCount; ++i) {
        const TJBox_PropertyDiff& diff = iPropertyDiffs[i];

        if (diff.fPropertyRef.fObject == fCustomPropertiesRef &&
            diff.fPropertyTag == kGestureTag) {

            const uint32_t content_length =
                DecodeParameterString(diff.fCurrentValue, (char*)fGestureBuffer,
                                      sizeof(fGestureBuffer));
            if (!content_length) continue;

            const bool is_press = static_cast<bool>(fGestureBuffer[0]);
            const uint32_t event_id =
                is_press ? EVENTID_GRIDPRESS : EVENTID_GRIDRELEASE;
            const uint8_t index = fGestureBuffer[1];

            iEventManager.QueueEvent(event_id, index);
        }
    }
}

void CGrid::SendProperties() {
    // Limit how often we send the grid data
    if (!fDirty) return;
    if (fFramesSinceSend++ < kMinFrameWait) return;

    const uint32_t max_index = kGridDataSize * kGridHeight * kGridWidth * 2;
    JBox_SetRTStringData(fGridDataRef, max_index, fGridData);

    fDirty = false;
    fFramesSinceSend = 0;
}

bool CGrid::HandleMMIOStore(uint32_t iAddress, uint32_t iValue) {
    switch (MMIO_INDEX(iAddress)) {
        case MMIO_INDEX(MMIO_MOVEGRIDCURSOR): {
            const uint32_t x = iValue & 0xFFFF;
            const uint32_t y = iValue >> 16;

            MoveCursor(x, y);
            break;
        }

        case MMIO_INDEX(MMIO_SETGRIDTILE): {
            uint32_t color = iValue >> 8;
            char ch = (char)(iValue & 0xFF);

            SetTile(color, ch);
            break;
        }

        default: return false;
    }

    return true;
}