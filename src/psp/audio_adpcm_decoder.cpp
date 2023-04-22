
#include <cstdint>
#include <algorithm>
#include "../main.h"
#include "audio_adpcm_decoder.h"

namespace Audio {

/*
 * This is not the best SPU-ADPCM (aka VAG) decoder ever written, but it works.
 * Loop flag handling in particular is simplified down to the bare minimum (i.e.
 * if a loop flag is encountered, stop decoding and return the number of samples
 * decoded so far).
 */

static const int ADPCM_FILTERS[16][2]{
    // Standard coefficients
    {   0,   0 },
    {  60,   0 },
    { 115, -52 },
    {  98, -55 },
    { 122, -60 },
    // Extended coefficients (?)
    {  30,   0 },
    {  57, -26 },
    {  49, -27 },
    {  61, -30 },
    {  15,   0 },
    {  29, -13 },
    {  24, -14 },
    {  30, -15 },
    {  32, -60 },
    {  15, -60 },
    {   7, -60 }
};

void ADPCMDecoder::reset(void) {
    _state1 = 0;
    _state2 = 0;
}
int ADPCMDecoder::decode(int16_t *output, const uint8_t *input, int length, int stride) {
    int state1 = _state1, state2 = _state2;
    int numSamples = 0;

    for (; length >= 16; length -= 16) {
        int shiftFactor = input[0] & 0x0f;
        int filterIndex = (input[0] & 0xf0) >> 4;
        if (input[1] & 1)
            break;
        input += 2;

        //ASSERTFUNC(filterIndex, "invalid filter index");
        auto filter = ADPCM_FILTERS[filterIndex];

        for (int j = 0; j < 28; j++) {
            int value;
            if (!(j & 1)) // Lower nibble is decoded first
                value = (*input & 0x0f) << 12;
            else
                value = (*(input++) & 0xf0) << 8;

            value >>= shiftFactor;
            value += (state1 * filter[0] + state2 * filter[1]) / 64;
            value = std::min(std::max(value, INT16_MIN), INT16_MAX);

            auto sample = static_cast<int16_t>(value);

            state2 = state1;
            state1 = sample;

            *output = sample;
            output += stride;
        }

        numSamples += 28;
    }

    _state1 = state1;
    _state2 = state2;

    return numSamples;
}
}
