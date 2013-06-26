#ifndef GAME_SERVER_H_
#define GAME_SERVER_H_

#include "../World.h"
#include "../PlayerView.h"
#include "../PlayerInput.h"

class DirControl {
    volatile bool _state_a;
    volatile bool _state_b;

    float _acumulator;

public:
    DirControl();

    void update(float dt);
    int get();
    void set_a(bool state);
    void set_b(bool state);
};

template<class ViewType, class InputType>
struct Player {
    unsigned int identifier;
    int x, y, z;
    ViewType view;
    InputType input;
    DirControl control_north_south;
    DirControl control_west_east;
    DirControl control_up_down;

    Player(unsigned int identifier, const World& world) : identifier(identifier) {
        x = rand() % 128;
        y = rand() % 128;
        z = 0;

        // find the surface and place the player on top of it
        for (int i = 0; i < world.depth(); ++i) {
            if (world.at(x, y, i) == 0) {
                z = i;
                break;
            }
        }
    }
};

class GameServer {
public:
    enum {ViewWidth = 24};
    enum {ViewHeight = 24};

private:
    typedef PlayerView<ViewWidth, ViewHeight> PlayerViewType;
    typedef PlayerInput PlayerInputType;

    void spawn_player(unsigned int identifier);

    float _update_timer;
    World _world;
    std::vector<Player<PlayerViewType, PlayerInputType> > _players;
	
public:
    enum {MAX_SERIALIZE_SIZE = PlayerViewType::MAX_SERIALIZE_SIZE};
    enum {MAX_DESERIALIZE_SIZE = PlayerInputType::MAX_SERIALIZE_SIZE};

	GameServer();

    void update(float dt);

    void serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size, unsigned int player_identifier);
    void deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size, unsigned int player_identifier);
};

#endif
