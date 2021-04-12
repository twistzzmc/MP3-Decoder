#ifndef UTILS_H
#define UTILS_H


#include <iostream>
#include <vector>


std::vector<uint8_t> slicing(const std::vector<uint8_t>& arr, int x, int y);

std::vector<bool> bytes_to_bits(const std::vector<uint8_t>& arr, int x, int y);

#endif
