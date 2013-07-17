// =================================================================
// Author: Kristof Niederholtmeyer
//

#ifndef __network_game__PlayerView__
#define __network_game__PlayerView__

#include <iostream>
#include <string>
#include "defines.h"

template<unsigned int ViewWidth, unsigned int ViewHeight>
class PlayerView {
public:
    enum {FLAGS_FIELD_SIZE = 1};
    enum {SYMBOL_FIELD_SIZE = 1};
    enum {COLORS_FIELD_SIZE = 2};
    enum {NUMBER_OF_PIXELS = ViewWidth * ViewHeight};
    enum {MAX_SERIALIZE_SIZE = NUMBER_OF_PIXELS * (FLAGS_FIELD_SIZE + SYMBOL_FIELD_SIZE + COLORS_FIELD_SIZE)};

private:
    struct PlayerViewPixel {
        // specifies the tile from the tile texture to show for this pixel
        unsigned char tile;
        // specifies wether the tile has to be flipped horizontaly or verticaly
        bool flip_x, flip_y;
        // specifies the background color
        unsigned char background_color;
        // specifies the symbol color
        unsigned char symbol_color;
    };
    PlayerViewPixel _pixels[NUMBER_OF_PIXELS];

public:
    PlayerView() {

    }

    void set_symbol(unsigned int x, unsigned int y, unsigned char tile, bool flip_x, bool flip_y) {
        _pixels[y * ViewWidth + x].tile = tile;
        _pixels[y * ViewWidth + x].flip_x = flip_x;
        _pixels[y * ViewWidth + x].flip_y = flip_y;
    }
    void set_background_color(unsigned int x, unsigned int y, unsigned char background_color) {
        _pixels[y * ViewWidth + x].background_color = background_color;
    }
    void set_symbol_color(unsigned int x, unsigned int y, unsigned char symbol_color) {
        _pixels[y * ViewWidth + x].symbol_color = symbol_color;   
    }
    void set_text(unsigned int x, unsigned int y, const std::string& text, unsigned char background_color, unsigned char text_color) {
        unsigned int cursor_x = x;
        unsigned int cursor_y = y;
        unsigned int cursor_text = 0;
        while (cursor_x < ViewWidth && cursor_y < ViewHeight && cursor_text < text.length()) {
            char c = text[cursor_text++];
            set_background_color(cursor_x, cursor_y, background_color);
            set_symbol_color(cursor_x, cursor_y, text_color);
            switch (c) {
                case 'a': case 'A': set_symbol(cursor_x, cursor_y, TILE_A, false, false); break;
                case 'b': case 'B': set_symbol(cursor_x, cursor_y, TILE_B, false, false); break;
                case 'c': case 'C': set_symbol(cursor_x, cursor_y, TILE_C, false, false); break;
                case 'd': case 'D': set_symbol(cursor_x, cursor_y, TILE_D, false, false); break;
                case 'e': case 'E': set_symbol(cursor_x, cursor_y, TILE_E, false, false); break;
                case 'f': case 'F': set_symbol(cursor_x, cursor_y, TILE_F, false, false); break;
                case 'g': case 'G': set_symbol(cursor_x, cursor_y, TILE_G, false, false); break;
                case 'h': case 'H': set_symbol(cursor_x, cursor_y, TILE_H, false, false); break;
                case 'i': case 'I': set_symbol(cursor_x, cursor_y, TILE_I, false, false); break;
                case 'j': case 'J': set_symbol(cursor_x, cursor_y, TILE_J, false, false); break;
                case 'k': case 'K': set_symbol(cursor_x, cursor_y, TILE_K, false, false); break;
                case 'l': case 'L': set_symbol(cursor_x, cursor_y, TILE_L, false, false); break;
                case 'm': case 'M': set_symbol(cursor_x, cursor_y, TILE_M, false, false); break;
                case 'n': case 'N': set_symbol(cursor_x, cursor_y, TILE_N, false, false); break;
                case 'o': case 'O': set_symbol(cursor_x, cursor_y, TILE_O, false, false); break;
                case 'p': case 'P': set_symbol(cursor_x, cursor_y, TILE_P, false, false); break;
                case 'q': case 'Q': set_symbol(cursor_x, cursor_y, TILE_Q, false, false); break;
                case 'r': case 'R': set_symbol(cursor_x, cursor_y, TILE_R, false, false); break;
                case 's': case 'S': set_symbol(cursor_x, cursor_y, TILE_S, false, false); break;
                case 't': case 'T': set_symbol(cursor_x, cursor_y, TILE_T, false, false); break;
                case 'u': case 'U': set_symbol(cursor_x, cursor_y, TILE_U, false, false); break;
                case 'v': case 'V': set_symbol(cursor_x, cursor_y, TILE_V, false, false); break;
                case 'w': case 'W': set_symbol(cursor_x, cursor_y, TILE_W, false, false); break;
                case 'x': case 'X': set_symbol(cursor_x, cursor_y, TILE_X, false, false); break;
                case 'y': case 'Y': set_symbol(cursor_x, cursor_y, TILE_Y, false, false); break;
                case 'z': case 'Z': set_symbol(cursor_x, cursor_y, TILE_Z, false, false); break;
                case ' ': set_symbol(cursor_x, cursor_y, TILE_NONE, false, false); break;
                case '.': set_symbol(cursor_x, cursor_y, TILE_PERIOD, false, false); break;
                case '-': set_symbol(cursor_x, cursor_y, TILE_HYPHEN, false, false); break;
                case '+': set_symbol(cursor_x, cursor_y, TILE_PLUS, false, false); break;
                case ':': set_symbol(cursor_x, cursor_y, TILE_COLON, false, false); break;
                case '0': set_symbol(cursor_x, cursor_y, TILE_0, false, false); break;
                case '1': set_symbol(cursor_x, cursor_y, TILE_1, false, false); break;
                case '2': set_symbol(cursor_x, cursor_y, TILE_2, false, false); break;
                case '3': set_symbol(cursor_x, cursor_y, TILE_3, false, false); break;
                case '4': set_symbol(cursor_x, cursor_y, TILE_4, false, false); break;
                case '5': set_symbol(cursor_x, cursor_y, TILE_5, false, false); break;
                case '6': set_symbol(cursor_x, cursor_y, TILE_6, false, false); break;
                case '7': set_symbol(cursor_x, cursor_y, TILE_7, false, false); break;
                case '8': set_symbol(cursor_x, cursor_y, TILE_8, false, false); break;
                case '9': set_symbol(cursor_x, cursor_y, TILE_9, false, false); break;
                default: set_symbol(cursor_x, cursor_y, TILE_NONE, false, false); break;
            }
            cursor_x++;
        }
    }

