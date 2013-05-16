//
//  Display.h
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#ifndef __ascii_art__Display__
#define __ascii_art__Display__

#include <iostream>
#include <vector>

#include "World.h"
#include "defines.h"

class Display {
    int _width;
    int _height;
    std::vector<unsigned char> _tiles;
    std::vector<unsigned char> _navigations;
    void set_tile(int x, int y, unsigned char tile);

    bool transparent(unsigned char tile) {
        if (tile == TILE_VOID) {
            return true;
        }
        if (tile == TILE_LADDER) {
            return true;
        }
        return false;
    }

    bool enterable(unsigned char tile) {
        if (tile == TILE_VOID) {
            return true;
        }
        if (tile == TILE_LADDER) {
            return true;
        }
        return false;
    }
    
public:
    Display(int width, int height,
            int position_x, int position_y, int position_z,
            const World& world);
    
    void set_navigation(int x, int y, unsigned char navigation);
    unsigned char tile_at(int x, int y) const;
    unsigned char navigation_at(int x, int y) const;
};

#endif /* defined(__ascii_art__Display__) */
