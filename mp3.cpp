#include "mp3.h"
#include "utils.h"
#include "tables.h"

mp3::mp3(std::vector<uint8_t> buf, int offset) {
    frame_start = offset;

    header = frame_header(slicing(buf, offset, offset + 3));

    int channels = (header.get_mode() == 4) ? 1 : 2;
    std::vector<uint8_t> side_info = slicing(buf, offset + 4, offset + 4 + (header.get_mode() == 4 ? 16 : 31));
    side = frame_side(side_info, channels);

    frame_size = (144 * header.get_bitrate() / (float) (header.get_frequency() / 1000)) + (int) header.get_padding();
}


int mp3::get_frame_size() { return frame_size; }
int mp3::get_frame_start() { return frame_start; }
frame_header mp3::get_header() { return header; }


void mp3::print_frame() {
    std::string frame = "\n\n[Frame Information]";
    frame += "\n  Frame start:       " + std::to_string(frame_start);
    frame += "\n  Frame size:        " + std::to_string(frame_size);

    std::cout << frame << std::endl;
    header.print_header();
    std::cout << "\n";
    side.print_side((header.get_mode() == 4) ? 1 : 2);
}
