#include "frame_side.h"
#include "utils.h"
#include "tables.h"


frame_side::frame_side() { }

frame_side::frame_side(std::vector<uint8_t> buf, int channels) {
    std::cout << "\n\nInside frame side constructor:\n";
    print_hex(buf, 0, buf.size());


    std::vector<bool> buf_bits = bytes_to_bits(buf, 0, buf.size());
    std::cout << "BUF BITS\n" << buf_bits;

    int idx = 9;

    /* main_data_begin */   
    main_data_begin = bits_to_int(get_bits(buf, 0, 1, 0, 1, 1));

    /* scfsi */
    for (int i = 0; i < ((channels == 1) ? 4 : 8); i++) {
        scfsi[(int) i / 4][i % 4] = buf_bits[idx + i];
    }
    
    idx += (channels == 1) ? 4 : 8;

    /* for each granule */
    for (int gr = 0; gr < 2; gr++) {

        /* for each channel */
        for (int ch = 0; ch < channels; ch++) {

            /* part2_3_length */
            part2_3_length[gr][ch] = bits_to_int(buf_bits, idx, 12);
            idx += 12;

            /* big_values */
            big_values[gr][ch] = bits_to_int(buf_bits, idx, 9);
            idx += 9;

            /* global_gain */
            global_gain[gr][ch] = bits_to_int(buf_bits, idx, 8);
            idx += 8;

            /* scalefac_compress */
            scalefac_compress[gr][ch] = bits_to_int(buf_bits, idx, 4);
            idx += 4;

            /* slen1 and slen2 */
            slen1[gr][ch] = slen[scalefac_compress[gr][ch]][0];
            slen2[gr][ch] = slen[scalefac_compress[gr][ch]][1];

            /* window_switching */
            windows_switching[gr][ch] = buf_bits[idx] == 1;
            idx++;

            /* Getting window parameters: 
             * If True:
             *  0: reserved
             *  1: start block
             *  2: 3 short windows
             *  3: end block
             * Else:
             *  normal type window
             */
            if (windows_switching[gr][ch]) {
                /* block_type */
                block_type[gr][ch] = bits_to_int(buf_bits, idx, 2);
                idx += 2;

                /* mixed_block_flag */
                mixed_block_flag[gr][ch] = buf_bits[idx] == 1;
                idx += 1;

                /* switch_point_l and switch_point_s */
                if (mixed_block_flag[gr][ch]) {
                    switch_point_l[gr][ch] = 8;
                    switch_point_s[gr][ch] = 3;
                } else {
                    switch_point_l[gr][ch] = 0;
                    switch_point_s[gr][ch] = 0;
                }

                /* region0_count and region1_count */
                region0_count[gr][ch] = block_type[gr][ch] == 2 ? 8 : 7;
                region1_count[gr][ch] = 20 - region0_count[gr][ch];

                /* table_select */
                for (int region = 0; region < 2; region++) {
                    table_select[gr][ch][region] = bits_to_int(buf_bits, idx, 5);
                    idx += 5;
                }
                table_select[gr][ch][2] = -1;

                /* subblock_gain */
                for (int window = 0; window < 3; window++) {
                    subblock_gain[gr][ch][window] = bits_to_int(buf_bits, idx, 3);
                    idx += 3;
                }
            } else {
                /* If we don't switch window block_type and mixed_block_flag are set by default */
                block_type[gr][ch] = 0;
                mixed_block_flag[gr][ch] = false;

                /* For this type of window these aren't used so I set to -1 */
                switch_point_l[gr][ch] = -1;
                switch_point_s[gr][ch] = -1;
                for (int i = 0; i < 3; i++) subblock_gain[gr][ch][i] = -1;

                /* table_select */
                for (int region = 0; region < 3; region++) {
                    table_select[gr][ch][region] = bits_to_int(buf_bits, idx, 5);
                    idx += 5;
                }

                /* region0_count */
                region0_count[gr][ch] = bits_to_int(buf_bits, idx, 4);
                idx += 4;

                /* region1_count */
                region1_count[gr][ch] = bits_to_int(buf_bits, idx, 3);
                idx += 3;
            }

            /* preflag */
            preflag[gr][ch] = buf_bits[idx];
            idx += 1;

            /* scalefac_scale */
            scalefac_scale[gr][ch] = buf_bits[idx];
            idx += 1;

            /* count1table_select */
            count1table_select[gr][ch] = buf_bits[idx];
            idx += 1;
        }
    }
}

void frame_side::print_side(int channels) {
    std::string side = "\n\t[Side Information]";
    side += "\n\t  main_data_begin:       " + std::to_string(main_data_begin);
    side += "\n\t  scfsi:                 " + arr_string(scfsi);
    side += "\n\t  part2_3_length:        " + arr_string(part2_3_length, channels);
    side += "\n\t  big_values:            " + arr_string(big_values, channels);
    side += "\n\t  global_gain:           " + arr_string(global_gain, channels);
    side += "\n\t  scalefac_compress:     " + arr_string(scalefac_compress, channels);
    side += "\n\t  slen1:                 " + arr_string(slen1, channels);
    side += "\n\t  slen2:                 " + arr_string(slen2, channels);
    side += "\n\t  windows_switching:     " + arr_string(windows_switching, channels);
    side += "\n\t  block_type:            " + arr_string(block_type, channels);
    side += "\n\t  mixed_block_flag:      " + arr_string(mixed_block_flag, channels);
    side += "\n\t  switch_point_l:        " + arr_string(switch_point_l, channels);
    side += "\n\t  switch_point_s:        " + arr_string(switch_point_s, channels);
    side += "\n\t  subblock_gain:         " + arr_string(subblock_gain, channels);
    side += "\n\t  table_select:          " + arr_string(table_select, channels);
    side += "\n\t  region0_count:         " + arr_string(region0_count, channels);
    side += "\n\t  region1_count:         " + arr_string(region1_count, channels);
    side += "\n\t  preflag:               " + arr_string(preflag, channels);
    side += "\n\t  scalefac_scale:        " + arr_string(scalefac_scale, channels);
    side += "\n\t  count1table_select:    " + arr_string(count1table_select, channels);

    std::cout << side << std::endl;
}
