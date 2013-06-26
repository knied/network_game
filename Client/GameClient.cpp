#include "GameClient.h"
#include "../defines.h"

GameClient::GameClient() {
	// initialize colors
    _colors[COLOR_BLACK] = Color(0, 0, 0, 255);
    _colors[COLOR_WHITE] = Color(255, 255, 255, 255);
    _colors[COLOR_DIRT] = Color(128, 64, 10, 255);
    _colors[COLOR_SAND] = Color(220, 200, 100, 255);
    _colors[COLOR_STONE] = Color(200, 200, 200, 255);
    _colors[COLOR_GRASS0] = Color(32, 128, 32, 255);
    _colors[COLOR_GRASS1] = Color(64, 160, 64, 255);
    _colors[COLOR_GRASS2] = Color(96, 192, 96, 255);
    _colors[COLOR_GRASS3] = Color(128, 224, 128, 255);
    _colors[COLOR_LADDER] = Color(50, 50, 50, 255);
    _colors[COLOR_WATER] = Color(100, 168, 200, 255);
    _colors[COLOR_DEEP_WATER] = Color(40, 124, 200, 255);
    _colors[COLOR_LIGHT_GRAY] = Color(200, 200, 200, 255);
    _colors[COLOR_DARK_GRAY] = Color(127, 127, 127, 255);
    _colors[COLOR_PLAYER0] = Color(200, 80, 80, 255);
}

void GameClient::update(float dt) {
	// TODO: network stuff
}

const GameClient::PlayerViewType& GameClient::player_view() const {
	return _view;
}

const Color& GameClient::color(unsigned char color_code) const {
	return _colors[color_code];
}

void GameClient::key_down(unsigned char key) {
	_input.key_down(key);
}

void GameClient::key_up(unsigned char key) {
	_input.key_up(key);
}

void GameClient::serialize(unsigned char data[GameClient::MAX_SERIALIZE_SIZE], unsigned int& size) {
	_input.serialize(data, size);
	// Input events have been sent to the server. Make room for new events.
	_input.clear();
}

void GameClient::deserialize(const unsigned char data[GameClient::MAX_DESERIALIZE_SIZE], const unsigned int size) {
	_view.deserialize(data, size);
}
