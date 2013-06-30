#include "PlayerController.h"

void PlayerController::draw_controls(unsigned int y, std::vector<Entity>& entities) {
    _view.set_symbol_color(1,y,COLOR_WHITE);
    _view.set_symbol(1,y,TILE_UP,false,false);
    _view.set_symbol_color(2,y,COLOR_WHITE);
    _view.set_symbol(2,y,TILE_UP,false,true);
    _view.set_text(3,y,": Change Selection", COLOR_BLACK, COLOR_WHITE);

    if (_active_item == ITEM_NONE) {
        unsigned char item = ITEM_NONE;
        if (_selected < 8) {
            item = (_entity.inventory())[_selected];
        } else {
            item = (entities[_other_entity].inventory())[_selected - 8];
        }
        if (item == ITEM_POTION) {
            _view.set_symbol_color(2,y-1,COLOR_WHITE);
            _view.set_symbol(2,y-1,TILE_RIGHT,true,false);
            _view.set_text(3,y-1,": Grab", COLOR_BLACK, COLOR_WHITE);

            _view.set_symbol_color(2,y-2,COLOR_WHITE);
            _view.set_symbol(2,y-2,TILE_RIGHT,false,false);
            _view.set_text(3,y-2,": Drink", COLOR_BLACK, COLOR_WHITE);
        } else if (item == ITEM_SWORD) {
            _view.set_symbol_color(2,y-1,COLOR_WHITE);
            _view.set_symbol(2,y-1,TILE_RIGHT,true,false);
            _view.set_text(3,y-1,": Grab", COLOR_BLACK, COLOR_WHITE);
        }

    } else {
        _view.set_symbol_color(2,y-1,COLOR_WHITE);
        _view.set_symbol(2,y-1,TILE_RIGHT,false,false);
        _view.set_text(3,y-1,": Put", COLOR_BLACK, COLOR_WHITE);
    }
}

void PlayerController::draw_health_bar(unsigned int x, unsigned int y) {
    unsigned int cursor_x = x;
    for (unsigned int i = 0; i < _entity.health(); ++i) {
        //_view.set_background_color(cursor_x, y, COLOR_BLACK);
        _view.set_symbol_color(cursor_x, y, COLOR_RED);
        _view.set_symbol(cursor_x, y, TILE_FULL_HEART, false, false);
        cursor_x++;
    }
    for (unsigned int i = _entity.health(); i < 5; ++i) {
        //_view.set_background_color(cursor_x, y, COLOR_BLACK);
        _view.set_symbol_color(cursor_x, y, COLOR_RED);
        _view.set_symbol(cursor_x, y, TILE_EMPTY_HEART, false, false);
        cursor_x++;
    }
}

