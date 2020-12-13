#ifndef __socketHandler
#define __socketHandler
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>
#define SERVER 1
#define CLIENT 0
#define BUFFER_LENGTH 2080
class socketHandler {
   private:
    int socketFd;
    int cli_Fd;
    void setUpServerMode(int PORT);
    void setUpClientMode(int PORT);

   public:
    socketHandler(int PORT, bool TYPE);
    void sendSocket(bool* message, int length);
    void recvSocket(bool* message, int& length);
};
#endif