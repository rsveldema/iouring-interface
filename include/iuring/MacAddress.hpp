#pragma once

#include <array>
#include <string>

#include <cstdlib>

namespace iuring
{
class MacAddress
{
public:
    explicit MacAddress(const std::string& mac)
        : m_value(mac)
    {
    }

    const std::string& to_string() const
    {
        return m_value;
    }

    const std::array<uint8_t, 6> to_bytes() const
    {
        std::array<uint8_t, 6> bytes{};
        std::sscanf(m_value.c_str(), "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            &bytes[0], &bytes[1], &bytes[2],
            &bytes[3], &bytes[4], &bytes[5]);
        return bytes;
    }

private:
    std::string m_value;
};
} // namespace iuring