void PlayerController::create_world_view(const World& world, std::vector<Entity>& entities) {
    const int position_x = x();
    const int position_y = y();
    const int position_z = z();

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
                            _view.set_symbol(x, y, TILE_WAVE, false, false);
                            if (world.at(x_index, y_index, position_z - i - 1) == BLOCK_WATER) {
                                // deep water
                                _view.set_symbol_color(x, y, (rand() % 16 == 0) ? COLOR_WHITE : COLOR_DEEP_WATER);
                                _view.set_background_color(x, y, COLOR_DEEP_WATER);
                            } else {
                                // water
                                _view.set_symbol_color(x, y, (rand() % 16 == 0) ? COLOR_WHITE : COLOR_WATER);
                                _view.set_background_color(x, y, COLOR_WATER);
                            }
                        } else {
                            // no water
                            _view.set_symbol(x, y, TILE_ARROW_UP, false, false);

                            unsigned char block = world.at(x_index, y_index, position_z - i);
                            switch (block) {
                                case BLOCK_DIRT: {
                                    _view.set_background_color(x, y, COLOR_DIRT);
                                    _view.set_symbol_color(x, y, COLOR_DIRT);
                                } break;
                                case BLOCK_SAND: {
                                    _view.set_background_color(x, y, COLOR_SAND);
                                    _view.set_symbol_color(x, y, COLOR_SAND);
                                } break;
                                case BLOCK_STONE: {
                                    _view.set_background_color(x, y, COLOR_STONE);
                                    _view.set_symbol_color(x, y, COLOR_STONE);
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

                                    _view.set_background_color(x, y, color_code);
                                    _view.set_symbol_color(x, y, color_code);
                                } break;
                                case BLOCK_LADDER: {
                                    _view.set_symbol(x, y, TILE_STAIRS, false, false);
                                    _view.set_background_color(x, y, COLOR_LADDER);
                                    _view.set_symbol_color(x, y, COLOR_WHITE);
                                } break;
                                default: std::cout << "WARNING: Unknown block" << std::endl;
                            }
                        }
                        break;
                    }
                }
                if (position_z - i < 0) {
                    // no ground
                    _view.set_symbol(x, y, TILE_CHECKER, false, false);
                    _view.set_background_color(x, y, COLOR_LIGHT_GRAY);
                    _view.set_symbol_color(x, y, COLOR_DARK_GRAY);
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
                                    _view.set_symbol(x, y, TILE_ARROW_UP, false, true);
                                    _view.set_symbol_color(x, y, COLOR_WHITE);
                                }
                            }
                        }
                    } else {
                        // this level is blocked
                        // can we go up?
                        if (world.enterable(x_index, y_index, position_z + 1)) {
                            // we can go up
                            _view.set_symbol(x, y, TILE_ARROW_UP, false, false);
                            _view.set_symbol_color(x, y, COLOR_WHITE);
                        } else {
                            // this way is blocked
                            _view.set_symbol(x, y, TILE_HATCHING, false, false);
                            _view.set_symbol_color(x, y, COLOR_WHITE);
                        }
                    }
                }

                // Show players
                int z = position_z;
                bool found = false;
                while(z > 0 && world.transparent(x_index, y_index, z)) {
                    for (unsigned int i = 0; i < entities.size(); ++i) {
                        if (entities[i].deals_damage_at(x_index, y_index, z)) {
                            _view.set_background_color(x, y, COLOR_WHITE);
                            _view.set_symbol(x, y, TILE_NONE, false, false);
                        }
                        if (!found && entities[i].x() == x_index && entities[i].y() == y_index && entities[i].z() == z) {
                            _view.set_symbol(x, y, entities[i].symbol(), entities[i].flip_x(), entities[i].flip_y());
                            _view.set_symbol_color(x, y, entities[i].color());
                            found = true;
                            //break;
                        }
                    }
                    z--;
                }
            } else {
                _view.set_symbol(x, y, TILE_CHECKER, false, false);
                _view.set_background_color(x, y, COLOR_LIGHT_GRAY);
                _view.set_symbol_color(x, y, COLOR_DARK_GRAY);
            }
        }
    }
    draw_health_bar(0, 23);
}

