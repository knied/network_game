// =================================================================
// Author: Kristof Niederholtmeyer
//

#include "../Entity.h"
#include "../PlayerView.h"
#include "../PlayerInput.h"
#include <stdlib.h>

class EntityLock {
    Entity _entity;
    bool _locked;

public:
    EntityLock() {
        _locked = false;
    }
    EntityLock(const EntityLock&) {
        _locked = false;
    }
    const EntityLock& operator = (const EntityLock&) {
        _locked = false;
        return *this;
    }

    /*EntityLock(const Entity& entity) : _entity(entity) {
        _entity.lock_inventory();
        _locked = true;
    }*/
    ~EntityLock() {
        unlock();
    }
    void lock(const Entity& entity) {
        if (_locked) unlock();
        if (entity.inventory_locked()) return;
        _entity = entity;
        _entity.lock_inventory();
        _locked = true;
    }

    void unlock() {
        if (_locked) {
            _entity.unlock_inventory();
            _locked = false;
        }
    }

    Entity* entity() {
        if (!_locked) return 0;
        return &_entity;
    }
};

class PlayerController {
public:
    enum {ViewWidth = 24};
    enum {ViewHeight = 24};

private:
    typedef PlayerView<ViewWidth, ViewHeight> PlayerViewType;
    typedef PlayerInput PlayerInputType;
    enum ViewDir {
        ViewUp,
        ViewDown,
        ViewLeft,
        ViewRight
    };

    unsigned int _identifier;
    Entity _entity;
    PlayerViewType _view;
    PlayerInputType _input;
    // controls
    int _north, _south;
    int _west, _east;
    int _up, _down;
    bool _deal_damage;

    //unsigned int _other_entity;
    //bool _other_entity_accessable;
    EntityLock _locked_entity;

    bool _in_inventory;
    bool _switch_inventory;

    int _selected;
    int _lines;

    unsigned char _active_item;

    int _equiped;

    ViewDir _view_dir;

    void draw_controls(unsigned int y, std::vector<Entity>& entities);
    void draw_health_bar(unsigned int x, unsigned int y);
    void create_world_view(const World& world, std::vector<Entity>& entities);
    void create_inventory_view(std::vector<Entity>& entities);

    // no copying allowed
    PlayerController(const PlayerController&);
    const PlayerController& operator = (const PlayerController&);

public:
    PlayerController(unsigned int identifier, const Entity& entity);
    ~PlayerController();
    void update(const World& world, std::vector<Entity>& entities);
    void draw(const World& world, std::vector<Entity>& entities);
    void handle_input();
    /*int x() const;
    int y() const;
    int z() const;
    unsigned char symbol() const;
    unsigned char color() const;*/
    const Entity& entity() const;

    PlayerViewType& view();
    PlayerInputType& input();
    unsigned int identifier() const;
};
