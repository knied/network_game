#include "PlayerController.h"

PlayerController::PlayerController(unsigned int identifier, const World& world)
    : _identifier(identifier), _entity(rand() % 128, rand() % 128, 127, TILE_GUY, COLOR_PLAYER0, world), _north(0), _south(0), _west(0), _east(0), _up(0), _down(0) {

}

void PlayerController::update(const World& world) {
    // Handle key holding
    if (_input.key_state(KEY_A) || _input.key_state(KEY_LEFT)) _west = 1;
    if (_input.key_state(KEY_D) || _input.key_state(KEY_RIGHT)) _east = 1;
    if (_input.key_state(KEY_W) || _input.key_state(KEY_UP)) _north = 1;
    if (_input.key_state(KEY_S) || _input.key_state(KEY_DOWN)) _south = 1;
    if (_input.key_state(KEY_COMMA)) _down = 1;
    if (_input.key_state(KEY_PERIOD)) _up = 1;

    // move the players entity
    _entity.move(_east - _west,
                 _north - _south,
                 _up - _down,
                 world);

    // reset the controls
    _west = _east = 0;
    _north = _south = 0;
    _up = _down = 0;

    // update the entity (like falling)
    _entity.update(world);
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
            }
        }
    }
}

int PlayerController::x() const {return _entity.x();}
int PlayerController::y() const {return _entity.y();}
int PlayerController::z() const {return _entity.z();}
unsigned char PlayerController::symbol() const {return _entity.symbol();}
unsigned char PlayerController::color() const {return _entity.color();}

PlayerController::PlayerViewType& PlayerController::view() {return _view;}
PlayerController::PlayerInputType& PlayerController::input() {return _input;}
unsigned int PlayerController::identifier() const {return _identifier;}
