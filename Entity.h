#include "defines.h"
#include "World.h"

// An entity is a movable object in the world.
class Entity {
    unsigned char _symbol;
    unsigned char _color;

    int _x, _y, _z;

public:
    Entity(int x, int y, int z, unsigned char symbol, unsigned char color, const World& world);

    void update(const World& world);
    void move(int dx, int dy, int dz, const World& world);

    unsigned char symbol() const;
    unsigned char color() const;
    int x() const;
    int y() const;
    int z() const;

};
