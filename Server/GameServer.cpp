#include "GameServer.h"

template<unsigned int ViewWidth, unsigned int ViewHeight>
void view_from_position(PlayerView<ViewWidth, ViewHeight>& result,
                        const World& world,
                        const std::vector<PlayerController>& players,
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
                        if (players[i].x() == x_index && players[i].y() == y_index && players[i].z() == z) {
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
    _players.push_back(PlayerController(identifier, _world));
}

GameServer::GameServer()
    : _update_timer(0.0f), _world(128, 128, 128) {
}

void GameServer::update(float dt) {
    _update_timer += dt;
    if (_update_timer > 0.1f) {
        _update_timer = 0.0f;

        for (unsigned int i = 0; i < _players.size(); ++i) {
            // update the players view
            view_from_position(_players[i].view(), _world, _players, _players[i].x(), _players[i].y(), _players[i].z());
            _players[i].update(_world);
        }
    }
}

void GameServer::serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size, unsigned int player_identifier) {
    std::vector<PlayerController>::iterator it;
    for (it = _players.begin(); it != _players.end(); ++it) {
        if (it->identifier() == player_identifier) {
            break;
        }
    }
    if (it == _players.end()) {
        spawn_player(player_identifier);
        it = _players.end() - 1;
    }
    it->view().serialize(data, size);
}

void GameServer::deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size, unsigned int player_identifier) {
    std::vector<PlayerController>::iterator it;
    for (it = _players.begin(); it != _players.end(); ++it) {
        if (it->identifier() == player_identifier) {
            break;
        }
    }
    if (it == _players.end()) {
        spawn_player(player_identifier);
        it = _players.end() - 1;
    }
    it->input().deserialize(data, size);
    it->handle_input();
}
