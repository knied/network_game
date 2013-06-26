#include "GameServer.h"

DirControl::DirControl() : _state_a(false), _state_b(false), _acumulator(0.0f) {}

void DirControl::update(float dt) {
    int dir = (_state_a ? 1 : 0) - (_state_b ? 1 : 0);
    _acumulator += 10.0f * (float)dir * dt;
}

int DirControl::get() {
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
void DirControl::set_a(bool state) {
    if (state && !_state_a)
    {
        _acumulator = 1.0f;
    }
    _state_a = state;
}
void DirControl::set_b(bool state) {
    if (state && !_state_b)
    {
        _acumulator = -1.0f;
    }
    _state_b = state;
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
                        const std::vector<Player<PlayerView<ViewWidth, ViewHeight>, PlayerInput> >& players,
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

                // Show players
                int z = position_z;
                while(z > 0 && world.transparent(x_index, y_index, z)) {
                    bool found = false;
                    for (unsigned int i = 0; i < players.size(); ++i) {
                        if (players[i].x == x_index && players[i].y == y_index && players[i].z == z) {
                            result.set_symbol(x, y, TILE_GUY, false, false);
                            result.set_symbol_color(x, y, COLOR_PLAYER0);
                            found = true;
                            break;
                        }
                    }

                    // Only show the player on top
                    if (found) {
                        break;
                    }
                    z--;
                }
            } else {
                result.set_symbol(x, y, TILE_CHECKER, false, false);
                result.set_background_color(x, y, COLOR_LIGHT_GRAY);
                result.set_symbol_color(x, y, COLOR_DARK_GRAY);
            }
        }
    }
}

void GameServer::spawn_player(unsigned int identifier) {
    _players.push_back(Player<PlayerViewType, PlayerInputType>(identifier, _world));
}

GameServer::GameServer()
    : _update_timer(0.0f), _world(128, 128, 128) {
}

void GameServer::update(float dt) {
    // Update the controls
    for (unsigned int i = 0; i < _players.size(); ++i) {
        _players[i].control_north_south.update(dt);
        _players[i].control_west_east.update(dt);
        _players[i].control_up_down.update(dt);

        PlayerInputType::KeyEvent event;
        while (_players[i].input.next_event(event)) {
            switch (event.key) {
            case KEY_A: case KEY_LEFT: _players[i].control_west_east.set_b(event.down); break;
            case KEY_D: case KEY_RIGHT: _players[i].control_west_east.set_a(event.down); break;
            case KEY_W: case KEY_UP: _players[i].control_north_south.set_a(event.down); break;
            case KEY_S: case KEY_DOWN: _players[i].control_north_south.set_b(event.down); break;
            case KEY_COMMA: _players[i].control_up_down.set_a(event.down); break;
            case KEY_PERIOD: _players[i].control_up_down.set_b(event.down); break;
            }
        }

        // to recover from lost packets: update controls with the current key state
        _players[i].control_west_east.set_b(_players[i].input.key_state(KEY_A) || _players[i].input.key_state(KEY_LEFT));
        _players[i].control_west_east.set_a(_players[i].input.key_state(KEY_D) || _players[i].input.key_state(KEY_RIGHT));
        _players[i].control_north_south.set_a(_players[i].input.key_state(KEY_W) || _players[i].input.key_state(KEY_UP));
        _players[i].control_north_south.set_b(_players[i].input.key_state(KEY_S) || _players[i].input.key_state(KEY_DOWN));
        _players[i].control_up_down.set_a(_players[i].input.key_state(KEY_COMMA));
        _players[i].control_up_down.set_b(_players[i].input.key_state(KEY_PERIOD));

        // update player position
        move_entity(_players[i].x, _players[i].y, _players[i].z,
                    _world,
                    _players[i].control_west_east.get(),
                    _players[i].control_north_south.get(),
                    _players[i].control_up_down.get());
    }


    _update_timer += dt;
    if (_update_timer > 0.1f) {
        _update_timer = 0.0f;

        for (unsigned int i = 0; i < _players.size(); ++i) {
            // update the players view
            view_from_position(_players[i].view, _world, _players, _players[i].x, _players[i].y, _players[i].z);
        }
    }
}

void GameServer::serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size, unsigned int player_identifier) {
    std::vector<Player<PlayerViewType, PlayerInputType> >::iterator it;
    for (it = _players.begin(); it != _players.end(); ++it) {
        if (it->identifier == player_identifier) {
            break;
        }
    }
    if (it == _players.end()) {
        spawn_player(player_identifier);
        it = _players.end() - 1;
    }
    it->view.serialize(data, size);
}

void GameServer::deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size, unsigned int player_identifier) {
    std::vector<Player<PlayerViewType, PlayerInputType> >::iterator it;
    for (it = _players.begin(); it != _players.end(); ++it) {
        if (it->identifier == player_identifier) {
            break;
        }
    }
    if (it == _players.end()) {
        spawn_player(player_identifier);
        it = _players.end() - 1;
    }
    it->input.deserialize(data, size);
}
