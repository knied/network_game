// =================================================================
// Author: Kristof Niederholtmeyer
//

#ifndef PLAYER_INPUT_H_
#define PLAYER_INPUT_H_

#include <vector>

class PlayerInput {
    enum {HEADER_SIZE = 1};
    enum {MAX_EVENTS = 255};
    enum {STATE_SIZE = 16};

public:
	struct KeyEvent {
		unsigned char key;
		bool down;

		KeyEvent() : key(0), down(false) {}
		KeyEvent(unsigned char key, bool down)
		: key(key), down(down) {}
	};

    enum {MAX_SERIALIZE_SIZE = HEADER_SIZE + MAX_EVENTS + STATE_SIZE};

private:
	std::vector<KeyEvent> _key_events;
	bool _key_states[128];

public:
	PlayerInput() {
		for (int i = 0; i < 128; ++i) {
			_key_states[i] = false;
		}
	}

	void key_down(unsigned char key) {
		if (key < 128) {
			_key_events.push_back(KeyEvent(key, true));
			_key_states[key] = true;
		}
	}
	void key_up(unsigned char key) {
		if (key < 128) {
			_key_events.push_back(KeyEvent(key, false));
			_key_states[key] = false;
		}
	}

	bool next_event(KeyEvent& key_event) {
		if (_key_events.size() > 0) {
			key_event = _key_events.front();
			_key_events.erase(_key_events.begin());
			return true;
		}
		return false;
	}
	void clear() {
		_key_events.clear();
	}

	bool key_state(unsigned char key) const {
		if (key < 128) {
			return _key_states[key];
		}
		return false;
	}

	void serialize(unsigned char data[MAX_SERIALIZE_SIZE], unsigned int& size) const {
		data[0] = 0;
		unsigned int data_index = 1;
		if (_key_events.size() > MAX_EVENTS) {
			std::cerr << "WARNING: Too many key events. Input information will get lost." << std::endl;
		}
		for (unsigned int i = 0; i < _key_events.size() && i <= MAX_EVENTS; ++i) {
			(data[0])++;
			// most significant bit contains up/down info
			data[data_index++] = _key_events[i].key | (_key_events[i].down ? 0x80 : 0x00);
		}
		for (int i = 0; i < 16; ++i) {
			unsigned char state_chunk = 0;
			for (int j = 0; j < 8; ++j) {
				state_chunk |= ((_key_states[8 * i + j] ? 0x01 : 0x00) << j);
			}
			data[data_index++] = state_chunk;
		}
		size = data_index;
	}
	void deserialize(const unsigned char data[MAX_SERIALIZE_SIZE], const unsigned int size) {
		clear();
		if (size == 0) {
			std::cerr << "WARNING: Tryied to deserialize an empty packet." << std::endl;
			return;
		}
		unsigned int event_count = data[0];
        if (size != HEADER_SIZE + event_count + STATE_SIZE) {
			std::cerr << "WARNING: Wrong packet size." << std::endl;
			return;
		}

		unsigned int i = 1;
		for (; i <= event_count; ++i) {
			_key_events.push_back(KeyEvent(data[i] & 0x7F, (data[i] & 0x80) != 0x00));
		}
		for (int k = 0; k < 16; ++k) {
			for (int j = 0; j < 8; ++j) {
				_key_states[8 * k + j] = (data[i + k] & (0x01 << j)) != 0x00;
			}
		}
	}
};

#endif
