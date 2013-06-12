//
//  World.h
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#ifndef __ascii_art__World__
#define __ascii_art__World__

#include <iostream>
#include <vector>
#include <functional>

#include "defines.h"

template<unsigned int ViewWidth, unsigned int ViewHeight>
class PlayerView {
public:
    enum {FLAGS_FIELD_SIZE = 1};
    enum {SYMBOL_FIELD_SIZE = 1};
    enum {COLORS_FIELD_SIZE = 2};
    enum {NUMBER_OF_PIXELS = ViewWidth * ViewHeight};
    enum {MAX_SERIALIZED_SIZE = NUMBER_OF_PIXELS * (FLAGS_FIELD_SIZE + SYMBOL_FIELD_SIZE + COLORS_FIELD_SIZE)};

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

    // SYMBOL
        // bits 0-7: Symbol identifier [0-255]

    // BACKGROUND COLOR
        // bits 0-7: Color identifier [0-255]

    // SYMBOL COLOR
        // bits 0-7: Color identifier [0-255]
private:
    // Get information about witch elements changed, relative to the last pixel
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
    void deserialize(const unsigned char data[MAX_SERIALIZED_SIZE], const unsigned int size) {
        PlayerViewPixel current_pixel = {0, false, false, 0, 0};
        unsigned int data_index = 0;
        unsigned char repeats = 0;
        for (int i = 0; i < NUMBER_OF_PIXELS; ++i) {
            //std::cout << repeats << std::endl;
            if (repeats == 0) {
                // Get a new flags field from the stream
                if (size <= data_index) {
                    std::cerr << "ERROR: Unexpected packet size." << std::endl;
                    std::cerr << "  At Pixel: " << i << std::endl;
                    return;
                }
                unsigned char flags_field = data[data_index++];
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
                repeats--;
            }
            _pixels[i] = current_pixel;
        }
        if (size != data_index) {
            std::cerr << "ERROR: Unexpected packet size." << std::endl;
            std::cerr << "  Packet larger than expected." << std::endl;
        }
    }
    void serialize(unsigned char data[MAX_SERIALIZED_SIZE], unsigned int& size) const {
        bool flags_changed = false;
        bool symbol_changed = false;
        bool background_color_changed = false;
        bool symbol_color_changed = false;
        unsigned int data_index = 0;
        // We have to keep a pointer to the last flag field, in order to change the repeat count
        unsigned char* last_flags_field = 0;
        for (unsigned int i = 0; i < NUMBER_OF_PIXELS; ++i) {
            unsigned char repeats = 0;
            if (last_flags_field) {
                repeats = ((*last_flags_field) & 0xE0) >> 5;
            }
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
                    data[data_index++] = _pixels[i].tile;
                    *last_flags_field |= 0x04;
                }

                if (background_color_changed) {
                    data[data_index++] = _pixels[i].background_color;
                    *last_flags_field |= 0x08;
                }

                if (symbol_color_changed) {
                    data[data_index++] = _pixels[i].symbol_color;
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
        size = data_index;
    }
};

class World {
    int _width; // west - east
    int _height; // north - south
    int _depth; // up - down
    
    std::vector<unsigned char> _data;

    struct BresenhamBackTransformation {
        int _m00; int _m01;
        int _m10; int _m11;

        int x(const int x, const int y) const {return _m00 * x + _m01 * y;}
        int y(const int x, const int y) const {return _m10 * x + _m11 * y;}

        BresenhamBackTransformation() : _m00(1), _m01(0), _m10(0), _m11(1) {}
        BresenhamBackTransformation(int m00, int m01, int m10, int m11) : _m00(m00), _m01(m01), _m10(m10), _m11(m11) {}
    };
    bool bresenham(int x0, int y0, int x1, int y1, int z, const BresenhamBackTransformation& transformation) const;

public:
    World(int width, int height, int depth);
    
    unsigned char at(const int x, const int y, const int z) const;
    void set(const int x, const int y, const int z, const unsigned char block);
    
    bool enterable(const int x, const int y, const int z) const;
    bool transparent(const int x, const int y, const int z) const;
    bool visible(const int x, const int y, const int z,
                 const int from_x, const int from_y, const int from_z) const;
    
    int width() const;
    int height() const;
    int depth() const;

    void move_entity(int &x, int &y, int &z, int diff_x, int diff_y, int diff_z) const;

