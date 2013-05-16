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

class DirControl {
    volatile bool _state_a;
    volatile bool _state_b;

    float _acumulator;

public:
    DirControl() : _state_a(false), _state_b(false), _acumulator(0.0f) {}

    void update(float dt) {
        int dir = (_state_a ? 1 : 0) - (_state_b ? 1 : 0);
        _acumulator += 10.0f * (float)dir * dt;
    }

    int get() {
        if (_acumulator >= 1.0f)
        {
            _acumulator = 0.0f;
            return 1;
        } else if (_acumulator <= -1.0f) {
            _acumulator = 0.0f;
            return -1;
        }
        return 0;
    }
    void set_a(bool state) {
        if (state && !_state_a)
        {
            _acumulator += 1.0f;
        }
        _state_a = state;
    }
    void set_b(bool state) {
        if (state && !_state_b)
        {
            _acumulator -= 1.0f;
        }
        _state_b = state;
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

    DirControl _control_up_down;
    DirControl _control_left_right;

    float _draw_acumulator;

    void handle_input(float dt);
    void update_tiles();
public:
    Game();

    int width() const { return _width; }
    int height() const { return _height; }

    void update(float dt);
    const Tile& tile(int x, int y) const;

    void set_control(ControlID control, bool state);
};

#endif // GAME_H
