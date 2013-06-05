#include "game.h"
//#include "Display.h"
#include "defines.h"

Game::Game() :
//_width(24), _height(24),
_world_width(128), _world_height(128), _world_depth(128),
_position_x(_world_width / 2), _position_y(_world_height / 2), _position_z(0),
//_move_x(0), _move_y(0),
_world(_world_width, _world_height, _world_depth),
_draw_acumulator(0.0f)
{
    // find the surface and place the player on top of it
    for (int i = 0; i < _world.depth(); ++i) {
        if (_world.at(_position_x, _position_y, i) == 0) {
            _position_z = i;
            break;
        }
    }

    // initialize colors
    _colors[COLOR_BLACK] = Color(0, 0, 0, 255);
    _colors[COLOR_WHITE] = Color(255, 255, 255, 255);
    _colors[COLOR_DIRT] = Color(128, 64, 10, 255);
    _colors[COLOR_SAND] = Color(220, 200, 100, 255);
    _colors[COLOR_STONE] = Color(200, 200, 200, 255);
    _colors[COLOR_GRASS0] = Color(32, 128, 32, 255);
    _colors[COLOR_GRASS1] = Color(64, 160, 64, 255);
    _colors[COLOR_GRASS2] = Color(96, 192, 96, 255);
    _colors[COLOR_GRASS3] = Color(128, 224, 128, 255);
    _colors[COLOR_LADDER] = Color(50, 50, 50, 255);
    _colors[COLOR_WATER] = Color(100, 168, 200, 255);
    _colors[COLOR_DEEP_WATER] = Color(40, 124, 200, 255);
    _colors[COLOR_LIGHT_GRAY] = Color(200, 200, 200, 255);
    _colors[COLOR_DARK_GRAY] = Color(127, 127, 127, 255);
    _colors[COLOR_PLAYER0] = Color(200, 80, 80, 255);

    update_tiles();
}

void Game::handle_input(float dt) {
    _control_north_south.update(dt);
    _control_west_east.update(dt);
    _control_up_down.update(dt);
}

void Game::update_tiles() {
    // update player position
    _world.move_entity(_position_x, _position_y, _position_z,
                       _control_west_east.get(), _control_north_south.get(), _control_up_down.get());

    // update the players view
    _world.view_from_position(_view, _position_x, _position_y, _position_z);

    // show the player character
    _view.set_symbol(ViewWidth / 2, ViewHeight / 2, 0, false, false);
    _view.set_symbol_color(ViewWidth / 2, ViewHeight / 2, COLOR_PLAYER0);
}

void Game::update(float dt) {
    handle_input(dt);

    _draw_acumulator += dt;
    if (_draw_acumulator > 0.1f) {
        update_tiles();
        _draw_acumulator = 0.0f;   
    }
    
}

int Game::width() const {
    return ViewWidth;
}

int Game::height() const {
    return ViewHeight;
}

const Game::PlayerViewType& Game::player_view() const {
    return _view;
}

const Color& Game::color(unsigned char color_code) const {
    return _colors[color_code];
}

void Game::set_control(ControlID control, bool state) {
    switch (control) {
        case CONTROL_WEST: _control_west_east.set_b(state); break;
        case CONTROL_EAST: _control_west_east.set_a(state); break;
        case CONTROL_NORTH: _control_north_south.set_a(state); break;
        case CONTROL_SOUTH: _control_north_south.set_b(state); break;
        case CONTROL_UP: _control_up_down.set_a(state); break;
        case CONTROL_DOWN: _control_up_down.set_b(state); break;
    }
}
