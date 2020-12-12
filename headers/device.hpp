#ifndef __DEVICE
#define __DEVICE
#include <iostream>
#define EVEN 0  // 0 for pair parity check, 1 for odd parity check
#define ODD 1
struct buffer {
    bool *buffer;
    int length;
};
typedef struct buffer buffer;
class Device {
   private:
    bool parityMethod;
    int id;
    void byteToBit(std::string &msg, bool *&data, int &length);
    int countBits(bool *buffer, int length);
    void calculateCRC(bool *buffer, int length, bool *&CRC);

   public:
    Device(bool parityMethod);
    void listenData();
    void sendData(std::string msg);
    void recvData(std::string msg);
    void PhysicalLayerSimulation(bool buffer[]);
    void senderDataLinkLayer(bool data[]);
    void receiverDataLinkLayer(bool data[]);
    bool checkCRC(bool buffer[], int length);
    void insertCRC(bool buffer[], int &dataLength);
    bool checkParity(bool buffer[], int length);
    void insertParity(bool buffer[], int &length);
    void insertStopFlag(bool buffer[], int &length);
};

#endif