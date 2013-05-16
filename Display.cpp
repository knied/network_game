//
//  Display.cpp
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#include "Display.h"

Display::Display(int width, int height,
                 int position_x, int position_y, int position_z,
                 const World& world)
: _width(width), _height(height), _tiles(width*height), _navigations(width*height) {
    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            int x_index = position_x + x - _width / 2;
            int y_index = position_y + y - _height / 2;
            
            set_tile(x, y, TILE_VOID);
            set_navigation(x, y, NAV_UNKNOWN);
            
            if (world.visible(x_index, y_index, position_z, position_x, position_y, position_z)) {
                // determine which block to show
                int i = 0;
                for (; position_z - i >= 0; ++i) {
                    if (world.at(x_index, y_index, position_z - i) != TILE_VOID) {
                        if (world.at(x_index, y_index, position_z - i) == TILE_WATER &&
                            world.at(x_index, y_index, position_z - i - 1) == TILE_WATER) {
                            set_tile(x, y, TILE_DEEP_WATER);
                        } else {
                            set_tile(x, y, world.at(x_index, y_index, position_z - i));
                        }
                        break;
                    }
                }
                if (position_z - i < 0) {
                    // no ground
                    set_navigation(x, y, NAV_UNKNOWN);
                } else {
                    // can we go that way?
                    if (enterable(world.at(x_index, y_index, position_z))) {
                        // we could
                        // is this a ladder?
                        if (world.at(x_index, y_index, position_z) == TILE_LADDER) {
                            // it is a ladder
                            set_navigation(x, y, NAV_LEVEL);
                        } else {
                            // this is not a ladder, could we fall?
                            if (world.at(x_index, y_index, position_z - 1) == TILE_VOID) {
                                // we would fall
                                set_navigation(x, y, NAV_FALL);
                            } else {
                                // we can walk here
                                set_navigation(x, y, NAV_LEVEL);
                            }
                        }
                    } else {
                        // this level is blocked
                        // can we go up?
                        if (enterable(world.at(x_index, y_index, position_z + 1))) {
                            // we can go up
                            set_navigation(x, y, NAV_UP);
                        } else {
                            // this way is blocked
                            set_navigation(x, y, NAV_BLOCKED);
                        }
                    }
                }
            } else {
                set_navigation(x, y, NAV_UNKNOWN);
                set_tile(x, y, TILE_VOID);
            }
        }
    }
}

void Display::set_tile(int x, int y, unsigned char tile) {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        return;
    }
    _tiles[y * _width + x] = tile;
}
void Display::set_navigation(int x, int y, unsigned char navigation) {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        return;
    }
    _navigations[y * _width + x] = navigation;
}
unsigned char Display::tile_at(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        return TILE_VOID;
    }
    return _tiles[y * _width + x];
}
unsigned char Display::navigation_at(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        return NAV_FALL;
    }
    return _navigations[y * _width + x];
}