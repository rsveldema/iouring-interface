#pragma once

#include <array>
#include <string>

#include <cstdlib>

namespace iuring
{
class MacAddress
{
public:
    explicit MacAddress(const std::string& mac);

    explicit MacAddress(
        uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5)
    {
        bytes[0] = b0;
        bytes[1] = b1;
        bytes[2] = b2;
        bytes[3] = b3;
        bytes[4] = b4;
        bytes[5] = b5;
    }


    const std::string to_string(const char sep = ':') const;

    const std::array<uint8_t, 6>& to_bytes() const
    {
        return bytes;
    }

private:
    std::array<uint8_t, 6> bytes{};
};
} // namespace iuring