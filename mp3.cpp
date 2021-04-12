#include "mp3.h"
#include "utils.h"
#include "tables.h"

mp3::mp3(std::vector<uint8_t> buf) {
    init_header(slicing(buf, 0, 3));
}

void mp3::init_header(std::vector<uint8_t> buf) {
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


void mp3::check_sync(std::vector<bool> bits) {
    for (int i = 0; i <= 12; i++) {
        if (bits[i] == 0) {
            std::cerr << "Not synchronized!" << std::endl;
            throw;
        }
    }
}
void mp3::set_mpeg(std::vector<bool> bits) {
    if (bits[12] == 0) {
        std::cerr << "Not MPEG version 1!" << std::endl;
        throw;
    } 
    MPEG_version = bits[12];
}
void mp3::set_layer(std::vector<bool> bits) {
    if (bits[13] == 0 && bits[14] == 1) layer = 3;
    else if (bits[13] == 1 && bits[14] == 0) layer = 2;
    else if (bits[13] == 1 && bits[14] == 1) layer = 1;
    else {
        std::cerr << "Layer cannot be set to reserved!" << std::endl;
        throw;
    }
}
void mp3::set_bitrate(std::vector<bool> bits) {
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
void mp3::set_frequency(std::vector<bool> bits) {
    if (bits[20] == 0 && bits[21] == 0) frequency = 44100;
    else if (bits[20] == 0 && bits[21] == 1) frequency = 48000;
    else if (bits[20] == 1 && bits[21] == 0) frequency = 32000;
    else if (bits[20] == 0 && bits[21] == 0) {
        std::cerr << "Frequency wrong value!" << std::endl;
        throw;
    }
}
void mp3::set_mode(std::vector<bool> bits) {
    if (bits[24] == 0 && bits[25] == 0) mode = Stereo;
    else if (bits[24] == 1 && bits[25] == 1) mode = SingleChannel;
    else if (bits[24] == 1 && bits[25] == 0) mode = DualChannel;
    else if (bits[24] == 0 && bits[25] == 1) mode = JointStereo;
}
void mp3::set_mode_extension (std::vector<bool> bits) {
    MS_stereo = bits[26];
    intensity_stereo = bits[27];
}
void mp3::set_emphasis(std::vector<bool> bits) {
    if (bits[30] == 0 && bits[31] == 0) emphasis = None;
    else if (bits[30] == 1 && bits[31] == 1) emphasis = CCITJ17;
    else if (bits[30] == 0 && bits[31] == 1) emphasis = MS5015;
    else {
        std::cerr << "Emphasis wrong value!" << std::endl;
        throw;
    }
}

bool mp3::get_mpeg() { return MPEG_version; }
int mp3::get_layer() { return layer; }
bool mp3::get_protection() { return protection; }
int mp3::get_bitrate() { return bitrate; }
int mp3::get_frequency() { return frequency; }
bool mp3::get_padding() { return padding; }
bool mp3::get_private_bit() { return private_bit; }
mp3::ChannelMode mp3::get_mode() { return mode; }
bool mp3::get_intensity_stereo() { return intensity_stereo; }
bool mp3::get_ms_stereo() { return MS_stereo; }
bool mp3::get_copyright() { return copyright; }
bool mp3::get_copy() { return copy; }
mp3::Emphasis mp3::get_emphasis() { return emphasis; }

void mp3::print_header() {
    std::string header = "\nHeader Informationi...\n";
    header += "\nMPEG version:      " + std::to_string(MPEG_version);
    header += "\nLayer:             " + std::to_string(layer);
    header += "\nProtection bit:    " + std::to_string(protection);
    header += "\nBitrate:           " + std::to_string(bitrate);
    header += "\nFrequency:         " + std::to_string(frequency);
    header += "\nPadding:           " + std::to_string(padding);
    header += "\nPrivate bit:       " + std::to_string(private_bit);
    header += "\nMode:              " + std::to_string(mode);
    header += "\nInensity Stereo:   " + std::to_string(intensity_stereo);
    header += "\nMS stereo:         " + std::to_string(MS_stereo);
    header += "\nCopyright:         " + std::to_string(copyright);
    header += "\nCopy:              " + std::to_string(copy);
    header += "\nEmphasis:          " + std::to_string(emphasis);
    
    std::cout << header << std::endl;
}