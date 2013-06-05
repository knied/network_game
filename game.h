#ifndef GAME_H
#define GAME_H

#include <vector>
#include <iostream>
#include "World.h"

enum ControlID {
    CONTROL_WEST,
    CONTROL_EAST,
    CONTROL_NORTH,
    CONTROL_SOUTH,
    CONTROL_UP,
    CONTROL_DOWN
};

struct Color {
    unsigned char r, g, b, a;

    Color() : r(0), g(0), b(0), a(0) {}
    Color(unsigned char red,
          unsigned char green,
          unsigned char blue,
          unsigned char alpha)
    : r(red), g(green), b(blue), a(alpha) {

    }
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
            _acumulator = 1.0f;
        }
        _state_a = state;
    }
    void set_b(bool state) {
        if (state && !_state_b)
        {
            _acumulator = -1.0f;
        }
        _state_b = state;
    }

};

class Game
{
    enum {ViewWidth = 24};
    enum {ViewHeight = 24};
    typedef PlayerView<ViewWidth, ViewHeight> PlayerViewType;

    const int _world_width;
    const int _world_height;
    const int _world_depth;

    int _position_x;
    int _position_y;
    int _position_z;

    World _world;
    PlayerViewType _view;
    Color _colors[256];

    DirControl _control_north_south;
    DirControl _control_west_east;
    DirControl _control_up_down;

    float _draw_acumulator;

    void handle_input(float dt);
    void update_tiles();
public:
    Game();

    int width() const;
    int height() const;

    void update(float dt);
    const PlayerViewType& player_view() const;
    const Color& color(unsigned char color_code) const;

    void set_control(ControlID control, bool state);
};

#endif // GAME_H
