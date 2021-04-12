#include <bitset>
#include "utils.h"


std::vector<uint8_t> slicing(const std::vector<uint8_t>& arr, int from, int to) {
    auto start = arr.begin() + from;
    auto end = arr.end() + to + 1;

    std::vector<uint8_t> result(to - from + 1);

    copy(start, end, result.begin());

    return result;
}


std::vector<bool> bytes_to_bits(const std::vector<uint8_t>& arr, int from, int to) {
    std::vector<bool> bits;
    
    for (int i = from; i < to; i++) {
        for (int j = 7; j >= 0; j--) {
            bits.push_back(((arr[i] >> j) & 1));
        }
    }

    return bits;
}
