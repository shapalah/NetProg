#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <system_error>


#define PORT 1228 // Одинаковый порт с сервером
#define SERVER_HOSTNAME "127.0.0.1"

int main() {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(SERVER_HOSTNAME, std::to_string(PORT).c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == -1) {
        perror("socket error");
        freeaddrinfo(res);
        return 2;
    }

    const char* msg = "hello";
    ssize_t sent_bytes = sendto(s, msg, strlen(msg), 0, res->ai_addr, res->ai_addrlen);
    if (sent_bytes == -1) {
        perror("sendto error");
        close(s);
        freeaddrinfo(res);
        return 3;
    }

    char buf[512];
    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len = sizeof(recv_addr);
    ssize_t recv_bytes = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&recv_addr, &recv_addr_len);
    if (recv_bytes == -1) {
        perror("recvfrom error");
        close(s);
        freeaddrinfo(res);
        return 4;
    }
    buf[recv_bytes] = '\0';

    if (recv_addr_len != sizeof(recv_addr)) {
        std::cerr << "recvfrom(): unexpected address size" << std::endl;
        close(s);
        freeaddrinfo(res);
        return 5;
    }

    std::cout << "Ответ от сервера (" << inet_ntoa(recv_addr.sin_addr) << "): " << buf << std::endl;

    close(s);
    freeaddrinfo(res);

    return 0;
}
