#include "../headers/device.hpp"

Device::Device(bool parityMethod, bool METHOD, int PORT) {
    this->id = 0;
    this->parityMethod = parityMethod;

    /* DEFINE FLAGS */
    this->flag = (bool *)calloc(8, sizeof(bool));
    this->ACK = (bool *)calloc(8, sizeof(bool));
    this->NACK = (bool *)calloc(8, sizeof(bool));
    this->flag[0] = 0;
    this->flag[1] = 1;
    this->flag[2] = 0;
    this->flag[3] = 0;
    this->flag[4] = 0;
    this->flag[5] = 0;
    this->flag[6] = 0;
    this->flag[7] = 0;

    /*
    this->ACK[0] = 1;
    this->ACK[1] = 1;
    this->ACK[2] = 1;
    this->ACK[3] = 1;
    this->ACK[4] = 1;
    this->ACK[5] = 1;
    this->ACK[6] = 1;
    this->ACK[7] = 1;

    this->NACK[0] = 1;
    this->NACK[1] = 0;
    this->NACK[2] = 1;
    this->NACK[3] = 1;
    this->NACK[4] = 1;
    this->NACK[5] = 1;
    this->NACK[6] = 1;
    this->NACK[7] = 0;
  */
    /* ***** */

    /* DEFINE BUFFERS */
    this->recv_buffer = (buffer *)calloc(1, sizeof(buffer));
    this->send_buffer = (buffer *)calloc(1, sizeof(buffer));
    recv_buffer->buffer_data = (bool *)calloc(BUFFER_LENGTH, sizeof(bool));
    send_buffer->buffer_data = (bool *)calloc(BUFFER_LENGTH, sizeof(bool));
    this->CRC = (bool *)calloc(CRC_SIZE, sizeof(bool));
    /* ***** */

    /* DEFINE SOCKET */
    this->socket = new socketHandler(PORT, METHOD);

    /* DEFINE THREADS */
    send_thread = std::thread(&Device::listenData, this);
    recv_thread = std::thread(&Device::recvData, this);
}

void Device::recvACK() {
    char ACK[1];
    this->recv_buffer->length = 8;
    bitToChar(this->recv_buffer, ACK);
}

/*
FUNÇÃO COM THREAD
Essa função constantemente lê um valor do teclado para enviar para outro dispotivo.
*/
void Device::listenData() {
    while (1) {
        std::string message;
        std::cout << "\033[1;32m[DEV]\033[0m Type your message: " << std::endl;

        std::cin >> message;
        sendData(message);
    }
    return;
}

/*
Dado uma mensagem, calcula CRC, bit de paridade, lida com a flag,
simula o meio físico e então envia.
Cada metodo utilizado aqui é explicado mais a fundo em sua definição.
*/
void Device::sendData(std::string message) {
    clearBuffer(send_buffer);
    byteToBit(message);
    this->parity = calculateParity(send_buffer->buffer_data, send_buffer->length);
    calculateCRC(send_buffer->buffer_data, send_buffer->length, this->CRC);
    insertStopFlag(send_buffer->buffer_data, send_buffer->length);
    insertCRC();
    insertParity();
    std::cout << "\033[1;31m";
    for (int i = 0; i < this->send_buffer->length; i++) {
        if (i == this->send_buffer->length - 42)
            std::cout << "\033[1;32m";
        if (i == this->send_buffer->length - 33)
            std::cout << "\033[1;33m";
        if (i == this->send_buffer->length - 1)
            std::cout << "\033[1;34m";
        if (i % 8 == 0)
            std::cout << " ";
        std::cout << this->send_buffer->buffer_data[i];
    }
    std::cout << "\033[0m" << std::endl;
    std::cout << "Data sent (physical interference): " << std::endl;
    PhysicalLayerSimulation();
    std::cout << "\033[1;31m";
    for (int i = 0; i < this->send_buffer->length; i++) {
        if (i == this->send_buffer->length - 42)
            std::cout << "\033[1;32m";
        if (i == this->send_buffer->length - 33)
            std::cout << "\033[1;33m";
        if (i == this->send_buffer->length - 1)
            std::cout << "\033[1;34m";
        if (i % 8 == 0)
            std::cout << " ";
        std::cout << this->send_buffer->buffer_data[i];
    }
    std::cout << "\033[0m" << std::endl;
    this->socket->sendSocket(this->send_buffer->buffer_data, this->send_buffer->length);
    return;
}
/* void Device::sendACK(bool is_ack) {
    clearBuffer(send_buffer);
    for (int i = 0; i < 8; i++) {
        if (is_ack)
            send_buffer->buffer_data[i] = ACK[i];
        else
            send_buffer->buffer_data[i] = NACK[i];
    }
    send_buffer->length += 8;
    this->parity = calculateParity(send_buffer->buffer_data, send_buffer->length);
    calculateCRC(send_buffer->buffer_data, send_buffer->length, this->CRC);
    insertStopFlag(send_buffer->buffer_data, send_buffer->length);
    insertCRC();
    insertParity();
    PhysicalLayerSimulation();
    return;
} */

