#pragma once
#include "UserIO.h"

// Use template to deal with cross referencing
template <typename CHost>
class CEmulatorMMIO {
public:
    CEmulatorMMIO(CHost* iHost) : fHost(iHost){};

    template <typename T>
    optional<T> read(uint32_t iAddress) {
        if (!IsMMIO(iAddress)) return optional<T>();

        uint32_t result;
        const bool success = fHost->HandleMMIORead(iAddress, result);

        if (success)
            return optional(static_cast<T>(result));
        else
            return optional<T>();
    };

    template <typename T>
    bool store(uint32_t iAddress, T iValue) {
        if (!IsMMIO(iAddress)) return false;
        const uint32_t u32Value = static_cast<uint32_t>(iValue);

        return fHost->HandleMMIOStore(iAddress, u32Value);
    };

private:
    bool IsMMIO(uint32_t iAddress) {
        return (iAddress & MMIO_MASK) == MMIO_MASK;
    };
    CHost* fHost;
};
