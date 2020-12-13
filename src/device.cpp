#include "../headers/device.hpp"

Device::Device(bool parityMethod) {
    this->id = 0;
    this->parityMethod = parityMethod;
    this->recv_buffer = (buffer *)calloc(1, sizeof(buffer));
    this->send_buffer = (buffer *)calloc(1, sizeof(buffer));
    recv_buffer->buffer_data = (bool *)calloc(BUFFER_LENGTH, sizeof(bool));
    send_buffer->buffer_data = (bool *)calloc(BUFFER_LENGTH, sizeof(bool));
    return;
}
void Device::listenData() {
    std::string message;
    std::cout << "[DEV " << this->id << " LISTENER] Type your message: " << std::endl;

    std::cin >> message;

    sendData(message);
}

void Device::sendData(std::string message) {
    clearBuffer(send_buffer);
    byteToBit(message);
    insertCRC();
    insertParity();
    for (int i = 0; i < send_buffer->length; i++)
        std::cout << send_buffer->buffer_data[i];
    return;
}
void Device::clearBuffer(buffer *buffer) {
    for (int i = 0; i < 5; i++)
        buffer->buffer_data[i] = 0;
    buffer->length = 0;
    return;
}
void Device::insertStopFlag(bool *buffer, int &length) {
    int i = 0;
    int flagRelativeOffset = this->findFlag(&buffer[i], length - i);
    while (flagRelativeOffset != -1) {
        int flagOffset = i + flagRelativeOffset;
        flagOffset += 8;
        bool aux = buffer[flagOffset];
        bool aux2 = buffer[flagOffset + 1];
        buffer[flagOffset] = false;
        buffer[flagOffset + 1] = aux;
        aux = aux2;
        for (int k = 0; k + 2 + flagOffset <= length; k++) {
            aux2 = buffer[flagOffset + k + 2];
            buffer[flagOffset + k + 2] = aux;
            aux = aux2;
        }
        length += 1;
        i += flagRelativeOffset + 1;
        flagRelativeOffset = this->findFlag(&buffer[i], length - i);
    }
}
void Device::removeStopFlag(bool *buffer, int &length) {
    bool *bufferData = (bool *)calloc(BUFFER_LENGTH, sizeof(bool));
    int bufferDataLength = 0;
    for (int i = 0; i < BUFFER_LENGTH; i++) {
        bool isFlag = true;
        for (int j = 0; j < 8; j++) {
            if (this->flag[i] != buffer[i + j]) {
                isFlag = false;
                break;
            }
        }
        if (isFlag) {
            for (int j = 0; j < 8; j++) {
                bufferData[bufferDataLength] = buffer[i + j];
                bufferDataLength++;
                i++;
            }
            if (buffer[i + 1]) {
                //é um fim
                buffer = bufferData;
                length = bufferDataLength - 8;
                break;
            } else {
                i++;
            }
        } else {
            bufferData[bufferDataLength] = buffer[i];
            bufferDataLength++;
        }
    }
}

//Este metodo recebe uma posição no buffer e contanto a partir da posição informada envia a posição da primeira flag encontrada
//caso não encontre nenhuma flag, retorna -1, para que ele funcione corretamente o length informado deve ser a distância entre a
//posição do buffer informada e seu fim.
int Device::findFlag(bool *buffer, int length) {
    for (int i = 0; i <= length - 8; i++) {
        //check if is flag
        bool isFlag = true;
        for (int j = 0; j < 8; j++) {
            if (this->flag[i] != buffer[i + j]) {
                isFlag = false;
                break;
            }
        }
        if (isFlag) {
            return i;
        }
    }
    return -1;
}

void Device::byteToBit(std::string &msg) {
    int dataLength = msg.length();
    send_buffer->length = dataLength * CHAR_LENGTH;
    for (int i = 0; i < dataLength; i++) {
        char word;
        word = msg.at(i);
        for (int j = (i + 1) * CHAR_LENGTH - 1; j >= i * CHAR_LENGTH; j--) {
            send_buffer->buffer_data[j] = word & 0x01;
            word >>= 1;
        }
    }
}
void Device::PhysicalLayerSimulation() {
    int errorChance = ERROR_CHANCE;
    bool changedBuffer[BUFFER_LENGTH];
    for (int i = 0; i <= send_buffer->length; i++) {
        if (rand() % 100 < ERROR_CHANCE) {
            //erro
            send_buffer->buffer_data[i] = !send_buffer->buffer_data[i];
        }
    }
}
void Device::calculateCRC(bool *buffer, int length, bool *&CRC) {
    bool result[length + 32];  //array utilizado para operações de calculo do CRC
    //Startando array de calculo do CRC
    for (int i = 0; i <= length + 32; i++) {
        if (i < length) {
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
    for (int j = 0; j < length; j++) {
        if (result[j]) {
            for (int i = 0; i + j < length + 32; i++) {
                if (i <= 32) {
                    currentBit = result[i + j] != poli[i];
                } else {
                    currentBit = result[i + j];
                }
                result[i + j] = currentBit;
            }
        }
    }
    CRC = (bool *)calloc(32, sizeof(bool));
    for (int i = 0; i < 32; i++) {
        CRC[i] = result[length + i];
    }
}
void Device::insertCRC() {
    bool *CRC;
    calculateCRC(send_buffer->buffer_data, send_buffer->length, CRC);
    //Adicionando o CRC no BUffer da Mensagem
    for (int i = 0; i < 32; i++) {
        send_buffer->buffer_data[send_buffer->length + i] = CRC[i];
        std::cout << CRC[i];
    }
    send_buffer->length += 32;
    //return &result[dataLength];
}
bool Device::checkCRC(bool *buffer, int length) {
    bool *receivedCRC = &buffer[length];  //CRC que foi recebido
    bool *CRC;                            //Variavel que armazena o CRC calculado
    calculateCRC(buffer, length, CRC);    //Calcula o CRC
    bool response = true;                 //Variavel de retorno, verifica se os CRC bateram
    for (int i = 0; i < 32; i++) {        //Verificando CRC
        if (receivedCRC[i] != CRC[i]) {
            response = false;
        }
    }
    return response;
}
/*
check Parity
0 - OK
1 - WRONG
*/
bool Device::checkParity(bool *buffer, int length) {
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
    return count;
}

void Device::insertParity() {
    int parityBit = checkParity(send_buffer->buffer_data, send_buffer->length);
    send_buffer->length += 1;
    send_buffer->buffer_data[send_buffer->length] = parityBit;
    return;
}