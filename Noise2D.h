//
//  Noise2D.h
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#ifndef __ascii_art__Noise2D__
#define __ascii_art__Noise2D__

#include <iostream>
#include <vector>
#include <stdlib.h>

class Noise2D {
    std::vector<float> _data;
    int _width;
    int _height;
    
public:
    Noise2D(int width, int height);
    
    float at(int x, int y) const;
    float interpolated_at(float x, float y) const;
};

#endif /* defined(__ascii_art__Noise2D__) */
