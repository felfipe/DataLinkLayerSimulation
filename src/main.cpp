#include <iostream>

#include "../headers/device.hpp"
int main() {
    Device *dev = new Device(ODD);
    dev->listenData();
    Device *dev2 = new Device(ODD);

    dev2->recvData(dev->getBuffer());
    return 0;
}