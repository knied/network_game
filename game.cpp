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

void move_entity(int &x, int &y, int &z, const World& world, int diff_x, int diff_y, int diff_z) {
    int new_position_x = x + diff_x;
    int new_position_y = y + diff_y;
    int new_position_z = z + diff_z;

    if (new_position_x < 0) {
        new_position_x = 0;
    }
    if (new_position_x >= world.width()) {
        new_position_x = world.width() - 1;
    }

    if (new_position_y < 0) {
        new_position_y = 0;
    }
    if (new_position_y >= world.height()) {
        new_position_y = world.height() - 1;
    }

    if (new_position_z < 0) {
        new_position_z = 0;
    }
    if (new_position_z >= world.depth()) {
        new_position_z = world.depth() - 1;
    }
    
    // handle movement on the same level
    if (world.at(new_position_x, new_position_y, z) != BLOCK_VOID &&
        world.at(new_position_x, new_position_y, z) != BLOCK_LADDER) {
        // blocked, can we go up?
        if (world.at(x, y, z) != BLOCK_LADDER)
        {
            new_position_z = z + 1;
            if (world.at(new_position_x, new_position_y, z + 1) != BLOCK_VOID &&
                world.at(new_position_x, new_position_y, z + 1) != BLOCK_LADDER) {
                // collision -> do not move
                new_position_x = x;
                new_position_y = y;
            }
        } else {
            // collision -> do not move
            new_position_x = x;
            new_position_y = y;
        }
    }

    // handle movement in vertical direction
    if (world.at(new_position_x, new_position_y, new_position_z) != BLOCK_VOID &&
        world.at(new_position_x, new_position_y, new_position_z) != BLOCK_LADDER) {
        new_position_z = z;
    }
    
    x = new_position_x;
    y = new_position_y;
    z = new_position_z;
    
    // are we falling?
    if (world.at(x, y, z - 1) == BLOCK_VOID && world.at(x, y, z) != BLOCK_LADDER) {
        // we are!
        z--;
    }
    
    if (z < 0) {
        z = 0;
    }
    if (z >= world.depth()) {
        z = world.depth() - 1;
    }
}

template<unsigned int ViewWidth, unsigned int ViewHeight>
void view_from_position(PlayerView<ViewWidth, ViewHeight>& result,
                        const World& world,
                        const int position_x,
                        const int position_y,
                        const int position_z) {
    for (unsigned int y = 0; y < ViewHeight; ++y) {
        for (unsigned int x = 0; x < ViewWidth; ++x) {
            int x_index = position_x + x - ViewWidth / 2;
            int y_index = position_y + y - ViewHeight / 2;

            if (world.visible(x_index, y_index, position_z, position_x, position_y, position_z)) {
                // determine which block to show
                int i = 0;
                for (; position_z - i >= 0; ++i) {
                    if (world.at(x_index, y_index, position_z - i) != BLOCK_VOID) {
                        if (world.at(x_index, y_index, position_z - i) == BLOCK_WATER) {
                            result.set_symbol(x, y, TILE_WAVE, false, false);
                            if (world.at(x_index, y_index, position_z - i - 1) == BLOCK_WATER) {
                                // deep water
                                result.set_symbol_color(x, y, (rand() % 16 == 0) ? COLOR_WHITE : COLOR_DEEP_WATER);
                                result.set_background_color(x, y, COLOR_DEEP_WATER);
                            } else {
                                // water
                                result.set_symbol_color(x, y, (rand() % 16 == 0) ? COLOR_WHITE : COLOR_WATER);
                                result.set_background_color(x, y, COLOR_WATER);
                            }
                        } else {
                            // no water
                            result.set_symbol(x, y, TILE_ARROW_UP, false, false);

                            unsigned char block = world.at(x_index, y_index, position_z - i);
                            switch (block) {
                                case BLOCK_DIRT: {
                                    result.set_background_color(x, y, COLOR_DIRT);
                                    result.set_symbol_color(x, y, COLOR_DIRT);
                                } break;
                                case BLOCK_SAND: {
                                    result.set_background_color(x, y, COLOR_SAND);
                                    result.set_symbol_color(x, y, COLOR_SAND);
                                } break;
                                case BLOCK_STONE: {
                                    result.set_background_color(x, y, COLOR_STONE);
                                    result.set_symbol_color(x, y, COLOR_STONE);
                                } break;
                                case BLOCK_GRASS: {
                                    unsigned char color_code = COLOR_GRASS0;
                                    if (position_z - i == world.depth() / 2 + 1) {
                                        color_code = COLOR_GRASS1;
                                    } else if (position_z - i == world.depth() / 2 + 2) {
                                        color_code = COLOR_GRASS2;
                                    } else if (position_z - i >= world.depth() / 2 + 3) {
                                        color_code = COLOR_GRASS3;
                                    }

                                    result.set_background_color(x, y, color_code);
                                    result.set_symbol_color(x, y, color_code);
                                } break;
                                case BLOCK_LADDER: {
                                    result.set_symbol(x, y, TILE_STAIRS, false, false);
                                    result.set_background_color(x, y, COLOR_LADDER);
                                    result.set_symbol_color(x, y, COLOR_WHITE);
                                } break;
                                default: std::cout << "WARNING: Unknown block" << std::endl;
                            }
                        }
                        break;
                    }
                }
                if (position_z - i < 0) {
                    // no ground
                    result.set_symbol(x, y, TILE_CHECKER, false, false);
                    result.set_background_color(x, y, COLOR_LIGHT_GRAY);
                    result.set_symbol_color(x, y, COLOR_DARK_GRAY);
                } else {
                    // can we go that way?
                    if (world.enterable(x_index, y_index, position_z)) {
                        // we could
                        // is this a ladder?
                        if (world.at(x_index, y_index, position_z) != BLOCK_LADDER) {
                            // this is not a ladder, could we fall?
                            if (world.at(x_index, y_index, position_z - 1) == BLOCK_VOID) {
                                // we would fall
                                // do we need to show the symbol?
                                if (world.at(x_index, y_index - 1, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index - 1, y_index, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index, y_index + 1, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index + 1, y_index, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index - 1, y_index - 1, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index + 1, y_index + 1, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index - 1, y_index + 1, position_z - 1) != BLOCK_VOID ||
                                    world.at(x_index + 1, y_index - 1, position_z - 1) != BLOCK_VOID) {
                                    result.set_symbol(x, y, TILE_ARROW_UP, false, true);
                                    result.set_symbol_color(x, y, COLOR_WHITE);
                                }
                            }
                        }
                    } else {
                        // this level is blocked
                        // can we go up?
                        if (world.enterable(x_index, y_index, position_z + 1)) {
                            // we can go up
                            result.set_symbol(x, y, TILE_ARROW_UP, false, false);
                            result.set_symbol_color(x, y, COLOR_WHITE);
                        } else {
                            // this way is blocked
                            result.set_symbol(x, y, TILE_HATCHING, false, false);
                            result.set_symbol_color(x, y, COLOR_WHITE);
                        }
                    }
                }
            } else {
                result.set_symbol(x, y, TILE_CHECKER, false, false);
                result.set_background_color(x, y, COLOR_LIGHT_GRAY);
                result.set_symbol_color(x, y, COLOR_DARK_GRAY);
            }
        }
    }
}

