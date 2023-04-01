#pragma once

#include <cstdint>
#include "../main.h"

namespace Audio {

class ADPCMDecoder {
private:
    int16_t _state1, _state2;

public:
    inline ADPCMDecoder(void)
    : _state1(0), _state2(0) {}

    void reset(void);
    int decode(int16_t *output, const uint8_t *input, int length, int stride);
};

}
