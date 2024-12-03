#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <netdb.h>
using namespace std;

#define PORT 7
#define SERVER_IP "127.0.0.1" 

int main() {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(SERVER_IP, std::to_string(PORT).c_str(), &hints, &res);
    if (status != 0) {
        cerr << "getaddrinfo ошибка: " << gai_strerror(status) << endl;
        return 1;
    }

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == -1) {
        perror("Ошибка сокета");
        freeaddrinfo(res);
        return 2;
    }

    if (connect(s, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Ошибка соединения");
        close(s);
        freeaddrinfo(res);
        return 3;
    }

    string message;
    cout << "Введите сообщение: ";
    getline(cin, message);

    ssize_t sent_bytes = send(s, message.c_str(), message.length(), 0);
    if (sent_bytes == -1) {
        perror("Ошибка");
        close(s);
        freeaddrinfo(res);
        return 4;
    }

    char buf[1024];
    ssize_t recv_bytes = recv(s, buf, sizeof(buf) - 1, 0);
    if (recv_bytes == -1) {
        perror("ошибка");
        close(s);
        freeaddrinfo(res);
        return 5;
    }
    buf[recv_bytes] = '\0';

    cout << "Ответ от сервера: " << buf << endl;

    close(s);
    freeaddrinfo(res);

    return 0;
}
