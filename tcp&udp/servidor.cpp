#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define PORT_TCP 8888
#define PORT_UDP 9999

using namespace std;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Função para receber um arquivo via TCP
void receiveFileTCP(const char *filename) {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Erro ao abrir o socket TCP");

    // Configuração do endereço do servidor
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_TCP);
    cout << "Escutando no IP " << inet_ntoa(serv_addr.sin_addr) << " e porta " << ntohs(serv_addr.sin_port) << " (TCP)" << endl;


    // Ligação do socket ao endereço do servidor
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Erro ao fazer bind do socket TCP");

    // Espera por conexões
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("Erro ao aceitar conexão TCP");

    // Abertura do arquivo para escrita
    ofstream file(filename, ios::out | ios::binary);
    if (!file) {
        cerr << "Erro ao abrir o arquivo\n";
        exit(1);
    }

    // Recebimento do arquivo
    ssize_t bytesReceived;
    while ((bytesReceived = recv(newsockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        file.write(buffer, bytesReceived);
    }

    // Fechamento do arquivo e do socket
    file.close();
    close(newsockfd);
    close(sockfd);
}

// Função para receber um arquivo via UDP
void receiveFileUDP(const char *filename) {
    int sockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Erro ao abrir o socket UDP");

    // Configuração do endereço do servidor
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_UDP);

    // Ligação do socket ao endereço do servidor
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Erro ao fazer bind do socket UDP");

    // Recebimento do arquivo
    cout << "Recebendo arquivo\n";
    ofstream file(filename, ios::out | ios::binary);
    if (!file) {
        cerr << "Erro ao abrir o arquivo\n";
        exit(1);
    }

    clilen = sizeof(cli_addr);
    ssize_t bytesReceived;
    while ((bytesReceived = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &clilen)) > 0) {
        file.write(buffer, bytesReceived);
    }

    // Fechamento do arquivo e do socket
    file.close();
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <tcp|udp> <nome_arquivo>\n";
        return 1;
    }

    string protocol(argv[1]);
    string filename(argv[2]);

    if (protocol == "tcp") {
        receiveFileTCP(filename.c_str());
    } else if (protocol == "udp") {
        receiveFileUDP(filename.c_str());
    } else {
        cerr << "Protocolo inválido. Use 'tcp' ou 'udp'\n";
        return 1;
    }

    cout << "Arquivo recebido com sucesso via " << protocol << endl;

    return 0;
}
