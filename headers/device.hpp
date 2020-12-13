#ifndef __DEVICE
#define __DEVICE
#include <stdlib.h>

#include <iostream>
#define EVEN 0  // 0 for pair parity check, 1 for odd parity check
#define ODD 1
#define CRC_SIZE 32
#define CRCPOLI 63
#define CHAR_LENGTH 8
#define BUFFER_LENGTH 2080
#define ERROR_CHANCE 80
#define FLAG_SIZE 8
struct buffer {
    bool *buffer_data;
    int length;
};
typedef struct buffer buffer;
class Device {
   private:
    bool *CRC;
    bool parity;
    bool *flag;
    bool parityMethod;
    int id;
    buffer *recv_buffer;
    buffer *send_buffer;
    void byteToBit(std::string &msg);
    int countBits(bool *buffer, int length);
    void calculateCRC(bool *buffer, int length, bool *&CRC);
    void clearBuffer(buffer *buffer);
    bool calculateParity(bool buffer[], int length);
    bool checkCRC(bool buffer[], int length);
    void removeCRC();
    bool checkParity(bool buffer[], int length);

   public:
    Device(bool parityMethod);
    void listenData();
    void sendData(std::string msg);
    void recvData(bool *recv_buffer);
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