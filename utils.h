#ifndef UTILS_H
#define UTILS_H


#include <iostream>
#include <vector>
#include <bitset>
#include <sstream>


std::vector<uint8_t> slicing(const std::vector<uint8_t>& arr, int x, int y);

std::vector<bool> bytes_to_bits(const std::vector<uint8_t>& arr, int x, int y);

std::vector<int> find_frames(const std::vector<uint8_t>& arr);

void print_hex(const std::vector<uint8_t> bytes, int from, int to);

void print_bits(const std::vector<uint8_t> bytes, int from, int to);

std::vector<bool> get_bits  (
                                const std::vector<uint8_t>& arr, 
                                const int starting_byte, 
                                const int starting_bit, 
                                const int full_bytes,
                                const int ending_byte,
                                const int ending_bit
                            );

std::vector<bool> get_next_n_bits(std::vector<bool> bits, int start, int n);

int bits_to_int(std::vector<bool> bits);
int bits_to_int(std::vector<bool> bits, int start, int n);

std::ostream &operator<<(std::ostream &os, std::vector<bool> const &v);
std::ostream &operator<<(std::ostream &os, std::vector<int> const &v);

std::string arr_string(int arr[2][2], int m);
std::string arr_string(int arr[2][2][3], int m);
std::string arr_string(bool arr[2][2], int m);
std::string arr_string(bool arr[2][4]);

#endif
