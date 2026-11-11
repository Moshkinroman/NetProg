#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void print_help(const char* prog_name) {
    cout << "Использование: " << prog_name << " [-h] [-s адрес_сервера]\n"
         << "Параметры:\n"
         << "  -s адрес  IP-адрес сервера daytime (по умолчанию 172.16.40.1)\n"
         << "  -h        Показать эту справку\n";
}

int main(int argc, char* argv[]) {
    
    const char* server_ip = "172.16.40.1";
    int port = 13;  // Порт службы daytime

    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_help(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            server_ip = argv[++i];
        }
    }

    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        cerr << "Ошибка создания сокета!" << endl;
        return 1;
    }

  
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Проверка корректности IP-адреса
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        cerr << "Неверный IP-адрес: " << server_ip << endl;
        close(sock);
        return 1;
    }

   
    const char* request = "GET TIME";
    int sent = sendto(sock, request, strlen(request), 0,
                      (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (sent == -1) {
        cerr << "Ошибка отправки данных!" << endl;
        close(sock);
        return 1;
    }

    cout << "Запрос отправлен на " << server_ip << ":" << port << endl;

    // Получаем ответ от сервера
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                            (struct sockaddr*)&from_addr, &from_len);

    if (received > 0) {
        buffer[received] = '\0';
        cout << "Получен ответ: " << buffer << endl;
    }
    else {
        cerr << "Не удалось получить ответ от сервера (возможно, таймаут)" << endl;
    }

 
    close(sock);

    return 0;
}
