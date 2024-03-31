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

// Função para enviar um arquivo via TCP
void sendFileTCP(const char *filename, const char *ip_addr) {
    int sockfd, connfd;
    struct sockaddr_in serv_addr;

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Erro ao abrir o socket TCP");

    // Configuração do endereço do servidor
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr.sin_port = htons(PORT_TCP);

    // Conexão ao servidor
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Erro ao conectar-se ao servidor TCP");

    // Abertura do arquivo para leitura
    ifstream file(filename, ios::in | ios::binary);
    if (!file) {
        cerr << "Erro ao abrir o arquivo\n";
        exit(1);
    }

    // Envio do arquivo
    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        if (send(sockfd, buffer, file.gcount(), 0) < 0) {
            cerr << "Erro ao enviar dados pelo socket\n";
            break;
        }
    }

    // Fechamento do arquivo e do socket
    file.close();
    close(sockfd);
}

// Função para enviar um arquivo via UDP
void sendFileUDP(const char *filename, const char *ip_addr) {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    cout << "Socket criado\n"
    if (sockfd < 0)
        error("Erro ao abrir o socket UDP");

    // Configuração do endereço do servidor
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr.sin_port = htons(PORT_UDP);
    cout << "Configuração completa\n"

    // Abertura do arquivo para leitura
    ifstream file(filename, ios::in | ios::binary);
    cout << "Arquivo aberto com sucesso\n"
    if (!file) {
        cerr << "Erro ao abrir o arquivo\n";
        exit(1);
    }

    // Envio do arquivo
    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        if (sendto(sockfd, buffer, file.gcount(), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            cerr << "Erro ao enviar dados pelo socket\n";
            break;
        }
    }

    // Fechamento do arquivo e do socket
    file.close();
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <tcp|udp> <nome_arquivo> <ip_destino>\n";
        return 1;
    }

    string protocol(argv[1]);
    string filename(argv[2]);
    string ip_addr(argv[3]);

    if (protocol == "tcp") {
        sendFileTCP(filename.c_str(), ip_addr.c_str());
    } else if (protocol == "udp") {
        sendFileUDP(filename.c_str(), ip_addr.c_str());
    } else {
        cerr << "Protocolo inválido. Use 'tcp' ou 'udp'\n";
        return 1;
    }

    cout << "Arquivo enviado com sucesso via " << protocol << endl;

    return 0;
}