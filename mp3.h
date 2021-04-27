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

    /* Init and Get*/
    public:
        mp3(std::vector<uint8_t> buf, int offset);

        int get_frame_size();
        int get_frame_start();
        frame_header get_header();

    /* Utility */
    public:
        void print_frame();
};

#endif
