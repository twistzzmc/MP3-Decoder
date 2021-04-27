#include "frame_header.h"
#include "utils.h"
#include "tables.h"


frame_header::frame_header() { }

frame_header::frame_header(std::vector<uint8_t> buf) {
    std::vector<bool> bits = bytes_to_bits(buf, 0, 4);

    std::cout << "\nPrinting header bits...\n";

    for (int i = 0; i < (int) bits.size(); i++) {
        if (i % 8 == 0) std::cout << " ";
        std::cout << bits[i];
    }

    check_sync(bits);
    set_mpeg(bits);
    set_layer(bits);
    protection = bits[15];
    set_bitrate(bits);
    set_frequency(bits);
    padding = bits[22];
    private_bit = bits[23];
    set_mode(bits);
    set_mode_extension(bits);
    copyright = bits[28];
    copy = bits[29];
    set_emphasis(bits);
}


void frame_header::check_sync(std::vector<bool> bits) {
    for (int i = 0; i <= 12; i++) {
        if (bits[i] == 0) {
            std::cerr << "Not synchronized!" << std::endl;
            throw;
        }
    }
}
void frame_header::set_mpeg(std::vector<bool> bits) {
    if (bits[12] == 0) {
        std::cerr << "Not MPEG version 1!" << std::endl;
        throw;
    } 
    MPEG_version = bits[12];
}
void frame_header::set_layer(std::vector<bool> bits) {
    if (bits[13] == 0 && bits[14] == 1) layer = 3;
    else if (bits[13] == 1 && bits[14] == 0) layer = 2;
    else if (bits[13] == 1 && bits[14] == 1) layer = 1;
    else {
        std::cerr << "Layer cannot be set to reserved!" << std::endl;
        throw;
    }
}
void frame_header::set_bitrate(std::vector<bool> bits) {
    int x = layer - 1;
    int y = 0;

    if (bits[16] == 1) y += 8;
    if (bits[17] == 1) y += 4;
    if (bits[18] == 1) y += 2;
    if (bits[19] == 1) y += 1;

    if (y == 0 || y == 15) {
        std::cerr << "Bitrate wrong value!" << std::endl;
        throw;
    }

    bitrate = bitrates[x][y];
}
void frame_header::set_frequency(std::vector<bool> bits) {
    if (bits[20] == 0 && bits[21] == 0) frequency = 44100;
    else if (bits[20] == 0 && bits[21] == 1) frequency = 48000;
    else if (bits[20] == 1 && bits[21] == 0) frequency = 32000;
    else if (bits[20] == 0 && bits[21] == 0) {
        std::cerr << "Frequency wrong value!" << std::endl;
        throw;
    }
}
void frame_header::set_mode(std::vector<bool> bits) {
    if (bits[24] == 0 && bits[25] == 0) mode = Stereo;
    else if (bits[24] == 1 && bits[25] == 1) mode = SingleChannel;
    else if (bits[24] == 1 && bits[25] == 0) mode = DualChannel;
    else if (bits[24] == 0 && bits[25] == 1) mode = JointStereo;
}
void frame_header::set_mode_extension (std::vector<bool> bits) {
    MS_stereo = bits[26];
    intensity_stereo = bits[27];
}
void frame_header::set_emphasis(std::vector<bool> bits) {
    if (bits[30] == 0 && bits[31] == 0) emphasis = None;
    else if (bits[30] == 1 && bits[31] == 1) emphasis = CCITJ17;
    else if (bits[30] == 0 && bits[31] == 1) emphasis = MS5015;
    else {
        std::cerr << "Emphasis wrong value!" << std::endl;
        throw;
    }
}


bool frame_header::get_mpeg() { return MPEG_version; }
int frame_header::get_layer() { return layer; }
bool frame_header::get_protection() { return protection; }
int frame_header::get_bitrate() { return bitrate; }
int frame_header::get_frequency() { return frequency; }
bool frame_header::get_padding() { return padding; }
bool frame_header::get_private_bit() { return private_bit; }
frame_header::ChannelMode frame_header::get_mode() { return mode; }
bool frame_header::get_intensity_stereo() { return intensity_stereo; }
bool frame_header::get_ms_stereo() { return MS_stereo; }
bool frame_header::get_copyright() { return copyright; }
bool frame_header::get_copy() { return copy; }
frame_header::Emphasis frame_header::get_emphasis() { return emphasis; }


void frame_header::print_header() {
    std::string header = "\n\t[Header Information]";
    header += "\n\t  MPEG version:      " + std::to_string(MPEG_version);
    header += "\n\t  Layer:             " + std::to_string(layer);
    header += "\n\t  Protection bit:    " + std::to_string(protection);
    header += "\n\t  Bitrate:           " + std::to_string(bitrate);
    header += "\n\t  Frequency:         " + std::to_string(frequency);
    header += "\n\t  Padding:           " + std::to_string(padding);
    header += "\n\t  Private bit:       " + std::to_string(private_bit);
    header += "\n\t  Mode:              " + std::to_string(mode);
    header += "\n\t  Inensity Stereo:   " + std::to_string(intensity_stereo);
    header += "\n\t  MS stereo:         " + std::to_string(MS_stereo);
    header += "\n\t  Copyright:         " + std::to_string(copyright);
    header += "\n\t  Copy:              " + std::to_string(copy);
    header += "\n\t  Emphasis:          " + std::to_string(emphasis);
    
    std::cout << header << std::endl;
}
