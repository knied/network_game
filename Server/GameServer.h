#ifndef GAME_SERVER_H_
#define GAME_SERVER_H_

#include "../World.h"
#include "../PlayerView.h"
#include "../PlayerInput.h"
#include "../PlayerController.h"
#include "../AIController.h"

class GameServer {
public:
    enum {ViewWidth = PlayerController::ViewWidth};
    enum {ViewHeight = PlayerController::ViewHeight};

private:
    typedef PlayerView<ViewWidth, ViewHeight> PlayerViewType;
    typedef PlayerInput PlayerInputType;

    // creates a new player with identifier 'identifier'
    void spawn_player(unsigned int identifier);
    // destroy player with identifer 'identifier'
    void despawn_player(unsigned int identifier);

    unsigned int _next_monster_identifier;
    void spawn_monster(unsigned int identifier);
    void despawn_monster(unsigned int identifier);

    float _update_timer;
    World _world;
    std::vector<Entity> _entities;
    std::vector<PlayerController> _players;
    std::vector<AIController> _monster;
	
public:
    enum {MAX_SERIALIZE_SIZE = PlayerViewType::MAX_SERIALIZE_SIZE};
    enum {MAX_DESERIALIZE_SIZE = PlayerInputType::MAX_SERIALIZE_SIZE};

	GameServer();

    void update(float dt);

    void serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size, unsigned int player_identifier);
    void deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size, unsigned int player_identifier);
};

#endif