/*
FUNÇÃO COM THREAD
A função aguarda a chegada dos dados pelo socket e trata os dados.
Primeiramente ela remove a flag, junto com os bits inseridos.
Então a função compara a paridade e o CRC.

*/
void Device::recvData() {
    while (1) {
        this->socket->recvSocket(recv_buffer->buffer_data, recv_buffer->length);
        std::cout << "DATA RECIVED:" << std::endl;
        std::cout << "\033[1;31m";  // RED
        for (int i = 0; i < this->recv_buffer->length; i++) {
            if (i == this->recv_buffer->length - 42)
                std::cout << "\033[1;32m";
            if (i == this->recv_buffer->length - 33)
                std::cout << "\033[1;33m";
            if (i == this->recv_buffer->length - 1)
                std::cout << "\033[1;34m";
            if (i % 8 == 0)
                std::cout << " ";
            std::cout << this->recv_buffer->buffer_data[i];
        }
        std::cout << "\033[0m" << std::endl;
        this->recv_buffer->length = 0;
        std::cout << std::endl;

        removeStopFlag(this->recv_buffer->buffer_data, this->recv_buffer->length);
        if (checkParity(this->recv_buffer->buffer_data, this->recv_buffer->length)) {
            removeParity();
            if (checkCRC(this->recv_buffer->buffer_data, this->recv_buffer->length)) {
                char data_char[BUFFER_LENGTH / 8];
                removeCRC();
                memset(data_char, '\0', BUFFER_LENGTH / 8);
                bitToChar(this->recv_buffer, data_char);
                std::cout << "PAYLOAD: " << data_char << std::endl;
            } else {
                std::cout << "\033[1;37m[ERROR] CRC IS NOT VALID\033[0m";
                // ERRO
            }
        } else {
            std::cout << "\033[1;37m[ERROR] PARITY IS NOT VALID\033[0m";
            // ERRO
        }
    }
    return;
}

/*
Limpa um buffer
*/
void Device::clearBuffer(buffer *buffer) {
    for (int i = 0; i < BUFFER_LENGTH; i++)
        buffer->buffer_data[i] = 0;
    buffer->length = 0;
    return;
}

