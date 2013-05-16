#include "game.h"
#include "Display.h"
#include "defines.h"

Game::Game() :
_width(24), _height(24),
_world_width(128), _world_height(128), _world_depth(128),
_position_x(_world_width / 2), _position_y(_world_height / 2), _position_z(0),
_move_x(0), _move_y(0),
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

    update_tiles();
}

void Game::handle_input(float dt) {
    _control_left_right.update(dt);
    _control_up_down.update(dt);
}

void Game::update_tiles() {
    // update player position
    int diff_x = _control_left_right.get();
    int diff_y = _control_up_down.get();

    int new_position_x = _position_x + diff_x;
    int new_position_y = _position_y + diff_y;
    int new_position_z = _position_z;

    if (new_position_x < 0) {
        new_position_x = 0;
    }
    if (new_position_x >= _world.width()) {
        new_position_x = _world.width() - 1;
    }

    if (new_position_y < 0) {
        new_position_y = 0;
    }
    if (new_position_y >= _world.height()) {
        new_position_y = _world.height() - 1;
    }
    
    if (_world.at(new_position_x, new_position_y, new_position_z) != TILE_VOID) {
        // blocked, can we go up?
        new_position_z++;
        if (_world.at(new_position_x, new_position_y, new_position_z) != TILE_VOID) {
            // collision -> do not move
            new_position_x = _position_x;
            new_position_y = _position_y;
            new_position_z = _position_z;
        }
    }
    
    _position_x = new_position_x;
    _position_y = new_position_y;
    _position_z = new_position_z;
    
    // are we falling?
    if (_world.at(_position_x, _position_y, _position_z - 1) == TILE_VOID) {
        // we are!
        _position_z--;
    }
    
    if (_position_z < 0) {
        _position_z = 0;
    }
    if (_position_z >= _world.depth()) {
        _position_z = _world.depth() - 1;
    }

    Display display(_width, _height,
                    _position_x, _position_y, _position_z,
                    _world);

    const Color text_color = {255,255,255,255};

    const Color water_color = {100,168,200,255};
    const Color deep_water_color = {40,124,200,255};
    const Color wave_color = {255,255,255,255};

    const Color dirt_color = {128,64,10,255};
    const Color sand_color = {220,200,100,255};
    const Color stone_color = {200,200,200,255};
    const Color grass3_color = {128,224,128,255};
    const Color grass2_color = {96,192,96,255};
    const Color grass1_color = {64,160,64,255};
    const Color grass0_color = {32,128,32,255};
    
    const Color guy_color = {200,80,80,255};

    _tiles.resize(_width * _height);
    for(int y = 0; y < _height; ++y) {
        for(int x = 0; x < _width; ++x) {
            unsigned char display_tile = display.tile_at(x, y);
            unsigned char navigation = display.navigation_at(x, y);

            Tile tile = {14, 1, 0, 0, {200,200,200,255}, {128,128,128,255}};

            switch (display_tile) {
                case TILE_WATER: {
                    tile.color0 = water_color;
                    tile.color1 = wave_color;
                    if (rand() % 8 == 0) {
                        tile.x = 14;
                        tile.y = 2;
                    } else {
                        tile.x = 6;
                        tile.y = 0;
                    }
                } break;
                case TILE_DEEP_WATER: {
                    tile.color0 = deep_water_color;
                    tile.color1 = wave_color;
                    if (rand() % 16 == 0) {
                        tile.x = 14;
                        tile.y = 2;
                    } else {
                        tile.x = 6;
                        tile.y = 0;
                    }
                } break;
                case TILE_DIRT: {
                    tile.color0 = dirt_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                case TILE_SAND: {
                    tile.color0 = sand_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                case TILE_STONE: {
                    tile.color0 = stone_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                case TILE_GRASS0: {
                    tile.color0 = grass0_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                case TILE_GRASS1: {
                    tile.color0 = grass1_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                case TILE_GRASS2: {
                    tile.color0 = grass2_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                case TILE_GRASS3: {
                    tile.color0 = grass3_color;
                    tile.color1 = text_color;
                    tile.x = 6;
                    tile.y = 0;
                } break;
                    
                default:
                    break;
            }

            switch (navigation) {
                case NAV_UP: {
                    if (display.navigation_at(x-1, y) == NAV_LEVEL ||
                        display.navigation_at(x+1, y) == NAV_LEVEL ||
                        display.navigation_at(x, y-1) == NAV_LEVEL ||
                        display.navigation_at(x, y+1) == NAV_LEVEL) {
                        tile.x = 0;
                        tile.y = 12;
                    } else {
                        //symbol = SYMBOL_LEVEL;
                    }
                } break;
                case NAV_DOWN: {
                    if (display.navigation_at(x-1, y) == NAV_LEVEL ||
                        display.navigation_at(x+1, y) == NAV_LEVEL ||
                        display.navigation_at(x, y-1) == NAV_LEVEL ||
                        display.navigation_at(x, y+1) == NAV_LEVEL) {
                        tile.flip_v = 1;
                        tile.x = 0;
                        tile.y = 12;
                    } else {
                        //symbol = SYMBOL_LEVEL;
                    }
                } break;
                case NAV_FALL: {
                    if (display.navigation_at(x-1, y) == NAV_LEVEL ||
                        display.navigation_at(x+1, y) == NAV_LEVEL ||
                        display.navigation_at(x, y-1) == NAV_LEVEL ||
                        display.navigation_at(x, y+1) == NAV_LEVEL) {
                        tile.flip_v = 1;
                        tile.x = 0;
                        tile.y = 12;
                    } else {
                        //symbol = SYMBOL_LEVEL;
                    }
                } break;
                case NAV_BLOCKED: {
                    if (display.navigation_at(x-1, y) == NAV_LEVEL ||
                        display.navigation_at(x+1, y) == NAV_LEVEL ||
                        display.navigation_at(x, y-1) == NAV_LEVEL ||
                        display.navigation_at(x, y+1) == NAV_LEVEL) {
                        tile.x = 14;
                        tile.y = 4;
                    } else {
                        
                    }
                } break;
                    
                default:
                    break;
            }

            if (x == _width / 2 && y == _height / 2) {
                tile.x = 0;
                tile.y = 0;
                tile.flip_v = 0;
                tile.flip_h = 0;
                tile.color1 = guy_color;
            }

            /*Color color0 = {rand() % 256, rand() % 256, rand() % 256, 255};
            Color color1 = {rand() % 256, rand() % 256, rand() % 256, 255};
            Tile tile = {rand() % 16, rand() % 16, color0, color1};*/
            _tiles[_width * y + x] = tile;
        }
    }
}

void Game::update(float dt) {
    handle_input(dt);

    _draw_acumulator += dt;
    if (_draw_acumulator > 0.1f) {
        update_tiles();
        _draw_acumulator = 0.0f;   
    }
    
}

const Tile& Game::tile(int x, int y) const {
    return _tiles[_width * y + x];
}

void Game::set_control(ControlID control, bool state) {
    switch (control) {
        case CONTROL_LEFT: _control_left_right.set_b(state); break;
        case CONTROL_RIGHT: _control_left_right.set_a(state); break;
        case CONTROL_UP: _control_up_down.set_a(state); break;
        case CONTROL_DOWN: _control_up_down.set_b(state); break;
    }
}
