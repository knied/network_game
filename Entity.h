// =================================================================
// Author: Kristof Niederholtmeyer
//

#ifndef ENTITY_H_
#define ENTITY_H_

#include "defines.h"
#include "World.h"

// An entity is a movable object in the world.
class Entity {
    // reference counted data storage
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
    Entity();
    Entity(int x, int y, int z, unsigned char symbol, bool flip_x, bool flip_y, unsigned char color, const World& world);
    Entity(const Entity& entity);
    ~Entity();

    // this is NO COPY. entities internaly function like smart pointers
    const Entity& operator = (const Entity& entity);

    bool operator == (const Entity& entity) const;
    bool operator != (const Entity& entity) const;

    void update(const World& world, const std::vector<Entity>& entities);

    // moves the entity by dx, dy, dz and performes collision detection
    void move(int dx, int dy, int dz, const World& world, const std::vector<Entity>& entities);

    // manage the health bar
    void heal();
    void hurt();
    unsigned int health() const;

    // sets the visible representation of this entity
    void set_symbol(unsigned char symbol, bool flip_x, bool flip_y);
    unsigned char symbol() const;
    bool flip_x() const;
    bool flip_y() const;
    unsigned char color() const;

    // this locks the inventory, so that only one player can access the inventory at a time
    void lock_inventory();
    void unlock_inventory();
    unsigned char* inventory();
    bool inventory_locked() const;

    // deals damage to entities on the tile x, y, z until clear_damage() is called
    void deal_damage(int x, int y, int z);
    void clear_damages();
    bool deals_damage_at(int x, int y, int z) const;

    // position
    int x() const;
    int y() const;
    int z() const;

}; // Entity

#endif
