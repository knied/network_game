// =================================================================
// Author: Kristof Niederholtmeyer
//

#include <iostream>
#include <sys/time.h>
#include <signal.h>
#include "GameServer.h"

volatile bool terminate;

void termination_handler(int signal) {
    terminate = true;
}

int main()
{
    std::cout << "INFO: Starting server." << std::endl;

    terminate = false;
    signal(SIGINT, termination_handler);
    signal(SIGTERM, termination_handler);
    #ifdef SIGBREAK
        signal(SIGBREAK, termination_handler);
    #endif

    GameServer server;

    // Initialize the Timer
    timeval lastTime;
    gettimeofday(&lastTime, 0);

    while(!terminate) {
        // Calculate Frame Time
        timeval tmpTime = lastTime;
        gettimeofday(&lastTime, 0);
        float dt = (float)(lastTime.tv_sec - tmpTime.tv_sec) + (float)(lastTime.tv_usec - tmpTime.tv_usec) * 0.000001f;

        // Update the Game
        server.update(dt);

        // We do not want to use 100% of the CPU
        usleep(1000);
    }

    std::cout << "INFO: Shutting down server." << std::endl;

    return 0;
}

