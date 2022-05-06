#pragma once

#include "Jukebox.h"
#include "EventManager.hpp"
#include <cstddef>
#include <cstdint>

class CGrid {
public:
    CGrid();

    static constexpr uint32_t kGridWidth = 8;
    static constexpr uint32_t kGridHeight = 8;

    void HandleDiffs(CEventManager& iEventManager,
                     const TJBox_PropertyDiff iPropertyDiffs[],
                     TJBox_UInt32 iDiffCount);
    void SendProperties();

    // Specify which tile to change
    void MoveCursor(uint32_t x, uint32_t y);

    // Change the tile at the cursor
    void SetTile(uint32_t color, char c = '\x00');

private:
    void WriteEncodedGridData(uint32_t index, const uint8_t* bytes,
                              uint32_t length);
    uint8_t fGridData[1024];
    uint8_t fGestureBuffer[32];

    // Layout:
    // R G B Character
    static constexpr uint32_t kGridDataSize = 4;
    bool fDirty = true;

    // The maximum supported rate of updates to a single value is once every 25
    // milliseconds. The precision doesnt matter that much since normally you
    // wont be writing text every frame.
    static constexpr uint64_t kMinFrameWait =
        static_cast<uint64_t>(25.0 * 96.0 / 64.0) + 1;
    uint64_t fFramesSinceSend = 0;

    uint32_t fCursorXPos = 0;
    uint32_t fCursorYPos = 0;

    TJBox_ObjectRef fCustomPropertiesRef;
    TJBox_PropertyRef fGridDataRef;
};