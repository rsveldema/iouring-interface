#pragma once

#include <array>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <arpa/inet.h>


namespace network
{
class SendPacket
{
public:
    void append_byte(uint8_t b)
    {
        append(&b, 1);
    }

    void append_uint16(uint16_t v)
    {
        const uint16_t n = ntohs(v);
        append((const uint8_t*)&n, sizeof(n));
    }

    void append_uint32(uint32_t v)
    {
        const uint32_t n = ntohl(v);
        append((const uint8_t*)&n, sizeof(n));
    }

    template <typename T> void append(const T& data)
    {
        append((const uint8_t*) &data, sizeof(data));
    }

    void append(const std::string& data)
    {
        append((const uint8_t*) data.c_str(), data.length());
    }

    void append(const char* data)
    {
        append((const uint8_t*) data, strlen(data));
    }

    void append(const uint8_t* data, size_t len)
    {
        assert((m_size + len) < m_buf.size());
        memcpy(&m_buf[m_size], data, len);
        m_size += len;
    }

    template<class T, class... Args>
    void emplace_back(Args&&... args)
    {
        assert((m_size + sizeof(T)) < m_buf.size());
        auto* ptr = &m_buf[m_size];

        memset(ptr, 0, sizeof(T)); // NOTE: memset possibly superfluous if T's ctor is ok
        new(ptr) T(args...);

        m_size += sizeof(T);
    }

    void reset()
    {
        memset(m_buf.data(), 0, m_size);
        m_size = 0;
    }

    void clean_proper()
    {
        m_size = 0;
        m_buf.fill(0);
    }

    size_t size() const
    {
        return m_size;
    }

    const uint8_t* data() const
    {
        return m_buf.data();
    }

private:
    size_t m_size = 0;
    std::array<uint8_t, 4096> m_buf;
};

} // namespace network