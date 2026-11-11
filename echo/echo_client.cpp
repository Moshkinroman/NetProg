#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void print_help(const char* prog_name) {
    cout << "Использование: " << prog_name << " [-h] [-s адрес_сервера] [-m сообщение]\n"
         << "Параметры:\n"
         << "  -s адрес    IP-адрес сервера echo (по умолчанию 172.16.40.1)\n"
         << "  -m текст    Сообщение для отправки (по умолчанию 'Hello')\n"
         << "  -h          Показать эту справку\n";
}

int main(int argc, char* argv[]) {
    
    const char* server_ip = "172.16.40.1";
    int port = 7;  
    const char* message = "Hello";

    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_help(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            server_ip = argv[++i];
        }
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            message = argv[++i];
        }
    }

    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Ошибка создания сокета!" << endl;
        return 1;
    }

    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        cerr << "Неверный IP-адрес: " << server_ip << endl;
        close(sock);
        return 1;
    }

    
    int rc = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (rc == -1) {
        cerr << "Ошибка подключения к " << server_ip << ":" << port << endl;
        close(sock);
        return 1;
    }

    cout << "Подключено к " << server_ip << ":" << port << endl;

    
    int sent = send(sock, message, strlen(message), 0);
    if (sent == -1) {
        cerr << "Ошибка отправки сообщения!" << endl;
        close(sock);
        return 1;
    }

    cout << "Отправлено: " << message << endl;

   
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (received > 0) {
        buffer[received] = '\0';
        cout << "Получено: " << buffer << endl;

        // Проверяем, совпадает ли отправленное и полученное
        if (strcmp(message, buffer) == 0) {
            cout << "Тест пройден: сообщения совпадают!" << endl;
        }
        else {
            cout << "Внимание: сообщения различаются!" << endl;
        }
    }
    else {
        cerr << "Не удалось получить ответ от сервера" << endl;
    }

   
    close(sock);

    return 0;
}
