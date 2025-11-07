#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "libmath.h"

using namespace std;

void writeToFile(int figure[LENGTH][WIDTH]) {
    ofstream outFile("answer.log");
    if (outFile.is_open()) {
        cout << "Файл answer.log успешно создан!" << endl;
        
        for (int i = 0; i < LENGTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                outFile << figure[i][j] << " ";
            }
            outFile << endl;
        }
        cout << "Данные записаны в answer.log" << endl;
    } else {
        cerr << "Ошибка при открытии файла!" << endl;
    }
    outFile.close();
}

void calculate(int figure[LENGTH][WIDTH]) {
    cout << "Инвертирование фигуры:" << endl;
    for (int i = 0; i < LENGTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (figure[i][j] == 1) {
                figure[i][j] = 0;
            }
            else if (figure[i][j] == 0) {
                figure[i][j] = 1;
            }
            else {
                cerr << "Неверное значение: " << figure[i][j] << " в позиции [" << i << "][" << j << "]" << endl;
            }
            cout << figure[i][j] << " ";
        }
        cout << endl;
    }
}

void processFigure(int figure[LENGTH][WIDTH]) {
    cout << "Обработка фигуры..." << endl;
    calculate(figure);
    writeToFile(figure);
}