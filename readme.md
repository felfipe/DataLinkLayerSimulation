# DataLink Layer Simulator
Aplicação desenvolvida com o objetivo de simular a transmissão de dados através da camada de Enlace. A aplicação possui algoritmos de detecção de erros, como o de paridade e o CRC (Check Redudance Cyclic), além de algoritmos de enquadramento para lidar com o fim de uma transmissão. A aplicação também utiliza Threads para simular uma conexão duplex, na qual ambos os hosts se comunicam. 
### Compilar
Para compilar, basta digitar:
`make`

### Executar
Primeiramente, execute o server na porta dada como argumento:
`./server PORT`
Então execute o client na mesma porta:
`./client PORT`

### Versão GNU/Linux e Compilador

Compilador g++ (Debian 8.3.0-6) 8.3.0

Sistema Operacional Debian Buster (kernel 4.19.0-13-amd64)