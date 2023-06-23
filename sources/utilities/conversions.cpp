#include "utilities/conversions.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>

template <>
void QCSP::bytes_to_int<6, 20>(const std::vector<char> & input, std::vector<int> & output) {
    constexpr unsigned nb_bits    = 6;
    constexpr unsigned nb_symbols = 20;
    constexpr uint8_t  int6_mask  = (1U << nb_bits) - 1U;

    int * output_ptr = output.data();

    unsigned sbl_index     = 0;
    unsigned bits_to_write = nb_bits;
    memset(output_ptr, 0, nb_symbols * sizeof(int));
    for (unsigned byte_counter = 0; byte_counter < ((nb_symbols * nb_bits) >> 3); byte_counter++) {
        const uint8_t to_write = input[byte_counter];

        uint8_t remaining_bits = 8;
        assert(sbl_index < nb_symbols);
        while (remaining_bits >= bits_to_write) {
            const uint8_t bit_offset = remaining_bits - bits_to_write;
            const uint8_t mask       = uint8_t(int6_mask) << bit_offset;

            output_ptr[sbl_index] += int((to_write & mask) >> bit_offset);
            assert(output_ptr[sbl_index] < (1 << nb_bits));

            sbl_index++;

            bits_to_write  = nb_bits; // New output_ptr[sbl_index] need full symbol
            remaining_bits = bit_offset;
            assert(remaining_bits < nb_bits);
        }

        if (remaining_bits != 0) {
            const uint8_t offset = nb_bits - remaining_bits;
            const uint8_t mask   = (1U << remaining_bits) - 1U;

            output_ptr[sbl_index] = int((to_write & mask) << offset);
            bits_to_write         = offset;
            assert(output_ptr[sbl_index] < (1 << nb_bits));
        }
    }
}