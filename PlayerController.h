#include "../Entity.h"
#include "../PlayerView.h"
#include "../PlayerInput.h"

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

    unsigned int _other_entity;
    bool _other_entity_accessable;

    bool _in_inventory;
    bool _switch_inventory;

    int _selected;
    int _lines;

    unsigned char _active_item;

    ViewDir _view_dir;

    void draw_controls(unsigned int y, std::vector<Entity>& entities);
    void draw_health_bar(unsigned int x, unsigned int y);
    void create_world_view(const World& world, std::vector<Entity>& entities);
    void create_inventory_view(std::vector<Entity>& entities);

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
