#include <iostream>

#include "../headers/device.hpp"
int main(int argc, char* argv[]) {
    srand(time(NULL));
    Device* dev = new Device(EVEN, SERVER, atoi(argv[1]));
    dev->recv_thread.join();
    dev->send_thread.join();

    return 0;
}