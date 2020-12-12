#include <iostream>

#include "../headers/device.hpp"
int main() {
    Device *dev = new Device(EVEN);
    dev->listenData();
    return 0;
}