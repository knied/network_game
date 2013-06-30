#ifndef ENTITY_H_
#define ENTITY_H_

#include "defines.h"
#include "World.h"

// An entity is a movable object in the world.
class Entity {
    struct Data {
        unsigned char symbol;
        bool flip_x, flip_y;
        unsigned char color;

        bool inventory_locked;
        unsigned char inventory[8];

        unsigned int health;

        struct DamagePosition {int x, y, z;};
        std::vector<DamagePosition> damages;

        int x, y, z;
        bool colliding; // collides with other collideing entities

        unsigned int references;
    };
    Data* _data;

    enum Collision {
        NoCollision,
        WorldCollision,
        EntityCollision
    };
    Collision can_move_to(int x, int y, int z, const World& world, const std::vector<Entity>& entities, unsigned int& collider) const;

public:
    Entity(int x, int y, int z, unsigned char symbol, bool flip_x, bool flip_y, unsigned char color, const World& world);
    Entity(const Entity& entity);
    ~Entity();

    const Entity& operator = (const Entity& entity);

    bool operator == (const Entity& entity) const;
    bool operator != (const Entity& entity) const;

    void update(const World& world, const std::vector<Entity>& entities);
    void move(int dx, int dy, int dz, const World& world, const std::vector<Entity>& entities);
    void heal();
    void hurt();
    void set_colliding(bool colliding);
    void set_symbol(unsigned char symbol, bool flip_x, bool flip_y);
    void lock_inventory();
    void unlock_inventory();
    void deal_damage(int x, int y, int z);
    void clear_damages();

    unsigned char symbol() const;
    bool flip_x() const;
    bool flip_y() const;
    unsigned char color() const;
    int x() const;
    int y() const;
    int z() const;
    unsigned int health() const;
    unsigned char* inventory();
    bool colliding() const;
    bool inventory_locked() const;
    bool deals_damage_at(int x, int y, int z) const;

};

#endif
