// =================================================================
// Author: Kristof Niederholtmeyer
//

#include "GameClient.h"
#include "../defines.h"
#include <sstream>

GameClient::GameClient() : _text_cursor_timer(0.0f) {
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
    _colors[COLOR_RED] = Color(200, 0, 0, 255);
    _colors[COLOR_CHEST] = Color(200, 128, 50, 255);
    _colors[COLOR_GREEN] = Color(0, 200, 0, 255);
}

#ifdef DO_NETWORK
bool GameClient::remote_name_valid() const {
    // check if its a valid ip address
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int p = 0;
    int result = sscanf(_remote_name.c_str(), "%d.%d.%d.%d:%d", &a, &b, &c, &d, &p);
    //std::cout << "result: " << result << std::endl;
    //std::cout << a << "." << b << "." << c << "." << d << ":" << p << std::endl;
    return result == 5;
}
#endif

void GameClient::show_input_screen() {
    for (unsigned int y = 0; y < ViewWidth; ++y) {
        for (unsigned int x = 0; x < ViewWidth; ++x) {
            _view.set_background_color(x, y, COLOR_BLACK);
            _view.set_symbol_color(x, y, COLOR_BLACK);
            _view.set_symbol(x, y, TILE_NONE, false, false);
        }
    }
#ifdef DO_NETWORK
    // CONNECT
    _view.set_text(7, 20, "CONNECT TO", COLOR_BLACK, COLOR_WHITE);
    int offset = _remote_name.length() / 2;
    _view.set_text(12-offset, 18, _remote_name, COLOR_BLACK, remote_name_valid() ? COLOR_WHITE : COLOR_RED);
    if (_text_cursor_timer > 0.25f) {
        if (_text_cursor_timer > 0.5f) {
            _text_cursor_timer = 0.0f;
        }
        _view.set_symbol(12 - offset + _remote_name.length(), 18, TILE_CURSOR, false, false);
        _view.set_symbol_color(12 - offset + _remote_name.length(), 18, COLOR_WHITE);
    }
#endif
}

void GameClient::update(float dt) {
#ifndef DO_NETWORK
    _server.update(dt);

    // simulate network communication
    unsigned char view_data[MAX_DESERIALIZE_SIZE];
    unsigned int view_size = 0;
    _server.serialize(view_data, view_size, 0);
    deserialize(view_data, view_size);

    unsigned char input_data[MAX_SERIALIZE_SIZE];
    unsigned int input_size = 0;
    serialize(input_data, input_size);
    _server.deserialize(input_data, input_size, 0);
#else
    _network.update(dt, *this);
    if (!_network.IsConnected()) {
        // allow the user to input an ip address
        _text_cursor_timer+=dt;
        show_input_screen();
    }
#endif
}

const GameClient::PlayerViewType& GameClient::player_view() const {
	return _view;
}

const Color& GameClient::color(unsigned char color_code) const {
	return _colors[color_code];
}

void GameClient::text_input(const std::string& text) {
#ifdef DO_NETWORK
    // xxx.xxx.xxx.xxx:xxxxx
    if (_remote_name.length() >= 21) return;
    std::string allowed_characters = "0123456789.:";
    for (unsigned int i = 0; i < allowed_characters.size(); ++i) {
        if (text == allowed_characters.substr(i,1)) {
            _remote_name += text;
            return;
        }
    }
#endif
}

void GameClient::text_delete() {
#ifdef DO_NETWORK
    if (_remote_name.length() == 0) return;
    _remote_name = _remote_name.substr(0,_remote_name.length()-1);
#endif
}

void GameClient::text_done() {
#ifdef DO_NETWORK
    if (!_network.IsConnected()) {
        int a = 0;
        int b = 0;
        int c = 0;
        int d = 0;
        int p = 0;
        int result = sscanf(_remote_name.c_str(), "%d.%d.%d.%d:%d", &a, &b, &c, &d, &p);
        if (result == 5) {
            std::stringstream address_stream;
            address_stream << a << "." << b << "." << c << "." << d;
            std::cout << "connect to: " << address_stream.str() << " on port: " << p << std::endl;
            _network.ConnectTo(Address(address_stream.str(), p));
        }
    }
#endif
}

void GameClient::key_down(unsigned char key) {
	_input.key_down(key);
}

void GameClient::key_up(unsigned char key) {
	_input.key_up(key);
}

void GameClient::disconnect() {
    std::cout << "Disconnected by server." << std::endl;
}

void GameClient::serialize(unsigned char data[GameClient::MAX_SERIALIZE_SIZE], unsigned int& size) {
	_input.serialize(data, size);
	// Input events have been sent to the server. Make room for new events.
	_input.clear();
}

void GameClient::deserialize(const unsigned char data[GameClient::MAX_DESERIALIZE_SIZE], const unsigned int size) {
	_view.deserialize(data, size);
}