void PlayerController::create_inventory_view(std::vector<Entity>& entities) {
    for (unsigned int y = 0; y < ViewWidth; ++y) {
        for (unsigned int x = 0; x < ViewWidth; ++x) {
            _view.set_background_color(x, y, COLOR_BLACK);
            _view.set_symbol_color(x, y, COLOR_BLACK);
            _view.set_symbol(x, y, TILE_NONE, false, false);
        }
    }
    // draw side lines
    for (unsigned int y = (_other_entity_accessable ? 6 : 15); y < ViewHeight; ++y) {
        _view.set_background_color(0, y, COLOR_BLACK);
        _view.set_symbol_color(0, y, COLOR_WHITE);
        _view.set_symbol(0, y, TILE_FRAME_TB, false, false);
        _view.set_background_color(23, y, COLOR_BLACK);
        _view.set_symbol_color(23, y, COLOR_WHITE);
        _view.set_symbol(23, y, TILE_FRAME_TB, false, false);
    }

    // draw top line
    _view.set_background_color(0, 23, COLOR_BLACK);
    _view.set_symbol_color(0, 23, COLOR_WHITE);
    _view.set_symbol(0, 23, TILE_FRAME_RB, false, false);
    _view.set_background_color(23, 23, COLOR_BLACK);
    _view.set_symbol_color(23, 23, COLOR_WHITE);
    _view.set_symbol(23, 23, TILE_FRAME_RB, true, false);
    for (unsigned int x = 1; x < ViewWidth - 1; ++x) {
        _view.set_background_color(x, 23, COLOR_BLACK);
        _view.set_symbol_color(x, 23, COLOR_WHITE);
        _view.set_symbol(x, 23, TILE_FRAME_LR, false, false);
    }
    _view.set_text(2, 23, " YOU ", COLOR_BLACK, COLOR_WHITE);



    int line = 0;

    // draw players item names
    for (unsigned int i = 0; i < 8; i++) {
        std::string item_name = "---";
        unsigned char item = (_entity.inventory())[i];
        if (line == _selected && _active_item != ITEM_NONE) {
            item = _active_item;
        }
        switch (item) {
        case ITEM_SWORD: item_name = "Sword"; break;
        case ITEM_POTION: item_name = "Potion"; break;
        default: item_name = "---"; break;
        }
        if (line == _selected) {
            _view.set_text(22 - item_name.length() - (_active_item != ITEM_NONE ? 2 : 0), 22 - i, item_name, COLOR_WHITE, COLOR_BLACK);
        } else {
            _view.set_text(22 - item_name.length(), 22 - i, item_name, COLOR_BLACK, COLOR_WHITE);
        }
        line++;
    }

    // draw player health
    draw_health_bar(2, 22);

    if (_other_entity_accessable) {
        // draw middle line
        _view.set_background_color(0, 14, COLOR_BLACK);
        _view.set_symbol_color(0, 14, COLOR_WHITE);
        _view.set_symbol(0, 14, TILE_FRAME_LTB, true, false);
        _view.set_background_color(23, 14, COLOR_BLACK);
        _view.set_symbol_color(23, 14, COLOR_WHITE);
        _view.set_symbol(23, 14, TILE_FRAME_LTB, false, false);
        for (unsigned int x = 1; x < ViewWidth - 1; ++x) {
            _view.set_background_color(x, 14, COLOR_BLACK);
            _view.set_symbol_color(x, 14, COLOR_WHITE);
            _view.set_symbol(x, 14, TILE_FRAME_LR, false, false);
        }
        _view.set_text(2, 14, " CHEST ", COLOR_BLACK, COLOR_WHITE);

        // draw chest item names
        for (unsigned int i = 0; i < 8; i++) {
            std::string item_name = "---";
            unsigned char item = (entities[_other_entity].inventory())[i];
            if (line == _selected && _active_item != ITEM_NONE) {
                item = _active_item;
            }
            switch (item) {
            case ITEM_SWORD: item_name = "Sword"; break;
            case ITEM_POTION: item_name = "Potion"; break;
            default: item_name = "---"; break;
            }
            if (line == _selected) {
                _view.set_text(22 - item_name.length() - (_active_item != ITEM_NONE ? 2 : 0), 13 - i, item_name, COLOR_WHITE, COLOR_BLACK);
            } else {
                _view.set_text(22 - item_name.length(), 13 - i, item_name, COLOR_BLACK, COLOR_WHITE);
            }
            line++;
        }

        // draw bottom line
        _view.set_background_color(0, 5, COLOR_BLACK);
        _view.set_symbol_color(0, 5, COLOR_WHITE);
        _view.set_symbol(0, 5, TILE_FRAME_RB, false, true);
        _view.set_background_color(23, 5, COLOR_BLACK);
        _view.set_symbol_color(23, 5, COLOR_WHITE);
        _view.set_symbol(23, 5, TILE_FRAME_RB, true, true);
        for (unsigned int x = 1; x < ViewWidth - 1; ++x) {
            _view.set_background_color(x, 5, COLOR_BLACK);
            _view.set_symbol_color(x, 5, COLOR_WHITE);
            _view.set_symbol(x, 5, TILE_FRAME_LR, false, false);
        }

        draw_controls(4, entities);
    } else {
        // draw bottom line
        _view.set_background_color(0, 14, COLOR_BLACK);
        _view.set_symbol_color(0, 14, COLOR_WHITE);
        _view.set_symbol(0, 14, TILE_FRAME_RB, false, true);
        _view.set_background_color(23, 14, COLOR_BLACK);
        _view.set_symbol_color(23, 14, COLOR_WHITE);
        _view.set_symbol(23, 14, TILE_FRAME_RB, true, true);
        for (unsigned int x = 1; x < ViewWidth - 1; ++x) {
            _view.set_background_color(x, 14, COLOR_BLACK);
            _view.set_symbol_color(x, 14, COLOR_WHITE);
            _view.set_symbol(x, 14, TILE_FRAME_LR, false, false);
        }
        draw_controls(13, entities);
    }
}

