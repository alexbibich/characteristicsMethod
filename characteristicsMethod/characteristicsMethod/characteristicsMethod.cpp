#include <iostream>
#include <fstream>
#include <vector>
#include<cstdlib>
#include <ctime>

using namespace std;

void characteristics()
{
    double u = 10, L = 100, dx = 5, ro_left = 840, ro_right = 860, curr_x;
    double dt = dx / u, k1 = 1 - dt / dx, k2 = dt / dx;
    int T = 300, time_dots = T / dt, x_dots = L / dx + 1;
    unsigned char ord = 0;
    string inpt;
    bool choice;

    vector<vector<double>> layers(2, vector<double>(x_dots, 860));

    setlocale(LC_ALL, "Russian");

    // Ввод
    while (1) {
        cout << "1. Свойство самоподобия\n2. Уголком\nВыберите метод: ";
        cin >> inpt;
        // Проверка ввода
        if (inpt.length() == 1 && (inpt == "1" || inpt == "2")) {
            cout << inpt << endl;
            choice = inpt == "1" ? true : false;
            break;
        } else
            cout << "Ошибка ввода!" << endl << endl;
    }

    cout << "Начальная плотность: " << ro_right << endl;
    cout << "Конечная плотность: " << ro_left << endl;
    cout << "Длина трубопровода:  " << L << endl;
    cout << "Время:               " << T << endl;
    cout << "Скорость:            " << u << endl;
    cout << "Шаг по координате:   " << dx << endl;
    cout << "Шаг по времени:      " << dt << endl;
    cout << "Количество точек T:  " << time_dots << endl;
    cout << "Количество точек X:  " << x_dots << endl;
    cout << "Коэффициент:         " << k2 << endl;
    
    // Файл для записи результатов
    ofstream my_file;
    my_file.open("res.csv");

    for (int i = 1; i <= time_dots; i++)
    {
        curr_x = 0;
        layers[ord][0] = ro_left;
        my_file << i * dt << ";" << curr_x << ";" << layers[ord][0] << endl;

        for (int j = 1; j < x_dots; j++)
        {
            curr_x += dx;
            if (choice)
                layers[ord][j] = layers[ord ^ 1][j - 1];
            else
                layers[ord][j] = k1 * layers[ord ^ 1][j] + k2 * layers[ord ^ 1][j - 1];
            
            my_file << i * dt << ";" << curr_x << ";" << layers[ord][j] << endl;
        }

        ord ^= 1;
    }

    my_file.close();
}

int main()
{
    int time_count = clock();

    characteristics();
    printf("Затраченное время: %i ms\n", time_count);

    //system("start charts.exe");
    system("py charts.py");

    return 0;
}

