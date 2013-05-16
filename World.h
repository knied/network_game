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

#include "defines.h"

class World {
    int _width; // west - east
    int _height; // north - south
    int _depth; // up - down
    
    std::vector<unsigned char> _data;

    bool transparent(unsigned char tile) const {
        if (tile == TILE_VOID) {
            return true;
        }
        if (tile == TILE_LADDER) {
            return true;
        }
        return false;
    }

    bool enterable(unsigned char tile) const {
        if (tile == TILE_VOID) {
            return true;
        }
        if (tile == TILE_LADDER) {
            return true;
        }
        return false;
    }
    
public:
    World(int width, int height, int depth);
    
    unsigned char at(int x, int y, int z) const;
    void set(int x, int y, int z, unsigned char block);
    
    bool visible(const int x, const int y, const int z,
                 const int from_x, const int from_y, const int from_z) const;
    
    int width() const;
    int height() const;
    int depth() const;
};

#endif /* defined(__ascii_art__World__) */
