#include "../headers/device.hpp"
#define CRCPOLI 63
#define CHAR_LENGTH 8
#define BUFFER_LENGTH 2080
#define ERROR_CHANCHE 80
Device::Device(bool parityMethod) {
    this->parityMethod = parityMethod;
    return;
}
void Device::listenData() {
    std::string message;
    std::cout << "[DEV " << this->id << " LISTENER] Type your message: " << std::endl;
    std::cin >> message;
    sendData(message);
}

void Device::sendData(std::string message) {
    bool *data;
    int length;
    byteToBit(message, data, length);
    insertParity(data, length);
    for (int i = 0; i < length + 1; i++) {
        std::cout << data[i];
    }
    return;
}

void Device::byteToBit(std::string &msg, bool *&data, int &length) {
    int dataLength = msg.length();
    length = dataLength * CHAR_LENGTH;
    data = new bool(length);
    for (int i = 0; i < dataLength; i++) {
        char word;
        word = msg.at(i);
        for (int j = (i + 1) * CHAR_LENGTH - 1; j >= i * CHAR_LENGTH; j--) {
            data[j] = word & 0x01;
            word >>= 1;
        }
    }
}
void Device::PhysicalLayerSimulation(bool buffer[]) {
    int errorChance = ERROR_CHANCE;
    bool changedBuffer[BUFFER_LENGTH];
    for (int i = 0;) {
    }
}
void Device::calculateCRC(bool *buffer, int dataLength) {
    bool result[dataLength + 32];  //array utilizado para operações de calculo do CRC
    //Startando array de calculo do CRC
    for (int i = 0; i <= dataLength + 32; i++) {
        if (i < dataLength) {
            result[i] = buffer[i];
        } else {
            result[i] = false;
        }
    }
    //variaveis para calculo de CRC
    bool currentBit = true;
    bool poli[33];
    //Definimos nosso polinomio como tendo todos os seus coeficientes iguais a 1 sendo assim o starmos da seguinte maneira
    for (int i = 0; i < 33; i++) {
        poli[i] = true;
    }
    //Calculo do CRC
    for (int j = 0; j < dataLength; j++) {
        if (result[j]) {
            for (int i = 0; i + j < dataLength + 32; i++) {
                if (i <= 32) {
                    currentBit = result[i + j] != poli[i];
                } else {
                    currentBit = result[i + j];
                }
                result[i + j] = currentBit;
            }
        }
    }
    //Adicionando o CRC no BUffer da Mensagem
    for (int i = 0; i < 32; i++) {
        buffer[dataLength] = result[dataLength + i];
    }
    //return &result[dataLength];
}

/*
check Parity
0 - OK
1 - WRONG
*/
int Device::checkParity(bool *buffer, int length) {
    int bits = countBits(buffer, length);
    bool parityBit = bits % 2;
    if (this->parityMethod == EVEN)  // EVEN OR ODD METHOD PARITY?
        return parityBit;
    else
        return parityBit ? 0 : 1;
}

/*
Count the number of 1's in a bool vector
return number of 1's 
*/
int Device::countBits(bool *data, int length) {
    int count = 0;
    for (int i = 0; i < length; i++) {
        count += data[i];
    }
    std::cout << count << std::endl;
    return count;
}

void Device::insertParity(bool *buffer, int length) {
    int parityBit = checkParity(buffer, length);
    buffer[length] = parityBit;
    return;
}