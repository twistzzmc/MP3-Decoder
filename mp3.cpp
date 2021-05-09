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

    set_main_data(buf, offset);
}


int mp3::get_frame_size() { return frame_size; }
int mp3::get_frame_start() { return frame_start; }
frame_header mp3::get_header() { return header; }


void mp3::set_main_data(std::vector<uint8_t> buf, int offset) {
    int channels = header.get_mode() == 4 ? 1 : 2;

    int constant = header.get_mode() == 4 ? 21 : 36 + offset;

    if (side.main_data_begin == 0) {
        for (int i = constant; i < offset + frame_size; i++)
            main_data.push_back(buf[i]);
    }

    main_data_bits = bytes_to_bits(main_data, 0, main_data.size());

    // TODO: if main_data_begin != 0

    std::cout << "HEX\n";
    print_hex(main_data, 0, main_data.size());

    for (int gr = 0; gr < 2; gr++) {
        for (int ch = 0; ch < 2; ch++) {
            for (int sfb = 0; sfb < 22; sfb++)
                used_scalefac_l[gr][ch][sfb] = false;

            for (int window = 0; window < 3; window++)
                for (int sfb = 0; sfb < 13; sfb++)
                    used_scalefac_s[gr][ch][window][sfb] = false;
        }
    }


    int bit = 0;
    for (int gr = 0; gr < 2; gr++) {
        for (int ch = 0; ch < channels; ch++) {
            int max_bit = bit + side.part2_3_length[gr][ch];
            bit = set_scale_factors( gr, ch, bit);
            
            bit = max_bit;
        }
    }
}

int mp3::set_scale_factors(int gr, int ch, int bit) {
    int sfb = 0;
    int window = 0;
    int scalefactor_length[2] {
        slen[side.scalefac_compress[gr][ch]][0],
        slen[side.scalefac_compress[gr][ch]][1]
    };

    /* There are no scalefactors for short blocks */
    if (side.block_type[gr][ch] == 2 && side.windows_switching[gr][ch]) {
        /* Mixed blocks */
        if (side.mixed_block_flag[gr][ch]) {
            for (sfb = 0; sfb < 8; sfb++) {
                scalefac_l[gr][ch][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[0]);
                bit += scalefactor_length[0];
                used_scalefac_l[gr][ch][sfb] = true;
            }

            for (sfb = 3; sfb < 6; sfb++) {
                for (window = 0; window < 3; window++) {
                    scalefac_s[gr][ch][window][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[0]);
                    bit += scalefactor_length[0];
                    used_scalefac_l[gr][ch][sfb] = true;
                }
            }
        } 

        /* Short blocks */
        else {
            for (sfb = 0; sfb < 6; sfb++) {
                for (window = 0; window < 3; window++) {
                    scalefac_s[gr][ch][window][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[0]);
                    bit += scalefactor_length[0];
                    used_scalefac_s[gr][ch][window][sfb] = true;
                }
            }
        }

        for (sfb = 6; sfb < 12; sfb++) {
            for (window = 0; window < 3; window++) {
                scalefac_s[gr][ch][window][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[1]);
                bit += scalefactor_length[1];
                used_scalefac_s[gr][ch][window][sfb] = true;
            }
        }

        for (window = 0; window < 3; window++) {
            scalefac_s[gr][ch][window][12] = 0;
            used_scalefac_s[gr][ch][window][12] = true;
        }
    }

    /* Scale factors for long blocks */
    else {
        if (gr == 0) {
            for (sfb = 0; sfb < 11; sfb++) {
                scalefac_l[gr][ch][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[0]);
                bit += scalefactor_length[0];
                used_scalefac_l[gr][ch][sfb] = true;
            }

            for (; sfb < 21; sfb++) {
                scalefac_l[gr][ch][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[1]);
                bit += scalefactor_length[1];
                used_scalefac_l[gr][ch][sfb] = true;
            }
        } else {
            /* Scale factors might be reused in the second granule */
            int sb[5] = {6, 11, 16, 21};

            for (int i = 0; i < 2; i ++) {
                for (; sfb < sb[i]; sfb++) {
                    if (side.scfsi[ch][i]) {
                        scalefac_l[gr][ch][sfb] = scalefac_l[0][ch][sfb];
                        used_scalefac_l[gr][ch][sfb] = true;
                    } else {
                        scalefac_l[gr][ch][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[0]);
                        bit += scalefactor_length[0];
                        used_scalefac_l[gr][ch][sfb] = true;
                    }
                }
            }

            for (int i = 2; i < 4; i++) {
                for (; sfb > sb[i]; sfb++) {
                    if (side.scfsi[ch][i]) {
                        scalefac_l[gr][ch][sfb] = scalefac_l[0][ch][sfb];
                        used_scalefac_l[gr][ch][sfb] = true;
                    } else {
                        scalefac_l[gr][ch][sfb] = bits_to_int(main_data_bits, bit, scalefactor_length[1]);
                        bit += scalefactor_length[1];
                        used_scalefac_l[gr][ch][sfb] = true;
                    }
                }
            }

            scalefac_l[gr][ch][21] = 0;
            used_scalefac_l[gr][ch][21] = true;
        }
    }

    return bit;
}


void mp3::print_frame() {
    std::string frame = "\n\n[Frame Information]";
    frame += "\n  Frame start:       " + std::to_string(frame_start);
    frame += "\n  Frame size:        " + std::to_string(frame_size);

    std::cout << frame << std::endl;
    header.print_header();
    std::cout << "\n";
    side.print_side((header.get_mode() == 4) ? 1 : 2);
}

void mp3::print_scalefactors() {
    std::string scalefactors = "\n\t[Scale Factors]";
    scalefactors += "\n\t  [scalefac_l]\n";
    
    for (int gr = 0; gr < 2; gr++) {
        for (int ch = 0; ch < 2; ch++) {
            scalefactors += "\t    Granule: " + std::to_string(gr + 1) + ", Channel: " + std::to_string(ch + 1) + " || ";
            for (int sfb = 0; sfb < 22; sfb++) {
                if (used_scalefac_l[gr][ch][sfb])
                    scalefactors += std::to_string(scalefac_l[gr][ch][sfb]) + " ";
                else
                    scalefactors += "-1 ";
            }
            scalefactors += "\n";
        }
    }

    scalefactors += "\n\t  [scalefac_s]\n";
    
    for (int gr = 0; gr < 2; gr++) {
        for (int ch = 0; ch < 2; ch++) {
            for (int window = 0; window < 3; window++) {
                scalefactors += "\t      Granule: " + std::to_string(gr + 1) + ", Channel: " + std::to_string(ch + 1);
                scalefactors += ", Window: " + std::to_string(window + 1) + " || ";
                for (int sfb = 0; sfb < 13; sfb++) {
                    if (used_scalefac_s[gr][ch][window][sfb])
                        scalefactors += std::to_string(scalefac_s[gr][ch][window][sfb]) + " ";
                    else
                        scalefactors += "-1 ";
                }
                scalefactors += "\n";
            }
        }
    }

    std::cout << scalefactors << "\n";
}
