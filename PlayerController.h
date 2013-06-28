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

    unsigned int _identifier;
    Entity _entity;
    PlayerViewType _view;
    PlayerInputType _input;
    // controls
    int _north, _south;
    int _west, _east;
    int _up, _down;

public:
    PlayerController(unsigned int identifier, const World& world);
    void update(const World& world);
    void handle_input();
    int x() const;
    int y() const;
    int z() const;
    unsigned char symbol() const;
    unsigned char color() const;

    PlayerViewType& view();
    PlayerInputType& input();
    unsigned int identifier() const;
};
