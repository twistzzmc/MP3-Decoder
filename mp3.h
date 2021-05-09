#ifndef MP3_H
#define MP3_H


#include <vector>
#include <iostream>
#include <sstream>

#include "frame_header.h"
#include "frame_side.h"


class mp3 {
    private:
        int frame_size;
        int frame_start;

        frame_header header;
        frame_side side;

    /* main data */
    private:
        std::vector<uint8_t> main_data;
        std::vector<bool> main_data_bits;
        
        int scalefac_l[2][2][22];
        int scalefac_s[2][2][3][13];

        bool used_scalefac_l[2][2][22];
        bool used_scalefac_s[2][2][3][13];

    /* Init and Get */
    public:
        mp3(std::vector<uint8_t> buf, int offset);

        int get_frame_size();
        int get_frame_start();
        frame_header get_header();

    private:
        void set_main_data(std::vector<uint8_t> buf, int offset);
        int set_scale_factors(int gr, int ch, int bit);

    /* Utility */
    public:
        void print_frame();
        void print_scalefactors();
};

#endif