/*
Função que insere a flag de stop no buffer.
Também verifica a ocorrência da flag nos dados e insere um 0 após.
Insere 1 no final da flag de stop, indicando a parada.
*/
void Device::insertStopFlag(bool *buffer, int &length) {
    int i = 0;
    int flagRelativeOffset = this->findFlag(&buffer[i], length - i);
    while (flagRelativeOffset != -1) {  //enquanto ainda tiverem flags que não são o fim real, inserimos o bit de verificação
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
    for (int i = 0; i < FLAG_SIZE; i++)  //inserindo a flag de fim real e seu bit de checagem 1
        buffer[length + i] = this->flag[i];
    length += FLAG_SIZE;

    buffer[length] = 1;
    length++;
}
void Device::removeStopFlag(bool *buffer, int &length) {
    bool bufferData[BUFFER_LENGTH];
    for (int i = 0; i < BUFFER_LENGTH; i++)  //partindo de cat bit de posição i procuramos uma flag
        bufferData[i] = 0;
    int bufferDataLength = 0;
    for (int i = 0; i < BUFFER_LENGTH; i++) {
        bool isFlag = true;
        for (int j = 0; j < 8; j++) {
            if (this->flag[j] != buffer[i + j]) {
                isFlag = false;
                break;
            }
        }
        if (isFlag) {  //encontrad verificamos se ela realmente é o fim ou se faz parte dos dados
            for (int j = 0; j < 8; j++) {
                bufferData[bufferDataLength] = buffer[i + j];
                bufferDataLength++;
            }
            i += 8;
            if (buffer[i]) {
                //é um fim
                length = bufferDataLength - 8;
                //Insere de volta no pacote o CRC E O BIT PARIDADE
                for (int j = 0; j < CRC_SIZE + 1; j++) {
                    bufferData[length + j] = buffer[i + 1 + j];
                }
                length += 33;
                for (int j = 0; j < length; j++)
                    buffer[j] = bufferData[j];
                break;
            }
        } else {
            bufferData[bufferDataLength] = buffer[i];
            bufferDataLength++;
        }
    }
    return;
}

/*
Função que recebe uma cadeia de bits e converte para uma cadeia de char
8 bits -> 1 char
*/
void Device::bitToChar(buffer *buffer, char *data_char) {
    for (int i = 0; i < buffer->length / 8; i++) {
        char dado = 0;
        for (int j = 8 * i; j < 8 * i + 8; j++) {
            dado += buffer->buffer_data[j];
            if (j % 8 == 7)
                continue;
            dado <<= 1;
        }
        data_char[i] = dado;
    }
    return;
}

void Device::removeCRC() {
    this->recv_buffer->length -= 32;
}

/*
Calcula a paridade e coompara com o último bit de um buffer
false - WRONG
true  - OK
*/
bool Device::checkParity(bool buffer[], int length) {
    bool calculated = calculateParity(buffer, length - 33);
    if (calculated == buffer[length - 1])
        return true;
    else
        return false;
}

//Este metodo recebe uma posição no buffer e contanto a partir da posição informada envia a posição da primeira flag encontrada
//caso não encontre nenhuma flag, retorna -1, para que ele funcione corretamente o length informado deve ser a distância entre a
//posição do buffer informada e seu fim.
int Device::findFlag(bool *buffer, int length) {
    for (int i = 0; i <= length - 8; i++) {
        //check if is flag
        bool isFlag = true;
        for (int j = 0; j < 8; j++) {  //verifica se contando a partir da posição i temos uma flag de fim
            if (this->flag[j] != buffer[i + j]) {
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
/*Converte uma String em um array de de bits já os inserindo no buffer*/
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

//Simulação da Camada Física, inserindo erros nos dados transmitidos
void Device::PhysicalLayerSimulation() {
    bool changedBuffer[BUFFER_LENGTH];
    for (int i = 0; i <= send_buffer->length; i++) {
        if ((rand() % 100000) / (float)1000 < ERROR_CHANCE) {
            //erro
            send_buffer->buffer_data[i] = !send_buffer->buffer_data[i];
        }
    }
}

/*Metodo que calcula o CRC de um conjunto de dados uma vez que seu tamanho é conhecido*/
void Device::calculateCRC(bool *buffer, int length, bool *CRC_p) {
    bool result[length + 32];  //array utilizado para operações de calculo do CRC
    //Startando array de calculo do CRC
    for (int i = 0; i < length + 32; i++) {
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
    for (int i = 0; i < 32; i++) {
        CRC_p[i] = result[length + i];
    }
}

//Metodo que pega o CRC calculado da memoria e o insere no nosso buffer de envio
void Device::insertCRC() {
    //Adicionando o CRC no BUffer da Mensagem
    for (int i = 0; i < CRC_SIZE; i++) {
        send_buffer->buffer_data[send_buffer->length + i] = this->CRC[i];
    }
    send_buffer->length += CRC_SIZE;
    //return &result[dataLength];
}
bool Device::checkCRC(bool *buffer, int length) {
    bool *receivedCRC = &buffer[length - 32];  //CRC que foi recebido
    bool CRC[32];                              //Variavel que armazena o CRC calculado
    calculateCRC(buffer, length - 32, CRC);    //Calcula o CRC
    bool response = true;                      //Variavel de retorno, verifica se os CRC bateram
    for (int i = 0; i < 32; i++) {             //Verificando CRC
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
bool Device::calculateParity(bool *buffer, int length) {
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
//Remove o bit de paridade do buffer de recepção
void Device::removeParity() {
    recv_buffer->buffer_data[recv_buffer->length] = 0;
    recv_buffer->length--;
    return;
}
//Insere o but de paridade previamente calculado no buffer de envio
void Device::insertParity() {
    send_buffer->buffer_data[send_buffer->length] = this->parity;
    send_buffer->length += 1;
    return;
}
//Retorna o conteudo do buffer
bool *Device::getBuffer() {
    return this->send_buffer->buffer_data;
}