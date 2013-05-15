//
//  Noise2D.cpp
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#include "Noise2D.h"

Noise2D::Noise2D(int width, int height)
: _data(width * height), _width(width), _height(height) {
    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            _data[y * _width + x] = (float)rand() / (float)RAND_MAX;
        }
    }
}

float Noise2D::at(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        return 0.0f;
    }
    return _data[y * _width + x];
}

float Noise2D::interpolated_at(float x, float y) const {
    float scaled_x = x * (float)_width;
    float scaled_y = y * (float)_height;
    
    int index_x = scaled_x;
    int index_y = scaled_y;
    
    float w_x = scaled_x - (float)index_x;
    float w_y = scaled_y - (float)index_y;
    
    return at(index_x, index_y) * (1.0f - w_x) * (1.0f - w_y) +
    at(index_x + 1, index_y) * w_x * (1.0f - w_y) +
    at(index_x, index_y + 1) * (1.0f - w_x) * w_y +
    at(index_x + 1, index_y + 1) * w_x * w_y;
}