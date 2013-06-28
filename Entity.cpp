#include "Entity.h"

Entity::Entity(int x, int y, int z, unsigned char symbol, unsigned char color, const World& world)
    : _symbol(symbol), _color(color), _x(x), _y(y), _z(z) {
    if (world.at(x, y, z) != BLOCK_VOID) {
        // find the surface and place the entity on top of it
        for (int i = z; i < world.depth(); ++i) {
            _z = i;
            if (world.at(x, y, i) == BLOCK_VOID) {
                break;
            }
        }
    } else if (world.at(x, y, z - 1) == BLOCK_VOID) {
        for (int i = z - 1; i >= 0; --i) {
            _z = i;
            if (world.at(x, y, i-1) != BLOCK_VOID) {
                break;
            }
        }
    }

}

void Entity::update(const World& world) {
    // are we falling?
    if (world.at(_x, _y, _z - 1) == BLOCK_VOID && world.at(_x, _y, _z) != BLOCK_LADDER) {
        // we are!
        _z--;
    }

    if (_z < 0) {
        _z = 0;
    }
    if (_z >= world.depth()) {
        _z = world.depth() - 1;
    }
}

void Entity::move(int dx, int dy, int dz, const World& world) {
    int new_position_x = _x + dx;
    int new_position_y = _y + dy;
    int new_position_z = _z + dz;

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
    if (world.at(new_position_x, new_position_y, _z) != BLOCK_VOID &&
        world.at(new_position_x, new_position_y, _z) != BLOCK_LADDER) {
        // blocked, can we go up?
        if (world.at(_x, _y, _z) != BLOCK_LADDER)
        {
            new_position_z = _z + 1;
            if (world.at(new_position_x, new_position_y, _z + 1) != BLOCK_VOID &&
                world.at(new_position_x, new_position_y, _z + 1) != BLOCK_LADDER) {
                // collision -> do not move
                new_position_x = _x;
                new_position_y = _y;
            }
        } else {
            // collision -> do not move
            new_position_x = _x;
            new_position_y = _y;
        }
    }

    // handle movement in vertical direction
    if (world.at(new_position_x, new_position_y, new_position_z) != BLOCK_VOID &&
        world.at(new_position_x, new_position_y, new_position_z) != BLOCK_LADDER) {
        new_position_z = _z;
    }

    _x = new_position_x;
    _y = new_position_y;
    _z = new_position_z;

    if (_z < 0) {
        _z = 0;
    }
    if (_z >= world.depth()) {
        _z = world.depth() - 1;
    }
}

unsigned char Entity::symbol() const {
    return _symbol;
}

unsigned char Entity::color() const {
    return _color;
}

int Entity::x() const {
    return _x;
}

int Entity::y() const {
    return _y;
}

int Entity::z() const {
    return _z;
}
