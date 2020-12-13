all:
	g++ -g ./src/client.cpp ./src/device.cpp ./src/socketHandler.cpp -lpthread -o client
	g++ -g ./src/server.cpp ./src/device.cpp ./src/socketHandler.cpp -lpthread -o server