    unsigned char symbol(unsigned int x, unsigned int y) const {
        return _pixels[y * ViewWidth + x].tile;
    }
    bool flip_x(unsigned int x, unsigned int y) const {
        return _pixels[y * ViewWidth + x].flip_x;
    }
    bool flip_y(unsigned int x, unsigned int y) const {
        return _pixels[y * ViewWidth + x].flip_y;
    }
    unsigned char background_color(unsigned int x, unsigned int y) const {
        return _pixels[y * ViewWidth + x].background_color;
    }
    unsigned char symbol_color(unsigned int x, unsigned int y) const {
        return _pixels[y * ViewWidth + x].symbol_color;
    }

    // Serialization and Compression
    // FLAGS:
        // bit 0: flip x                        0x01
        // bit 1: flip y                        0x02
        // bit 2: symbo changed                 0x04
        // bit 3: background color changed      0x08
        // bit 4: symbol color changed          0x10
        // bits 5-7: #repeats (specifies how often this pixel gets repeated without changing any of its elements)

    // SYMBOL (optional: defined by 'symbol changed')
        // bits 0-7: Symbol identifier [0-255]

    // BACKGROUND COLOR (optional: defined by 'background color changed')
        // bits 0-7: Color identifier [0-255]

    // SYMBOL COLOR (optional: defined by 'symbol color changed')
        // bits 0-7: Color identifier [0-255]
private:
    // Get information about witch elements changed, compared to the pixel next to it
    bool pixel_changed(unsigned int pixel_index,
                       bool& flags_changed,
                       bool& symbol_changed,
                       bool& background_color_changed,
                       bool& symbol_color_changed) const {
        flags_changed = false;
        symbol_changed = false;
        background_color_changed = false;
        symbol_color_changed = false;

        if (pixel_index == 0) {
            // the first pixel always has to be specified completely
            flags_changed = true;
            symbol_changed = true;
            background_color_changed = true;
            symbol_color_changed = true;
            return true;
        }

        // Check which elements changed
        if (_pixels[pixel_index - 1].flip_x != _pixels[pixel_index].flip_x ||
            _pixels[pixel_index - 1].flip_y != _pixels[pixel_index].flip_y) {
            flags_changed = true;
        }
        if (_pixels[pixel_index - 1].tile != _pixels[pixel_index].tile) {
            symbol_changed = true;
        }
        if (_pixels[pixel_index - 1].background_color != _pixels[pixel_index].background_color) {
            background_color_changed = true;
        }
        if (_pixels[pixel_index - 1].symbol_color != _pixels[pixel_index].symbol_color) {
            symbol_color_changed = true;
        }

        // Return true if one of the elements changed
        return flags_changed || symbol_changed || background_color_changed || symbol_color_changed;
    }

public:
    void deserialize(const unsigned char data[MAX_SERIALIZE_SIZE], const unsigned int size) {
        PlayerViewPixel current_pixel = {0, false, false, 0, 0};
        unsigned int data_index = 0;
        unsigned char repeats = 0;
        // Fill all pixels based on the data stream
        for (int i = 0; i < NUMBER_OF_PIXELS; ++i) {
            if (repeats == 0) {
                // Get a new flags field from the stream
                if (size <= data_index) {
                    std::cerr << "ERROR: Unexpected packet size." << std::endl;
                    std::cerr << "  At Pixel: " << i << std::endl;
                    return;
                }
                unsigned char flags_field = data[data_index++];

                // Parse the flags field
                current_pixel.flip_x = (flags_field & 0x01) != 0x00;
                current_pixel.flip_y = (flags_field & 0x02) != 0x00;
                bool symbol_changed = (flags_field & 0x04) != 0x00;
                bool background_color_changed = (flags_field & 0x08) != 0x00;
                bool symbol_color_changed = (flags_field & 0x10) != 0x00;
                repeats = (flags_field & 0xE0) >> 5;

                if (symbol_changed) {
                    // Get the Symbol from the stream
                    if (size <= data_index) {
                        std::cerr << "ERROR: Unexpected packet size." << std::endl;
                        std::cerr << "  At Pixel: " << i << std::endl;
                        return;
                    }
                    current_pixel.tile = data[data_index++];
                }
                if (background_color_changed) {
                    // Get the Background Color from the stream
                    if (size <= data_index) {
                        std::cerr << "ERROR: Unexpected packet size." << std::endl;
                        std::cerr << "  At Pixel: " << i << std::endl;
                        return;
                    }
                    current_pixel.background_color = data[data_index++];
                }
                if (symbol_color_changed) {
                    // Get the Symbol Color from the stream
                    if (size <= data_index) {
                        std::cerr << "ERROR: Unexpected packet size." << std::endl;
                        std::cerr << "  At Pixel: " << i << std::endl;
                        return;
                    }   
                    current_pixel.symbol_color = data[data_index++];
                }
            } else {
                // Repeat the pixel from before
                repeats--;
            }
            _pixels[i] = current_pixel;
        }
        if (size != data_index) {
            std::cerr << "ERROR: Unexpected packet size." << std::endl;
            std::cerr << "  Packet larger than expected." << std::endl;
        }
    }
    void serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size) const {
        // offset for the next data element to write
        unsigned int data_index = 0;
        
        // We have to keep a pointer to the last flag field, in order to change the repeat count
        unsigned char* last_flags_field = 0;
        
        for (unsigned int i = 0; i < NUMBER_OF_PIXELS; ++i) {
            bool flags_changed = false;
            bool symbol_changed = false;
            bool background_color_changed = false;
            bool symbol_color_changed = false;

            // Get the current number of repeats of the last pixel
            unsigned char repeats = 0;
            if (last_flags_field) {
                repeats = ((*last_flags_field) & 0xE0) >> 5;
            }

            // Do we have to insert a new flags element?
            if (pixel_changed(i, flags_changed,
                              symbol_changed,
                              background_color_changed,
                              symbol_color_changed) || repeats >= 7) {
                // At least one element changed compared to the last pixel or we hit the repeat limit
                // Insert a new Flags field
                data[data_index++] = (_pixels[i].flip_x ? 0x01 : 0x00) | (_pixels[i].flip_y ? 0x02 : 0x00);
                // Keep track of this new flags field
                last_flags_field = data + (data_index - 1);

                if (symbol_changed) {
                    // Write the new symbol
                    data[data_index++] = _pixels[i].tile;

                    // Set the Flag in the flags field
                    *last_flags_field |= 0x04;
                }

                if (background_color_changed) {
                    // Write the new background color
                    data[data_index++] = _pixels[i].background_color;

                    // Set the Flag in the flags field
                    *last_flags_field |= 0x08;
                }

                if (symbol_color_changed) {
                    // Write the new symbol color
                    data[data_index++] = _pixels[i].symbol_color;

                    // Set the Flag in the flags field
                    *last_flags_field |= 0x10;
                }
            } else {
                // We can repeat this pixel
                repeats++;

                // Clear the current repeat count
                *last_flags_field ^= ((*last_flags_field) & 0xE0);

                // Store the new repeat count
                *last_flags_field |= (repeats << 5);
            }
        }

        // Return the size of this packet
        size = data_index;
    }
};

#endif
