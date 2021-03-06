#ifndef __DEVICE
#define __DEVICE
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <thread>

#include "../headers/socketHandler.hpp"
#define EVEN 0  // 0 for pair parity check, 1 for odd parity check
#define ODD 1
#define CRC_SIZE 32
#define CRCPOLI 63
#define CHAR_LENGTH 8
#define BUFFER_LENGTH 2080
#define ERROR_CHANCE 0.1
#define FLAG_SIZE 8
struct buffer {
    bool *buffer_data;
    int length;
};
typedef struct buffer buffer;
class Device {
   private:
    bool *ACK;
    bool *CRC;
    bool *NACK;
    bool parity;
    bool *flag;
    bool parityMethod;
    int id;
    socketHandler *socket;
    buffer *recv_buffer;
    buffer *send_buffer;
    void byteToBit(std::string &msg);
    void bitToChar(buffer *buffer, char *data_char);
    int countBits(bool *buffer, int length);
    void calculateCRC(bool *buffer, int length, bool *CRC);
    void clearBuffer(buffer *buffer);
    bool calculateParity(bool buffer[], int length);
    bool checkCRC(bool buffer[], int length);
    void removeCRC();
    bool checkParity(bool buffer[], int length);

   public:
    std::thread recv_thread, send_thread;
    Device(bool parityMethod, bool METHOD, int PORT);
    void recvACK();
    void listenData();
    void sendData(std::string msg);
    void recvData();
    void sendACK(bool is_ack);
    void PhysicalLayerSimulation();
    void senderDataLinkLayer(bool data[]);
    void receiverDataLinkLayer(bool data[]);
    void insertCRC();
    void insertParity();
    void removeParity();
    void insertStopFlag(bool buffer[], int &length);
    void removeStopFlag(bool buffer[], int &length);
    int findFlag(bool buffer[], int length);
    bool *getBuffer();
};

#endif