#ifndef NETWORK_CLIENT_H_
#define NETWORK_CLIENT_H_

class NetworkClient {
public:
    NetworkClient() {}

    template<class State>
    void update(float dt, State& state) {
        // To serialize the state:
        unsigned char state_data[State::MAX_SERIALIZE_SIZE];
        unsigned int state_size = 0;
        state.serialize(state_data, state_size);

        // To deserialize a state update:
        unsigned char new_state_data[State::MAX_DESERIALIZE_SIZE];
        unsigned int new_state_size = 0;
        // TODO: fill the array and update the size (from a network packet)
        state.deserialize(new_state_data, new_state_size);
    }
};

#endif
