#include "../headers/socketHandler.hpp"
socketHandler::socketHandler(int PORT, bool TYPE) {
    if (TYPE == SERVER)
        setUpServerMode(PORT);
    else
        setUpClientMode(PORT);

    return;
}

void socketHandler::setUpClientMode(int PORT) {
    char ip_address[20];
    strcpy(ip_address, "127.0.0.1");
    struct sockaddr_in server;

    cli_Fd = socket(AF_INET, SOCK_STREAM, 0);

    if (cli_Fd == -1) {
        std::cout << "[ERROR] Failed to create socket!" << std::endl;
        exit(0);
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_address, &server.sin_addr) <= 0) {
        std::cout << "[ERROR] Invalid Address" << std::endl;
        exit(0);
        return;
    }
    if (connect(cli_Fd, (struct sockaddr *)&server, sizeof(server))) {  // try to connect to server
        std::cout << "[ERROR] Connection Failed" << std::endl;
        exit(0);
        return;
    }
    return;
}

void socketHandler::setUpServerMode(int PORT) {
    struct sockaddr_in server;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cout << "[ERROR] Failed to create socket!" << std::endl;
        exit(0);
        return;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    memset(&server.sin_zero, 0, sizeof(server.sin_zero));
    if (bind(socketFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        std::cout << "[ERROR] Failed to bind!" << std::endl;
        exit(0);
        return;
    }

    listen(this->socketFd, 1);
    int sizeSock = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    int cli_id = 0;
    int socket_client;
    socket_client = accept(socketFd, (struct sockaddr *)&client, (socklen_t *)&sizeSock);
    if (socket_client < 0) {
        std::cout << "[ERROR] Connection Error " << std::endl;
        return;
    }
    std::cout << "[INFO] New connection accepted (IP " << inet_ntoa(client.sin_addr) << ":" << client.sin_port << ")" << std::endl;
    this->cli_Fd = socket_client;
    return;
}

void socketHandler::sendSocket(bool *message, int length) {
    send(this->cli_Fd, message, length * sizeof(bool), 0);
    return;
}
void socketHandler::recvSocket(bool *message, int &length) {
    if (!(length = recv(this->cli_Fd, message, BUFFER_LENGTH * sizeof(bool), 0))) {
        std::cout << "[ERROR] DISCONECTED!";
        exit(0);
    }
    return;
}