    template<unsigned int ViewWidth, unsigned int ViewHeight>
    void view_from_position(PlayerView<ViewWidth, ViewHeight>& result,
                            const int position_x,
                            const int position_y,
                            const int position_z) const {
        for (unsigned int y = 0; y < ViewHeight; ++y) {
            for (unsigned int x = 0; x < ViewWidth; ++x) {
                int x_index = position_x + x - ViewWidth / 2;
                int y_index = position_y + y - ViewHeight / 2;

                if (visible(x_index, y_index, position_z, position_x, position_y, position_z)) {
                    // determine which block to show
                    int i = 0;
                    for (; position_z - i >= 0; ++i) {
                        if (at(x_index, y_index, position_z - i) != BLOCK_VOID) {
                            if (at(x_index, y_index, position_z - i) == BLOCK_WATER) {
                                result.set_symbol(x, y, TILE_WAVE, false, false);
                                if (at(x_index, y_index, position_z - i - 1) == BLOCK_WATER) {
                                    // deep water
                                    result.set_symbol_color(x, y, (rand() % 16 == 0) ? COLOR_WHITE : COLOR_DEEP_WATER);
                                    result.set_background_color(x, y, COLOR_DEEP_WATER);
                                } else {
                                    // water
                                    result.set_symbol_color(x, y, (rand() % 16 == 0) ? COLOR_WHITE : COLOR_WATER);
                                    result.set_background_color(x, y, COLOR_WATER);
                                }
                            } else {
                                // no water
                                result.set_symbol(x, y, TILE_ARROW_UP, false, false);

                                unsigned char block = at(x_index, y_index, position_z - i);
                                switch (block) {
                                    case BLOCK_DIRT: {
                                        result.set_background_color(x, y, COLOR_DIRT);
                                        result.set_symbol_color(x, y, COLOR_DIRT);
                                    } break;
                                    case BLOCK_SAND: {
                                        result.set_background_color(x, y, COLOR_SAND);
                                        result.set_symbol_color(x, y, COLOR_SAND);
                                    } break;
                                    case BLOCK_STONE: {
                                        result.set_background_color(x, y, COLOR_STONE);
                                        result.set_symbol_color(x, y, COLOR_STONE);
                                    } break;
                                    case BLOCK_GRASS: {
                                        unsigned char color_code = COLOR_GRASS0;
                                        if (position_z - i == depth() / 2 + 1) {
                                            color_code = COLOR_GRASS1;
                                        } else if (position_z - i == depth() / 2 + 2) {
                                            color_code = COLOR_GRASS2;
                                        } else if (position_z - i >= depth() / 2 + 3) {
                                            color_code = COLOR_GRASS3;
                                        }

                                        result.set_background_color(x, y, color_code);
                                        result.set_symbol_color(x, y, color_code);
                                    } break;
                                    case BLOCK_LADDER: {
                                        result.set_symbol(x, y, TILE_STAIRS, false, false);
                                        result.set_background_color(x, y, COLOR_LADDER);
                                        result.set_symbol_color(x, y, COLOR_WHITE);
                                    } break;
                                    default: std::cout << "WARNING: Unknown block" << std::endl;
                                }
                            }
                            break;
                        }
                    }
                    if (position_z - i < 0) {
                        // no ground
                        result.set_symbol(x, y, TILE_CHECKER, false, false);
                        result.set_background_color(x, y, COLOR_LIGHT_GRAY);
                        result.set_symbol_color(x, y, COLOR_DARK_GRAY);
                    } else {
                        // can we go that way?
                        if (enterable(x_index, y_index, position_z)) {
                            // we could
                            // is this a ladder?
                            if (at(x_index, y_index, position_z) != BLOCK_LADDER) {
                                // this is not a ladder, could we fall?
                                if (at(x_index, y_index, position_z - 1) == BLOCK_VOID) {
                                    // we would fall
                                    // do we need to show the symbol?
                                    if (at(x_index, y_index - 1, position_z - 1) != BLOCK_VOID ||
                                        at(x_index - 1, y_index, position_z - 1) != BLOCK_VOID ||
                                        at(x_index, y_index + 1, position_z - 1) != BLOCK_VOID ||
                                        at(x_index + 1, y_index, position_z - 1) != BLOCK_VOID ||
                                        at(x_index - 1, y_index - 1, position_z - 1) != BLOCK_VOID ||
                                        at(x_index + 1, y_index + 1, position_z - 1) != BLOCK_VOID ||
                                        at(x_index - 1, y_index + 1, position_z - 1) != BLOCK_VOID ||
                                        at(x_index + 1, y_index - 1, position_z - 1) != BLOCK_VOID) {
                                        result.set_symbol(x, y, TILE_ARROW_UP, false, true);
                                        result.set_symbol_color(x, y, COLOR_WHITE);
                                    }
                                }
                            }
                        } else {
                            // this level is blocked
                            // can we go up?
                            if (enterable(x_index, y_index, position_z + 1)) {
                                // we can go up
                                result.set_symbol(x, y, TILE_ARROW_UP, false, false);
                                result.set_symbol_color(x, y, COLOR_WHITE);
                            } else {
                                // this way is blocked
                                result.set_symbol(x, y, TILE_HATCHING, false, false);
                                result.set_symbol_color(x, y, COLOR_WHITE);
                            }
                        }
                    }
                } else {
                    result.set_symbol(x, y, TILE_CHECKER, false, false);
                    result.set_background_color(x, y, COLOR_LIGHT_GRAY);
                    result.set_symbol_color(x, y, COLOR_DARK_GRAY);
                }
            }
        }
    }
};

#endif /* defined(__ascii_art__World__) */
