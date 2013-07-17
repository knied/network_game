// =================================================================
// Author: Kristof Niederholtmeyer
//

#include "Entity.h"

Entity::Entity() : _data(new Data()) {
    _data->symbol = TILE_NONE;
    _data->flip_x = false;
    _data->flip_y = false;
    _data->color = COLOR_BLACK;
    _data->x = 0;
    _data->y = 0;
    _data->z = 0;
    _data->references = 1;
    _data->inventory_locked = false;
    for (unsigned int i = 0; i < 8; ++i) {
        _data->inventory[i] = ITEM_NONE;
    }
    _data->health = 0;
}

Entity::Entity(int x, int y, int z, unsigned char symbol, bool flip_x, bool flip_y, unsigned char color, const World& world)
    : _data(new Data()) {
    _data->symbol = symbol;
    _data->flip_x = flip_x;
    _data->flip_y = flip_y;
    _data->color = color;
    _data->x = x;
    _data->y = y;
    _data->z = z;
    _data->references = 1;
    _data->inventory_locked = false;
    for (unsigned int i = 0; i < 8; ++i) {
        _data->inventory[i] = ITEM_NONE;
    }
    _data->health = 5;

    if (world.at(x, y, z) != BLOCK_VOID) {
        // find the surface and place the entity on top of it
        for (int i = z; i < world.depth(); ++i) {
            _data->z = i;
            if (world.at(x, y, i) == BLOCK_VOID) {
                break;
            }
        }
    } else if (world.at(x, y, z - 1) == BLOCK_VOID) {
        for (int i = z - 1; i >= 0; --i) {
            _data->z = i;
            if (world.at(x, y, i-1) != BLOCK_VOID) {
                break;
            }
        }
    }

}

Entity::Entity(const Entity& entity)
    : _data(entity._data) {
    _data->references++;
}

Entity::~Entity() {
    _data->references--;
    if (_data->references == 0) {
        delete _data;
    }
}

const Entity& Entity::operator = (const Entity& entity) {
    _data->references--;
    if (_data->references == 0) {
        delete _data;
    }
    _data = entity._data;
    _data->references++;

    return *this;
}

bool Entity::operator == (const Entity& entity) const {
    return _data == entity._data;
}

bool Entity::operator != (const Entity& entity) const {
    return _data != entity._data;
}

void Entity::update(const World& world, const std::vector<Entity>& entities) {
    unsigned int collider = 0;
    if (world.at(_data->x, _data->y, _data->z) != BLOCK_LADDER &&
        world.at(_data->x, _data->y, _data->z - 1) != BLOCK_LADDER &&
        NoCollision == can_move_to(_data->x, _data->y, _data->z - 1, world, entities, collider)) {
        _data->z--; // fall down
    }

    if (_data->z < 0) {
        _data->z = 0;
    }
    if (_data->z >= world.depth()) {
        _data->z = world.depth() - 1;
    }

    for (unsigned int i = 0; i < entities.size(); ++i) {
        if (entities[i].deals_damage_at(_data->x, _data->y, _data->z)) hurt();
    }
}

Entity::Collision Entity::can_move_to(int x, int y, int z, const World& world, const std::vector<Entity>& entities, unsigned int& collider) const {
    if (x < 0 || x >= world.width()) return WorldCollision;
    if (y < 0 || y >= world.height()) return WorldCollision;
    if (z < 0 || z >= world.depth()) return WorldCollision;

    if (world.at(x, y, z) != BLOCK_VOID && world.at(x, y, z) != BLOCK_LADDER) return WorldCollision;

    for (unsigned int i = 0; i < entities.size(); ++i) {
        if (entities[i] != *this) {
            if (entities[i].x() == x && entities[i].y() == y && entities[i].z() == z) {
                collider = i;
                return EntityCollision;
            }
        }
    }

    return NoCollision;
}

void Entity::move(int dx, int dy, int dz, const World& world, const std::vector<Entity>& entities) {
    int new_position_x = _data->x + dx;
    int new_position_y = _data->y + dy;
    if(world.at(_data->x, _data->y, _data->z) != BLOCK_LADDER && dz > 0) dz = 0;
    if(world.at(_data->x, _data->y, _data->z - 1) != BLOCK_LADDER && dz < 0) dz = 0;
    int new_position_z = _data->z + dz;

    unsigned int collider = 0;
    Collision c = can_move_to(new_position_x, new_position_y, new_position_z, world, entities, collider);
    if (c == NoCollision) {
        _data->x = new_position_x;
        _data->y = new_position_y;
        _data->z = new_position_z;
    } else if (c == WorldCollision &&
               world.at(_data->x, _data->y, _data->z) != BLOCK_LADDER &&
               NoCollision == can_move_to(new_position_x, new_position_y, new_position_z + 1, world, entities, collider)) {
        _data->x = new_position_x;
        _data->y = new_position_y;
        _data->z = new_position_z + 1;
    }
}

void Entity::heal() {
    if (_data->health < 5) _data->health++;
}
void Entity::hurt() {
    if (_data->health > 0) _data->health--;
}

void Entity::set_symbol(unsigned char symbol, bool flip_x, bool flip_y) {
    _data->symbol = symbol;
    _data->flip_x = flip_x;
    _data->flip_y = flip_y;
}

void Entity::lock_inventory() {
    _data->inventory_locked = true;
}

void Entity::unlock_inventory() {
    _data->inventory_locked = false;
}

void Entity::deal_damage(int x, int y, int z) {
    Data::DamagePosition damage = {x, y, z};
    _data->damages.push_back(damage);
}

void Entity::clear_damages() {
    _data->damages.clear();
}

unsigned char Entity::symbol() const {
    return _data->symbol;
}

bool Entity::flip_x() const {
    return _data->flip_x;
}

bool Entity::flip_y() const {
    return _data->flip_y;
}

unsigned char Entity::color() const {
    return _data->color;
}

int Entity::x() const {
    return _data->x;
}

int Entity::y() const {
    return _data->y;
}

int Entity::z() const {
    return _data->z;
}

unsigned char* Entity::inventory() {
    return _data->inventory;
}

unsigned int Entity::health() const {
    return _data->health;
}

bool Entity::inventory_locked() const {
    return _data->inventory_locked;
}

bool Entity::deals_damage_at(int x, int y, int z) const {
    for (unsigned int i = 0; i < _data->damages.size(); ++i) {
        if (_data->damages[i].x == x && _data->damages[i].y == y && _data->damages[i].z == z) return true;
    }
    return false;
}
