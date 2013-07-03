#ifndef GAME_CLIENT_H_
#define GAME_CLIENT_H_

#include "../PlayerView.h"
#include "../PlayerInput.h"
#include "../Server/GameServer.h"
#include "NetworkClient.h"

//#define DO_NETWORK

struct Color {
    unsigned char r, g, b, a;

    Color() : r(0), g(0), b(0), a(0) {}
    Color(unsigned char red,
          unsigned char green,
          unsigned char blue,
          unsigned char alpha)
    : r(red), g(green), b(blue), a(alpha) {

    }
};

class GameClient {
public:
	enum {ViewWidth = 24};
    enum {ViewHeight = 24};

private:
    typedef PlayerView<ViewWidth, ViewHeight> PlayerViewType;
    typedef PlayerInput PlayerInputType;

	PlayerViewType _view;
	PlayerInputType _input;

	Color _colors[256];

#ifdef DO_NETWORK
    // the client depends on a server
    NetworkClient _network;
    std::string _remote_name;
    bool remote_name_valid() const;
#else
    // include the server in the client
    GameServer _server;
#endif
    //float _server_timer;

    float _text_cursor_timer;
    void show_input_screen();

public:
	enum {MAX_SERIALIZE_SIZE = PlayerInputType::MAX_SERIALIZE_SIZE};
	enum {MAX_DESERIALIZE_SIZE = PlayerViewType::MAX_SERIALIZE_SIZE};

	GameClient();

	void update(float dt);
    const PlayerViewType& player_view() const;
    const Color& color(unsigned char color_code) const;

    void text_input(const std::string& text);
    void text_delete();
    void text_done();
	void key_down(unsigned char key);
    void key_up(unsigned char key);

    void serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size);
    void deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size);

};

#endif
