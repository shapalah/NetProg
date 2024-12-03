#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
using namespace std;

#define PORT 7    
#define BACKLOG 10 

volatile sig_atomic_t running = true;

void signal_handler(int signum) {
    running = false;
}

int main() {
    signal(SIGINT, signal_handler);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        close(server_fd);
        return 1;
    }

    cout << "Сервер запущен на порту " << PORT << endl;

    while (running) {
        sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }

        char buffer[1024] = {0};
        ssize_t valread = read(new_socket, buffer, 1024);
        if (valread < 0) {
            perror("read failed");
        } else {
            cout << "Получено сообщение: " << buffer << endl;
            send(new_socket, buffer, strlen(buffer), 0);
            cout << "Отправлен ответ." << endl;
        }

        close(new_socket);
    }

    cout << "Сервер остановлен." << endl;
    close(server_fd);

    return 0;
}
