// =================================================================
// Author: Kristof Niederholtmeyer
//

#include "AIController.h"

AIController::AIController(unsigned int identifier, const Entity& entity) : _entity(entity), _identifier(identifier) {
    _view_dir = ViewDown;
    _timer = 0;
    (_entity.inventory())[0] = rand() % 3;
    _entity.lock_inventory();
}

AIController::~AIController() {
    _entity.unlock_inventory();
}

void AIController::update(const World& world, std::vector<Entity>& entities) {
    _entity.clear_damages();

    if (_timer > 0) {
        _timer--;
    } else {
        unsigned int rand_dir = rand() % 4;
        switch (rand_dir) {
            case 0: _view_dir = ViewLeft; break;
            case 1: _view_dir = ViewRight; break;
            case 2: _view_dir = ViewUp; break;
            case 3: _view_dir = ViewDown; break;
        }

        int dx = 0;
        int dy = 0;
        switch (_view_dir) {
            case ViewLeft: _entity.set_symbol(TILE_MONSTER_SIDE, true, false); dx = -1; break;
            case ViewRight: _entity.set_symbol(TILE_MONSTER_SIDE, false, false); dx = 1; break;
            case ViewUp: _entity.set_symbol(TILE_MONSTER_BACK, false, false); dy = 1; break;
            case ViewDown: _entity.set_symbol(TILE_MONSTER, false, false); dy = -1; break;
        }

        _timer = rand() % 8;
        if (rand() % 2 == 0) {
            _entity.move(dx, dy, 0, world, entities);
        } else {
            int look_at_x = _entity.x();
            if (_view_dir == ViewLeft) look_at_x--;
            if (_view_dir == ViewRight) look_at_x++;
            int look_at_y = _entity.y();
            if (_view_dir == ViewDown) look_at_y--;
            if (_view_dir == ViewUp) look_at_y++;

            _entity.deal_damage(look_at_x, look_at_y, _entity.z());
            _entity.deal_damage(look_at_x, look_at_y, _entity.z()-1);
        }
    }

}

const Entity& AIController::entity() const {
    return _entity;
}

unsigned int AIController::identifier() const {
    return _identifier;
}
