//done
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Hash
{
    static const uint32_t FNV32_PRIME = 0x01000193;
    static const uint32_t FNV32_IV  = 0x811C9DC5;

    constexpr static inline uint32_t FromConst(const char *const literal, size_t max_length = 0xFFFFFFFF, uint32_t accumulator = FNV32_IV)
    {
        if (*literal && max_length)
            return FromConst(&literal[1], max_length - 1, (accumulator ^ static_cast<uint32_t>(*literal)) * FNV32_PRIME);
        return accumulator;
    }

    static inline uint32_t FromBuffer(const uint8_t *data, uint32_t length)
    {
        uint32_t accumulator = FNV32_IV;
        while (length-- > 0)
            accumulator = (accumulator ^ static_cast<uint32_t>(*data++)) * FNV32_PRIME;
        return accumulator;
    }

    static inline uint32_t FromString(const char *string)
    {
        uint32_t accumulator = FNV32_IV;
        while (*string != '\0')
            accumulator = (accumulator ^ static_cast<uint32_t>(*string++)) * FNV32_PRIME;
        return accumulator;
    }
}

constexpr static inline uint32_t operator"" _h(const char *const literal, size_t length)
{
    return Hash::FromConst(literal, length);
}