PlayerController::PlayerController(unsigned int identifier, const Entity& entity)
    : _identifier(identifier), _entity(entity), _north(0), _south(0), _west(0), _east(0), _up(0), _down(0), _in_inventory(false), _selected(0), _active_item(ITEM_NONE) {
    (_entity.inventory())[0] = ITEM_SWORD;
    (_entity.inventory())[1] = ITEM_POTION;
    _entity.hurt();
    _entity.hurt();
    _view_dir = ViewDown;
    _switch_inventory = false;
    _other_entity_accessable = false;
    _entity.lock_inventory();
    _deal_damage = false;
}

void PlayerController::update(const World& world, std::vector<Entity>& entities) {
    // update the entity (like falling)
    //_entity.update(world, entities);

    _entity.clear_damages();

    if (_switch_inventory) {
        _in_inventory = !_in_inventory;
        if (!_in_inventory) {
            if (_active_item != ITEM_NONE) {
                // deactivate this item
                if (_selected < 8) {
                    (_entity.inventory())[_selected] = _active_item;
                    _active_item = ITEM_NONE;
                } else {
                    (entities[_other_entity].inventory())[_selected - 8] = _active_item;
                    _active_item = ITEM_NONE;
                }
            }
        }
    }

    if (!_in_inventory) {
        // Handle key holding
        if (_input.key_state(KEY_A) || _input.key_state(KEY_LEFT)) _west = 1;
        if (_input.key_state(KEY_D) || _input.key_state(KEY_RIGHT)) _east = 1;
        if (_input.key_state(KEY_W) || _input.key_state(KEY_UP)) _north = 1;
        if (_input.key_state(KEY_S) || _input.key_state(KEY_DOWN)) _south = 1;
        if (_input.key_state(KEY_COMMA)) _down = 1;
        if (_input.key_state(KEY_PERIOD)) _up = 1;

        if (_east - _west > 0) _view_dir = ViewRight;
        if (_east - _west < 0) _view_dir = ViewLeft;
        if (_north - _south > 0) _view_dir = ViewUp;
        if (_north - _south < 0) _view_dir = ViewDown;

        switch (_view_dir) {
            case ViewLeft: _entity.set_symbol(TILE_GUY_SIDE, true, false); break;
            case ViewRight: _entity.set_symbol(TILE_GUY_SIDE, false, false); break;
            case ViewUp: _entity.set_symbol(TILE_GUY_BACK, false, false); break;
            case ViewDown: _entity.set_symbol(TILE_GUY, false, false); break;
        }

        if (_deal_damage) {
            int look_at_x = _entity.x();
            if (_view_dir == ViewLeft) look_at_x--;
            if (_view_dir == ViewRight) look_at_x++;
            int look_at_y = _entity.y();
            if (_view_dir == ViewDown) look_at_y--;
            if (_view_dir == ViewUp) look_at_y++;

            _entity.deal_damage(look_at_x, look_at_y, _entity.z());
        } else {
            // move the players entity
            _entity.move(_east - _west,
                         _north - _south,
                         _up - _down,
                         world,
                         entities);
        }
    }

    // can we see an other entities inventory?
    if (_other_entity_accessable) entities[_other_entity].unlock_inventory();
    _other_entity = 0;
    _other_entity_accessable = false;
    int look_at_x = _entity.x();
    if (_view_dir == ViewLeft) look_at_x--;
    if (_view_dir == ViewRight) look_at_x++;
    int look_at_y = _entity.y();
    if (_view_dir == ViewDown) look_at_y--;
    if (_view_dir == ViewUp) look_at_y++;
    for (;_other_entity < entities.size(); ++_other_entity) {
        if (entities[_other_entity].x() == look_at_x &&
            entities[_other_entity].y() == look_at_y &&
            entities[_other_entity].z() == _entity.z() &&
            !entities[_other_entity].inventory_locked()) {
            _other_entity_accessable = true;
            if (_in_inventory) {
                entities[_other_entity].lock_inventory();
            }
            break;
        }
    }

    if (_in_inventory) {
        _selected -= _north - _south;
        _lines = 8;
        if (_other_entity_accessable) _lines = 16;
        if (_selected < 0) _selected = _lines-1;
        if (_selected >= _lines) _selected = 0;

        if (_active_item != ITEM_NONE) {
            bool selection_clear = true;
            do {
                selection_clear = true;
                if (_selected < 8) {
                    selection_clear = (_entity.inventory())[_selected] == ITEM_NONE;
                } else {
                    selection_clear = (entities[_other_entity].inventory())[_selected - 8] == ITEM_NONE;
                }

                if (!selection_clear) {
                    _selected -= _north - _south;
                    if (_selected < 0) _selected = _lines-1;
                    if (_selected >= _lines) _selected = 0;
                }
            } while (!selection_clear);
        }

        if (_east - _west < 0) {
            if (_active_item == ITEM_NONE) {
                // activate this item
                if (_selected < 8) {
                    _active_item = (_entity.inventory())[_selected];
                    (_entity.inventory())[_selected] = ITEM_NONE;
                } else {
                    _active_item = (entities[_other_entity].inventory())[_selected - 8];
                    (entities[_other_entity].inventory())[_selected - 8] = ITEM_NONE;
                }
            }
        } else if (_east - _west > 0) {
            if (_active_item != ITEM_NONE) {
                // deactivate this item
                if (_selected < 8) {
                    (_entity.inventory())[_selected] = _active_item;
                    _active_item = ITEM_NONE;
                } else {
                    (entities[_other_entity].inventory())[_selected - 8] = _active_item;
                    _active_item = ITEM_NONE;
                }
            } else {
                // use item
                if (_selected < 8) {
                    if ((_entity.inventory())[_selected] == ITEM_POTION) {
                        (_entity.inventory())[_selected] = ITEM_NONE;
                        _entity.heal();
                    }
                } else {
                    if ((entities[_other_entity].inventory())[_selected - 8] == ITEM_POTION) {
                        (entities[_other_entity].inventory())[_selected - 8] = ITEM_NONE;
                        _entity.heal();
                    }
                }
            }
        }
    }

    _switch_inventory = false;
    _deal_damage = false;
    // reset the controls
    _west = _east = 0;
    _north = _south = 0;
    _up = _down = 0;
}

