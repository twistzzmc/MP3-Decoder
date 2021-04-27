#include "utils.h"


std::vector<uint8_t> slicing(const std::vector<uint8_t>& arr, int from, int to) {
    std::vector<uint8_t> result;

    for (int i = from; i <= to; i++) {
        result.push_back(arr[i]);
    }

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

std::vector<int> find_frames(const std::vector<uint8_t>& arr) {
    std::vector<int> frames_positions;
    std::cout << "Arr len: " << arr.size() << "\n";

    for (int i = 0; i < (int) arr.size() - 1; i++) {
        std::bitset<8> byte(arr[i]);
        bool ones = true;
        for (int j = 0; j < 8; j++) {
            if (!byte.test(j)) {
                ones = false;
                break;
            }
        }

        if (ones) {
            std::bitset<8> secondByte(arr[i + 1]);
            for (int j = 7; j >= 4; j--) {
                if (!secondByte.test(j)) {
                    ones = false;
                    break;
                }
            }
        }

        if (ones) {
            std::cout << "Found frame: POS(" << i << ") ";
            print_hex(arr, i, i + 4 + 32);
            frames_positions.push_back(i);
        }
    }

    return frames_positions;
}


void print_hex(const std::vector<uint8_t> bytes, int from, int to) {
    constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};                     

    // std::string s(bytes.size() * 2, ' ');
    // for (int i = 0; i < (int) bytes.size(); ++i) {
    //     s[2 * i] = hexmap[(bytes[i] & 0xF0) >> 4];
    //     s[2 * i + 1] = hexmap[bytes[i] & 0x0F];
    // }
    int newline = 25;
    for (int i = from; i < to; ++i) {
        if ((i - from) % newline == 0 && i - from != 0) std::cout << "\n";

        std::cout << hexmap[(bytes[i] & 0xF0) >> 4] << hexmap[bytes[i] & 0x0F] << " ";
    }
    std::cout << "\n";
}

void print_bits(const std::vector<uint8_t> bytes, int from, int to) {
    for (int i = from; i < to; ++i) {
        std::cout << std::bitset<8>(bytes[i]) << " ";
    }
    std::cout << "\n";
}

std::vector<bool> get_bits  (
                                const std::vector<uint8_t>& arr, 
                                const int starting_byte,        
                                const int starting_bit,         // from 1 (first bit) to 8 (last bit)
                                const int full_bytes,
                                const int ending_byte,
                                const int ending_bit            // from 1 (first bit) to 8 (last bit)
                            ) 
{
    std::vector<bool> bits;

    if (starting_byte == ending_byte) {
        std::bitset<8> only(arr[starting_byte]);
        for (int i = 8 - starting_bit; i >= 8 - ending_bit; i--) {
            bits.push_back(only.test(i));
        } 
        return bits;
    }

    std::bitset<8> first(arr[starting_byte]);
    for (int i = 8 - starting_bit; i >= 0; i--) bits.push_back(first.test(i));

    for (int i = starting_byte + 1; i < starting_byte + full_bytes + 1; i++) {
        std::bitset<8> middle(arr[i]);
        for (int j = 7; j >= 0; j--) bits.push_back(middle.test(j));
    }

    std::bitset<8> last(arr[ending_byte]);
    for (int i = 7; i >= 8 - ending_bit; i--) bits.push_back(last.test(i));

    return bits;
}

std::vector<bool> get_next_n_bits(std::vector<bool> bits, int start, int n) {
    std::vector<bool> new_bits;

    for (int i = start; i < n + start; i++) {
        new_bits.push_back(bits[i]);
    }

    return new_bits;
}

int bits_to_int(std::vector<bool> bits) {
    int power = 1;
    int number = 0;

    for (int i = bits.size() - 1; i >= 0; i--) {
        if (bits[i]) number += power;
        power *= 2;
    }

    return number;
}

int bits_to_int(std::vector<bool> bits, int start, int n) {
    return bits_to_int(get_next_n_bits(bits, start, n));
}

std::ostream &operator<<(std::ostream &os, std::vector<bool> const &v) { 
    for (int i = 0; i < (int) v.size(); i++) {
        if (i % 8 == 0 && i != 0) os << " ";
        os << v[i];
    }
    os << "\n";
    return os;
}
std::ostream &operator<<(std::ostream &os, std::vector<int> const &v) {
    for (int i = 0; i < (int) v.size(); i++) {
        if (i % 8 == 0 && i != 0) os << " ";
        os << v[i];
    }
    os << "\n";
    return os;
}

std::string arr_string(int arr[2][2], int m) {
    std::string string = "";
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m; j++) {
            string += std::to_string(arr[i][j]) + (j < m - 1 ? " | " : " ");
        }
        if (i == 0) string += "|| ";
    }
    return string;
}
std::string arr_string(int arr[2][2][3], int m) {
    std::string string = "";
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < 3; k++) {
                string += std::to_string(arr[i][j][k]) + " ";
            }
            if (j != m - 1) string += "| ";
        }
        if (i == 0) string += "|| ";
    }
    return string;
}
std::string arr_string(bool arr[2][2], int m) {
    std::string string = "";
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m; j++) {
            string += std::to_string(arr[i][j]) + (j < m - 1 ? " | " : " ");
        }
        if (i == 0) string += "|| ";
    }
    return string;
}
std::string arr_string(bool arr[2][4]) {
    std::string string = "";
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            string += std::to_string(arr[i][j]) + " ";
        }
        if (i == 0) string += "|| ";
    }
    return string;
}
