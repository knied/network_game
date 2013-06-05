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
    PlayerViewPixel _pixels[ViewWidth * ViewHeight];

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