void PlayerController::draw(const World& world, std::vector<Entity>& entities) {
    if (_in_inventory) {
        create_inventory_view(entities);
    } else {
        create_world_view(world, entities);
    }
}

void PlayerController::handle_input() {
    PlayerInputType::KeyEvent event;
    while (_input.next_event(event)) {
        if (event.down) {
            switch (event.key) {
                case KEY_A: case KEY_LEFT: _west = 1; break;
                case KEY_D: case KEY_RIGHT: _east = 1; break;
                case KEY_W: case KEY_UP: _north = 1; break;
                case KEY_S: case KEY_DOWN: _south = 1; break;
                case KEY_COMMA: _down = 1; break;
                case KEY_PERIOD: _up = 1; break;
                case KEY_I: _switch_inventory = true; break;
                case KEY_SPACE: _deal_damage = true; break;
                default: break;
            }
        }
    }
}

int PlayerController::x() const {return _entity.x();}
int PlayerController::y() const {return _entity.y();}
int PlayerController::z() const {return _entity.z();}
unsigned char PlayerController::symbol() const {return _entity.symbol();}
unsigned char PlayerController::color() const {return _entity.color();}
const Entity& PlayerController::entity() const {
    return _entity;
}

PlayerController::PlayerViewType& PlayerController::view() {return _view;}
PlayerController::PlayerInputType& PlayerController::input() {return _input;}
unsigned int PlayerController::identifier() const {return _identifier;}
