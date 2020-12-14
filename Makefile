all:
	g++ ./src/client.cpp ./src/device.cpp ./src/socketHandler.cpp -lpthread -o client
	g++ ./src/server.cpp ./src/device.cpp ./src/socketHandler.cpp -lpthread -o server