void Game::update_tiles() {
    // Serialize the input just for fun.
    unsigned char serialized_input[PlayerInputType::MAX_SERIALIZE_SIZE];
    unsigned int serialized_input_size = 0;
    _input.serialize(serialized_input, serialized_input_size);
    std::cout << "Serialized input size: " << serialized_input_size << std::endl;

    // Clear the inputs
    _input = PlayerInputType();

    // Deserialize the inputs
    _input.deserialize(serialized_input, serialized_input_size);

    // Update the controls
    PlayerInputType::KeyEvent event;
    while (_input.next_event(event)) {
        switch (event.key) {
            case KEY_A: case KEY_LEFT: _control_west_east.set_b(event.down); break;
            case KEY_D: case KEY_RIGHT: _control_west_east.set_a(event.down); break;
            case KEY_W: case KEY_UP: _control_north_south.set_a(event.down); break;
            case KEY_S: case KEY_DOWN: _control_north_south.set_b(event.down); break;
            case KEY_COMMA: _control_up_down.set_a(event.down); break;
            case KEY_PERIOD: _control_up_down.set_b(event.down); break;
        }
    }

    // to recover from lost packets: update controls with the current key state
    _control_west_east.set_b(_input.key_state(KEY_A) || _input.key_state(KEY_LEFT));
    _control_west_east.set_a(_input.key_state(KEY_D) || _input.key_state(KEY_RIGHT));
    _control_north_south.set_a(_input.key_state(KEY_W) || _input.key_state(KEY_UP));
    _control_north_south.set_b(_input.key_state(KEY_S) || _input.key_state(KEY_DOWN));
    _control_up_down.set_a(_input.key_state(KEY_COMMA));
    _control_up_down.set_b(_input.key_state(KEY_PERIOD));

    // update player position
    move_entity(_position_x, _position_y, _position_z, _world,
                _control_west_east.get(), _control_north_south.get(), _control_up_down.get());

    // update the players view
    view_from_position(_view, _world, _position_x, _position_y, _position_z);

    // show the player character
    _view.set_symbol(ViewWidth / 2, ViewHeight / 2, 0, false, false);
    _view.set_symbol_color(ViewWidth / 2, ViewHeight / 2, COLOR_PLAYER0);

    // Serialize the View just for fun. Later, this will be done by the serber
    unsigned char serialized_view[PlayerViewType::MAX_SERIALIZE_SIZE];
    unsigned int serialized_view_size = 0;
    _view.serialize(serialized_view, serialized_view_size);
    std::cout << "Serilized view size: " << serialized_view_size << "/" << PlayerViewType::MAX_SERIALIZE_SIZE << std::endl;
    std::cout << "Saved " << (1.0f - (float)serialized_view_size / (float)PlayerViewType::MAX_SERIALIZE_SIZE) * 100.0f << "\%" << std::endl;

    // Clear the view, to be sure, that the derserialization works
    _view = PlayerViewType();

    // Deserialize the packet. Later this will be done on the client
    _view.deserialize(serialized_view, serialized_view_size);
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

/*void Game::set_control(ControlID control, bool state) {
    switch (control) {
        case CONTROL_WEST: _control_west_east.set_b(state); break;
        case CONTROL_EAST: _control_west_east.set_a(state); break;
        case CONTROL_NORTH: _control_north_south.set_a(state); break;
        case CONTROL_SOUTH: _control_north_south.set_b(state); break;
        case CONTROL_UP: _control_up_down.set_a(state); break;
        case CONTROL_DOWN: _control_up_down.set_b(state); break;
    }
}*/

void Game::key_down(unsigned char key) {
    _input.key_down(key);
}
void Game::key_up(unsigned char key) {
    _input.key_up(key);
}
