#ifndef __DEVICE
#define __DEVICE
#include <iostream>
#define EVEN 0  // 0 for pair parity check, 1 for odd parity check
#define ODD 1
class Device {
   private:
    bool parityMethod;
    int id;
    void byteToBit(std::string &msg, bool *&data, int &length);
    int countBits(bool *data, int length);

   public:
    Device(bool parityMethod);
    void listenData();
    void sendData(std::string msg);
    void recvData(std::string msg);
    void PhysicalLayerSimulation(bool buffer[]);
    void senderDataLinkLayer(bool data[]);
    void receiverDataLinkLayer(bool data[]);
    void checkCRC(bool buffer[]);
    void calculateCRC(bool buffer[], int dataLength);
    int checkParity(bool buffer[], int length);
    void insertParity(bool buffer[], int length);
};

#endif