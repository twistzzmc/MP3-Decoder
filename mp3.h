#ifndef MP3_H
#define MP3_H


#include <vector>
#include <iostream>


class mp3 {
    /* Enums */
    public:
        enum ChannelMode {
            Stereo = 1,
            JointStereo = 2,
            DualChannel = 3,
            SingleChannel = 4
        };

        enum Emphasis {
            None = 1,
            MS5015 = 2,
            Reserved = 3,
            CCITJ17 = 4
        };

    /* Header */
    private:                                // Position of bits in header
        bool MPEG_version;                  // 13
        int layer;                          // 14 - 15
        bool protection;                    // 16
        int bitrate;                        // 17 - 20
        int frequency;                      // 21 - 22
        bool padding;                       // 23
        bool private_bit;                   // 24
        ChannelMode mode;                   // 25 - 26
        bool intensity_stereo;              // 27 - 28
        bool MS_stereo;                     // 27 - 28
        bool copyright;                     // 29
        bool copy;                          // 30
        Emphasis emphasis;                  // 31 - 32

    /* Init */
    public:
        mp3(std::vector<uint8_t> buf);

    /* Set Header Data */
    private:
        void init_header(std::vector<uint8_t> buf);

        void check_sync         (std::vector<bool> bits);
        void set_mpeg           (std::vector<bool> bits);
        void set_layer          (std::vector<bool> bits);
        void set_bitrate        (std::vector<bool> bits);
        void set_frequency      (std::vector<bool> bits);
        void set_mode           (std::vector<bool> bits);
        void set_mode_extension (std::vector<bool> bits);
        void set_emphasis       (std::vector<bool> bits);

    /* Get Header Data*/
    public:
        bool get_mpeg();
        int get_layer();
        bool get_protection();
        int get_bitrate();
        int get_frequency();
        bool get_padding();
        bool get_private_bit();
        ChannelMode get_mode();
        bool get_intensity_stereo();
        bool get_ms_stereo();
        bool get_copyright();
        bool get_copy();
        Emphasis get_emphasis();

    /* Utility */
    public:
        void print_header();
};

#endif
