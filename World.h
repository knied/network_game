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
};

#endif /* defined(__ascii_art__World__) */
