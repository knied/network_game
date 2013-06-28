#include "../PlayerView.h"
#include "../PlayerInput.h"
#include "../Server/GameServer.h"

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

    GameServer _server;
    //float _server_timer;

public:
	enum {MAX_SERIALIZE_SIZE = PlayerInputType::MAX_SERIALIZE_SIZE};
	enum {MAX_DESERIALIZE_SIZE = PlayerViewType::MAX_SERIALIZE_SIZE};

	GameClient();

	void update(float dt);
    const PlayerViewType& player_view() const;
    const Color& color(unsigned char color_code) const;

	void key_down(unsigned char key);
    void key_up(unsigned char key);

    void serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size);
    void deserialize(const unsigned char data[MAX_DESERIALIZE_SIZE], const unsigned int size);

};
