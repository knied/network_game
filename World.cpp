//
//  World.cpp
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#include "World.h"
#include <math.h>
#include "defines.h"
#include "Noise2D.h"

World::World(int width, int height, int depth)
: _width(width), _height(height), _depth(depth), _data(width*height*depth) {
    Noise2D noise_8(16, 16);
    
    for (int z = 0; z < _depth; ++z) {
        for (int y = 0; y < _height; ++y) {
            for (int x = 0; x < _width; ++x) {
                int max = _depth / 2 + 12.0f * (noise_8.interpolated_at((float)x / (float)_width,
                                                                        (float)y / (float)_height) - 0.5f);
                if (z < max) {
                    if (z + 1 >= max) {
                        // surface
                        if (z <= _depth / 2 - 1) {
                            set(x, y, z, BLOCK_SAND);
                        } else if (z == _depth / 2) {
                            set(x, y, z, BLOCK_GRASS);
                        } else if (z == _depth / 2 + 1) {
                            set(x, y, z, BLOCK_GRASS);
                        } else if (z == _depth / 2 + 2) {
                            set(x, y, z, BLOCK_GRASS);
                        } else if (z == _depth / 2 + 3) {
                            set(x, y, z, BLOCK_GRASS);
                        } else {
                            set(x, y, z, BLOCK_STONE);
                        }
                    } else {
                        // below surface
                        set(x, y, z, BLOCK_STONE);
                    }
                    
                } else {
                    if (z < _depth / 2) {
                        set(x, y, z, BLOCK_WATER);
                    } else {
                        set(x, y, z, BLOCK_VOID);
                    }
                }
            }
        }
    }

    // place a random dungeon :)
    bool found = false;
    while(!found) {
        int dungeon_x = rand() % _width;
        int dungeon_y = rand() % _height;
        // find the surface at this position
        int i = 0;
        for (; i < _depth; ++i) {
            if (at(dungeon_x, dungeon_y, i) == BLOCK_VOID) {
                if (at(dungeon_x, dungeon_y, i-1) != BLOCK_WATER) {
                    found = true;
                }
                break;
            }
        }

        if (!found) {
            continue;
        }

        // make a room
        for (int y = -5; y < 5; ++y) {
            for (int x = -5; x < 5; ++x) {
                set(x + dungeon_x, y + dungeon_y, i - 16, BLOCK_VOID);
            }
        }

        // create a ladder
        for (int j = 0; j < 16; ++j) {
            set(dungeon_x, dungeon_y, i - j - 1, BLOCK_LADDER);
        }

        std::cout << "placed a dungeon at: " << dungeon_x << " " << dungeon_y << std::endl;
    }
}

unsigned char World::at(const int x, const int y, const int z) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height || z < 0 || z >= _depth) {
        return BLOCK_VOID; // void
    }
    return _data[z * _height * _width + y * _width + x];
}
void World::set(const int x, const int y, const int z, const unsigned char block) {
    if (x < 0 || x >= _width || y < 0 || y >= _height || z < 0 || z >= _depth) {
        return; // void
    }
    _data[z * _height * _width + y * _width + x] = block;
}

bool World::enterable(const int x, const int y, const int z) const {
    unsigned char tile = at(x, y, z);
    if (tile == BLOCK_VOID) {
        return true;
    }
    if (tile == BLOCK_LADDER) {
        return true;
    }
    return false;
}

bool World::transparent(const int x, const int y, const int z) const {
    unsigned char tile = at(x, y, z);
    if (tile == BLOCK_VOID) {
        return true;
    }
    if (tile == BLOCK_LADDER) {
        return true;
    }
    return false;
}

