#include <iomanip>
#include <fixed/fixed.h>
#include <pde_solvers/pde_solvers.h>

#include <iostream>
#include <fstream>
#include <vector>
#include<cstdlib>

#include <Windows.h>
#include <ctime>

using namespace std;

typedef composite_layer_t<profile_collection_t<1>, moc_solver<1>::specific_layer> single_var_moc_t;

// Структура начальных условий
struct calc_data {
    double L;
    double speed;
    double dx, dt;
    double ro_left;
    double ro_right;
    double k1;
    int T;
    int x_dots, time_dots;
    bool method;
};

// Функция пользовательского ввода
void selFun(calc_data& initCondition) {

    string inpt;

    while (1) {
        cout << "1. Свойство самоподобия\n2. Уголком\nВыберите метод: ";
        cin >> inpt;
        // Проверка ввода
        if (inpt.length() == 1 && (inpt == "1" || inpt == "2")) {
            cout << inpt << endl;
            initCondition.method = inpt == "1" ? true : false;
            break;
        }
        else
            cout << "Ошибка ввода!" << endl << endl;
    }
}

// Ввод значений начальных условий
void iniFun(calc_data& iniStruct) {
    iniStruct.speed = 10;
    iniStruct.L = 100;
    iniStruct.dx = 5;
    iniStruct.ro_left = 840;
    iniStruct.ro_right = 860;
    iniStruct.T = 300;
    iniStruct.dt = iniStruct.dx / iniStruct.speed;
    iniStruct.k1 = iniStruct.dt / iniStruct.dx;
    iniStruct.time_dots = (int) (iniStruct.T / iniStruct.dt);
    iniStruct.x_dots = (int)(iniStruct.L / iniStruct.dx + 1);

    cout << "Начальная плотность: " << iniStruct.ro_right << endl;
    cout << "Конечная плотность:  " << iniStruct.ro_left << endl;
    cout << "Длина трубопровода:  " << iniStruct.L << endl;
    cout << "Время:               " << iniStruct.T << endl;
    cout << "Скорость:            " << iniStruct.speed << endl;
    cout << "Шаг по координате:   " << iniStruct.dx << endl;
    cout << "Шаг по времени:      " << iniStruct.dt << endl;
    cout << "Количество точек T:  " << iniStruct.time_dots << endl;
    cout << "Количество точек X:  " << iniStruct.x_dots << endl;
    cout << "Коэффициент:         " << iniStruct.k1 << endl;
}

// Свойство самоподобия
void selfSim(vector<double>& prev, vector<double>& next, calc_data& iniData) {
    next[0] = iniData.ro_left;
    for (int j=1;j< iniData.x_dots;j++)
        next[j] = prev[j - 1];
}

// Метод уголком
void angleMethod(vector<double>& prev, vector<double>& next, calc_data& iniData) {
    next[0] = iniData.ro_left;
    for (int j = 1; j < iniData.x_dots; j++)
        next[j] = (1 - iniData.k1) * prev[j] + iniData.k1 * prev[j - 1];
}

// Функция записи в файл
void writeFun(calc_data& strData, vector<double>& layer, int ti, string fileName = "res.csv") {
    ofstream my_file;
    (ti == 0) ? (my_file.open(fileName)) : (my_file.open(fileName, ios::app));

    for (int i=0;i< strData.x_dots;i++)
        my_file << ti * strData.dt << ";" << i * strData.dx << ";" << layer[i] << endl;
    
    my_file.close();
}

// Функция вычисления
void characteristics(custom_buffer_t<single_var_moc_t>& buff, calc_data iniData)
{
    writeFun(iniData, buff.current().vars.point_double[0], 0);

    for (int i = 1; i <= iniData.time_dots; i++)
    {
        buff.advance(1);

        if (iniData.method)
            selfSim(buff.previous().vars.point_double[0], buff.current().vars.point_double[0], iniData);
        else
            angleMethod(buff.previous().vars.point_double[0], buff.current().vars.point_double[0], iniData);
        
        writeFun(iniData, buff.current().vars.point_double[0], i);
    }
}

int main()
{
    //system("chcp 866");
    //setlocale(LC_ALL, "Russian");
    //#ifdef _WIN32
    //    std::wcout.imbue(std::locale("rus_rus.866"));
    //#endif

    // Отсчёт времени работы программы
    int time_count = clock();

    // Установка кодировки консоли
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Структура для начальных условий
    calc_data initial_data;

    // Выбор метода 
    selFun(initial_data);

    // Ввод начальных условий
    iniFun(initial_data);

    // Буфер 
    custom_buffer_t<single_var_moc_t> buffer(2, initial_data.x_dots);
    buffer.current().vars.point_double[0] = vector<double>(initial_data.x_dots, initial_data.ro_right);

    // Расчёт
    characteristics(buffer, initial_data);

    // Вывод затраченного времени
    printf("Затраченное время: %i ms\n", time_count);

    // График
    system("py charts.py");

    return 0;
}