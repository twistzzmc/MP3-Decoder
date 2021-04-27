#ifndef FRAME_SIDE_H
#define FRAME_SIDE_H


#include <vector>
#include <iostream>
#include <sstream>


class frame_side {
    private:
        int main_data_begin;
        bool scfsi[2][4];

        int part2_3_length[2][2];
        int big_values[2][2];
        int global_gain[2][2];
        int scalefac_compress[2][2];
        int slen1[2][2];
        int slen2[2][2];
        bool windows_switching[2][2];

        int block_type[2][2];
        bool mixed_block_flag[2][2];
        int switch_point_l[2][2];
        int switch_point_s[2][2];
        int subblock_gain[2][2][3];

        int table_select[2][2][3];
        int region0_count[2][2];
        int region1_count[2][2];

        bool preflag[2][2];
        bool scalefac_scale[2][2];
        bool count1table_select[2][2];

    public:
        frame_side();
        frame_side(std::vector<uint8_t> buf, int channels);

        void print_side(int channels);
};


#endif
