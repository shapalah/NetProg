#include <iostream>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <csignal> 

volatile sig_atomic_t running = true; // Флаг для завершения сервера

void signal_handler(int signum) {
    running = false;
}

int main() {
    
    std::signal(SIGINT, signal_handler);

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("Ошибка создания сокета");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1228);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        close(s);
        return 1;
    }

    std::cout << "Сервер запущен на порту 1228" << std::endl;

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t slen = sizeof(client_addr);
        char buffer[256];

        ssize_t recv_bytes = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &slen);
        if (recv_bytes < 0) {
            perror("Ошибка получения данных");
            continue;
        }
        buffer[recv_bytes] = '\0';

        std::cout << "Получено сообщение от " << inet_ntoa(client_addr.sin_addr) << ": " << buffer << std::endl;

        time_t current_time = time(nullptr);
        std::string datetime = ctime(&current_time);

        ssize_t sent_bytes = sendto(s, datetime.c_str(), datetime.length(), 0, (struct sockaddr *)&client_addr, slen);
        if (sent_bytes < 0) {
            perror("Ошибка отправки данных");
            continue;
        }

        std::cout << "Отправлено время: " << datetime;
    }

    std::cout << "Сервер остановлен." << std::endl;
    close(s);
    return 0;
}
