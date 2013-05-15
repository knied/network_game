#ifndef GAME_H
#define GAME_H

#include <vector>
#include <iostream>
#include "World.h"

enum ControlID {
    CONTROL_LEFT,
    CONTROL_RIGHT,
    CONTROL_UP,
    CONTROL_DOWN
};

struct Color {
    unsigned char r, g, b, a;
};

struct Tile {
    int x, y;
    int flip_h, flip_v;
    Color color0;
    Color color1;
};

class Control {
    bool _state;
    bool _next_returned_state;

public:
    Control() : _state(false), _next_returned_state(false) {}

    int get() {
        if (_next_returned_state) {
            _next_returned_state = _state;
            return 1;
        }
        return 0;
    }
    void set(bool state) {
        _state = state;
        if (!_next_returned_state) {
            _next_returned_state = state;
        }
    }

};

class Game
{
    const int _width;
    const int _height;

    const int _world_width;
    const int _world_height;
    const int _world_depth;

    int _position_x;
    int _position_y;
    int _position_z;

    int _move_x;
    int _move_y;

    World _world;

    std::vector<Tile> _tiles;

    Control _control_up;
    Control _control_down;
    Control _control_left;
    Control _control_right;
public:
    Game();

    int width() const { return _width; }
    int height() const { return _height; }

    void update();
    const Tile& tile(int x, int y) const;

    void set_control(ControlID control, bool state);
};

#endif // GAME_H
