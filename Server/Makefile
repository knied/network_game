CC=clang++
CFLAGS=-Wall -I. -stdlib=libc++
TARGET=NetworkGameServer
SOURCES=main.cpp GameServer.cpp NetworkServer.cpp ../Entity.cpp ../Network.cpp ../VirtualConnection.cpp ../PlayerController.cpp ../AIController.cpp ../World.cpp ../PlayerView.cpp ../PlayerInput.cpp ../Noise2D.cpp

all: NetworkGameServer

NetworkGameServer:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf *.o NetworkGameServer	