bool World::bresenham(int x0, int y0, int x1, int y1, int z,
                      const BresenhamBackTransformation& transformation) const {
    int a = y1 - y0;
    int b = x0 - x1;
    float m = (float)(y1 - y0) / (float)(x1 - x0);
    float t = (float)y0 - m * (float)x0;
    int c = roundf((float)(x1 - x0) * t);
    int dE = 2 * a;
    int dNE = 2 * (a + b);
    int d = 2 * a * (x0 + 1) + b * (2 * y0 + 1) + 2 * c;
    int current_x = x0;
    int current_y = y0;
    while (current_x < x1 || current_y < y1) {
        if (d > 0) {
            // NE
            d += dNE;
            current_x++;
            current_y++;
        } else {
            // E
            d += dE;
            current_x++;
        }
        
        if (!transparent(transformation.x(current_x, current_y),
                         transformation.y(current_x, current_y), z)) {
            if (current_x == x1 && current_y == y1) {
                return true;
            }
            return false;
        }
    }
    return true;
}

bool World::visible(const int x, const int y, const int z,
                    const int from_x, const int from_y, const int from_z) const {
    if (z > from_z) {
        return false;
    }
    
    int dx = x - from_x;
    int dy = y - from_y;
    
    if (dx == 0) {
        // special case to prevent division by zero
        int current_x = from_x;
        int current_y = from_y;
        if (dy > 0) {
            for (; current_y < y; ++current_y) {
                if (!transparent(current_x, current_y, from_z)) {
                    return false;
                }
            }
        } else if (dy < 0) {
            for (; current_y > y; --current_y) {
                if (!transparent(current_x, current_y, from_z)) {
                    return false;
                }
            }
        }
    } else if (dx > 0 && dy >= 0) {
        if ((float)dy / (float)dx < 1.0f) {
            /*
            //       ^y
            //   \   |   /
            //     \ | / X
            // ------|------>x
            //     / | \
            //   /   |   \
            */
            return bresenham(from_x, from_y, x, y, from_z, BresenhamBackTransformation(1,0,0,1));
        } else {
            /*
            //       ^y
            //   \   | X /
            //     \ | /
            // ------|------>x
            //     / | \
            //   /   |   \
            */
            return bresenham(from_y, from_x, y, x, from_z, BresenhamBackTransformation(0,1,1,0));
        }
    } else if (dx < 0 && dy >= 0) {
        if ((float)dy / (float)dx < -1.0f) {
            /*
            //       ^y
            //   \   |   /
            //   X \ | /
            // ------|------>x
            //     / | \
            //   /   |   \
            */
            return bresenham(from_y, -from_x, y, -x, from_z, BresenhamBackTransformation(0,-1,1,0));
        } else {
            /*
            //       ^y
            //   \ X |   /
            //     \ | /
            // ------|------>x
            //     / | \
            //   /   |   \
            */
            return bresenham(-from_x, from_y, -x, y, from_z, BresenhamBackTransformation(-1,0,0,1));
        }
    } else if (dx > 0 && dy < 0) {
        if ((float)dy / (float)dx >= -1.0f) {
            /*
            //       ^y
            //   \   |   /
            //     \ | /
            // ------|------>x
            //     / | \ X
            //   /   |   \
            */
            return bresenham(from_x, -from_y, x, -y, from_z, BresenhamBackTransformation(1,0,0,-1));
        } else {
            /*
            //       ^y
            //   \   |   /
            //     \ | /
            // ------|------>x
            //     / | \
            //   /   | X \
            */
            return bresenham(-from_y, from_x, -y, x, from_z, BresenhamBackTransformation(0,1,-1,0));
        }
    } else { // dx < 0 && dy < 0
        if ((float)dy / (float)dx > 1.0f) {
            /*
            //       ^y
            //   \   |   /
            //     \ | /
            // ------|------>x
            //     / | \
            //   / X |   \
            */
            return bresenham(-from_y, -from_x, -y, -x, from_z, BresenhamBackTransformation(0,-1,-1,0));
        } else {
            /*
            //       ^y
            //   \   |   /
            //     \ | /
            // ------|------>x
            //   X / | \
            //   /   |   \
            */
            return bresenham(-from_x, -from_y, -x, -y, from_z, BresenhamBackTransformation(-1,0,0,-1));
        }
    }
    return true;
}

int World::width() const {return _width;}
int World::height() const {return _height;}
int World::depth() const {return _depth;}
