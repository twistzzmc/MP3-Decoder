#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

#include "mp3.h"
#include "utils.h"

std::vector<uint8_t> read_mp3_file(const char* file_dir);
unsigned get_offset(std::vector<uint8_t> buf);

void print_hex(const std::vector<uint8_t> bytes, int from, int to);
void print_bits(const std::vector<uint8_t> bytes, int from, int to);


int main(int argc, char** argv)
{
    if (argc > 2) {
        std::cout << "Incorrect number of arguments!" << std::endl;
        return -1;
    } else if (argc == 1) {
        std::cout << "No MP3 file directory given!" << std::endl;
        return -1;
    }

    std::vector<uint8_t> buf = read_mp3_file(argv[1]);

    unsigned offset = get_offset(buf);

    mp3 decoder(buf, offset);
    mp3 next_frame(buf, decoder.get_frame_start() + decoder.get_frame_size());
    // mp3 next_next_frame(buf, next_frame.get_frame_start() + next_frame.get_frame_size());
    // decoder.print_frame();
    next_frame.print_frame();
    next_frame.print_scalefactors();
    // next_next_frame.print_frame();
}

std::vector<uint8_t> read_mp3_file(const char* file_dir) {
    std::vector<uint8_t> vec_buf;

    try {
        std::ifstream bin_file(file_dir, std::ios::binary);

        if (bin_file.good()) {
            std::vector<uint8_t> v_buf(
                (std::istreambuf_iterator<char>(bin_file)),
                (std::istreambuf_iterator<char>())
            );

            vec_buf = v_buf;
            bin_file.close();
        } else {
            throw std::exception();
        }
    } catch (std::exception const& ex) {
        std::cerr << std::string(file_dir) + ": file not found" << std::endl;
        throw;
    }

    return vec_buf;
}

unsigned get_offset(std::vector<uint8_t> buf) {
    if (buf[0] != 'I' || buf[1] != 'D' || buf[2] != '3') {
        return 0;
    }

    return 10 + (int) (buf[6] << 21 | buf[7] << 14 | buf[8] << 7 | buf[9]);
}
