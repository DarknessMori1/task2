#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 1024;

string readFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return "";
    }
    
    string content, line;
    while (getline(inFile, line)) {
        content += line + "\n";
    }
    inFile.close();
    
    if (content.empty()) {
        cerr << "Файл " << filename << " пустой" << endl;
        return "";
    }
    
    return content;
}

int main() {
    cout << "Клиент запущен (ping - pong)" << endl;

    string task = readFromFile("task.txt");
    if (task.empty()) {
        cerr << "Не удалось прочитать задание" << endl;
        return 1;
    }

    cout << "Задание из файла:" << endl << task << endl;

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Ошибка создания сокета" << endl;
        return 1;
    }
    cout << "Сокет создан" << endl;

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    cout << "Подключение к серверу " << SERVER_IP << ":" << SERVER_PORT << endl;
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Ошибка подключения к серверу" << endl;
        close(clientSocket);
        return 2;
    }
    cout << "Подключение установлено" << endl;

    // Отправляем задание серверу
    cout << "Отправка задания серверу..." << endl;
    int bytesSent = send(clientSocket, task.c_str(), task.length(), 0);
    if (bytesSent <= 0) {
        cerr << "Ошибка отправки данных" << endl;
        close(clientSocket);
        return 3;
    }
    cout << "Отправлено " << bytesSent << " байт" << endl;
    
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesReceived > 0) {
        cout << "Получен ответ от сервера:" << endl << buffer << endl;
    } else {
        cerr << "Ошибка при получении ответа от сервера" << endl;
    }

    cout << "Завершение работы..." << endl;
    send(clientSocket, "exit", 4, 0);
    close(clientSocket);
    cout << "Клиент завершил работу" << endl;
    return 0;
}