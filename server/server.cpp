#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include "libmath.h"

using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

bool parseTaskData(const string& taskData, int figure[LENGTH][WIDTH]) {
    stringstream ss(taskData);
    string line;
    
    getline(ss, line);
    
    for (int i = 0; i < LENGTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (!(ss >> figure[i][j])) {
                cerr << "Ошибка парсинга данных в позиции [" << i << "][" << j << "]" << endl;
                return false;
            }
        }
    }
    return true;
}

string figureToString(int figure[LENGTH][WIDTH]) {
    stringstream result;
    result << "Обработанная фигура:\n";
    for (int i = 0; i < LENGTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            result << figure[i][j] << " ";
        }
        result << "\n";
    }
    return result.str();
}

int main() {
    cout << "сервер" << endl;
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "ошибка создания сокета!" << endl;
        return 1;
    }
    cout << "сокет создан!" << endl;

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "ошибка bind!" << endl;
        close(serverSocket);
        return 2;
    }
    cout << "Сокет привязан!!!" << endl;

    if (listen(serverSocket, 4) < 0) {
        cerr << "ошибка listen!" << endl;
        close(serverSocket);
        return 3;
    }
    cout << "listen на порту " << PORT << endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {
            cerr << "Ошибка accept" << endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        cout << "Клиент подключён: " << clientIP << endl;
        
        char buffer[BUFFER_SIZE];
        bool clientConnected = true;
        
        while (clientConnected) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) {
                cout << "Клиент отключился" << endl;
                clientConnected = false;
                break;
            }
            
            string message(buffer, bytesReceived);
            cout << "Получено: " << message << endl;
       
            if (message == "exit") {
                cout << "Клиент запросил выход" << endl;
                clientConnected = false;
                break;
            }
            
            string response;
            if (message.find('1') != string::npos || message.find('0') != string::npos) {
                int figure[LENGTH][WIDTH] = {0}; // Инициализируем нулями
                
                if (parseTaskData(message, figure)) {
                    cout << "Исходная фигура:" << endl;
                    for (int i = 0; i < LENGTH; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                            cout << figure[i][j] << " ";
                        }
                        cout << endl;
                    }
                    
                    processFigure(figure);
                    
                    response = figureToString(figure);
                } else {
                    response = "Ошибка: неверный формат данных";
                }
            } else {
                response = "DA";
            }

            cout << "Отправка response..." << endl;
            send(clientSocket, response.c_str(), response.length(), 0);
        }

        close(clientSocket);
        cout << "Соединение с клиентом закрыто" << endl;
    }

    close(serverSocket);
    cout << "Завершено" << endl;
    return 0;
}