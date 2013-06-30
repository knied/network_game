#include "GameServer.h"

void GameServer::spawn_player(unsigned int identifier) {
    _entities.push_back(Entity(rand() % _world.width(), rand() % _world.height(), _world.depth()-1, TILE_GUY, false, false, COLOR_PLAYER0, _world));
    _players.push_back(PlayerController(identifier, _entities.back()));
}

void GameServer::despawn_player(unsigned int identifier) {
    std::vector<PlayerController>::iterator player;
    for (player = _players.begin(); player != _players.end(); ++player) {
        if (player->identifier() == identifier) {
            break;
        }
    }
    if (player != _players.end()) {
        Entity player_entity = player->entity();
        _players.erase(player);
        // delete the entity
        /*std::vector<Entity>::iterator it;
        for (it = _entities.begin(); it != _entities.end(); ++it) {
            if (*it == player_entity) {
                _entities.erase(it);
                break;
            }
        }*/

    }
}

GameServer::GameServer()
    : _update_timer(0.0f), _world(128, 128, 128) {
    // spawn chests
    for (unsigned int i = 0; i < 100; ++i) {
        _entities.push_back(Entity(rand() % _world.width(), rand() % _world.height(), 0, TILE_CHEST, false, false, COLOR_CHEST, _world));
    }
}

void GameServer::update(float dt) {
    _update_timer += dt;
    if (_update_timer > 0.1f) {
        _update_timer = 0.0f;

        for (unsigned int i = 0; i < _entities.size(); ++i) {
            _entities[i].update(_world, _entities);
        }

        bool done = false;
        while (!done) {
            std::vector<Entity>::iterator it;
            for (it = _entities.begin(); it != _entities.end(); ++it) {
                if (it->health() == 0) break;
            }
            if (it == _entities.end()) {
                done = true;
            } else {
                // is this a player entity?
                for (unsigned int i = 0; i < _players.size(); ++i) {
                    if (*it == _players[i].entity()) {
                        despawn_player(_players[i].identifier());
                        break;
                    }
                }
                _entities.erase(it);
            }
        }

        for (unsigned int i = 0; i < _players.size(); ++i) {
            _players[i].update(_world, _entities);
        }

        for (unsigned int i = 0; i < _players.size(); ++i) {
            _players[i].draw(_world, _entities);
        }
    }
}

void GameServer::serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size, unsigned int player_identifier) {
    std::vector<PlayerController>::iterator it;
    for (it = _players.begin(); it != _players.end(); ++it) {
        if (it->identifier() == player_identifier) {
            break;
        }
    }
    if (it == _players.end()) {
        spawn_player(player_identifier);
        it = _players.end() - 1;
    }
    it->view().serialize(data, size);
}

void GameServer::deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size, unsigned int player_identifier) {
    std::vector<PlayerController>::iterator it;
    for (it = _players.begin(); it != _players.end(); ++it) {
        if (it->identifier() == player_identifier) {
            break;
        }
    }
    if (it == _players.end()) {
        spawn_player(player_identifier);
        it = _players.end() - 1;
    }
    it->input().deserialize(data, size);
    it->handle_input();
}
