#include <iomanip>
#include <fixed/fixed.h>
#include <pde_solvers/pde_solvers.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

#include <Windows.h>
#include <ctime>

using namespace std;

typedef composite_layer_t<profile_collection_t<2>> layer_template;

/// @brief Класс солвера, включающий в себя метод самоподобия и метод уголком
class PipeSolver
{
public:
    /// @brief Конструктор класса солвера
    /// @param pr Ссылка на предыдущий слой
    /// @param ne Ссылка на следующий слой
    /// @param dir Переменная, обозначающая направление течения
    PipeSolver(vector<double>& pr, vector<double>& ne, bool& dir)
        : prev{pr}, next{ne}, direction{dir}
    {}

    /// @brief Метод расчёта с помощбю свойства самоподобия
    /// @param left Левое граничное условие
    /// @param right Правое граничное условие
    void step(double left, double right)
    {
        if (direction)
        {
            next[0] = left;
            for (size_t j = 1; j < next.size(); j++)
                next[j] = prev[j - 1];
        }
        else 
        {
            next[next.size()-1] = right;
            for (size_t j = 0; j < next.size() - 1; j++)
                next[j] = prev[j + 1];
        }
        
    }

    /// @brief Метод расчёта уголком
    /// @param left Левое граничное условие
    /// @param right Правое граничное условие
    /// @param dt Шаг по времени для расчёт коэффициента k
    /// @param dx Шаг по времени для расчёт коэффициента k
    void step(double left, double right, double dt, double dx)
    {
        double k = dt / dx;
        if (direction)
        {
            next[0] = left;
            for (int j = 1; j < next.size(); j++)
                next[j] = (1 - k) * prev[j] + k * prev[j - 1];
        }
        else 
        {
            next[next.size() - 1] = right;
            for (size_t j = 0; j < next.size() - 1; j++)
                next[j] = (1 - k) * prev[j] + k * prev[j + 1];
        }
        
    }

private:
    vector<double>& prev; // Ссылка на предыдущий слой
    vector<double>& next; // ССылка на следующий слой
    bool& direction;
};

/// @brief Структура начальных условий
struct calc_data {
    double L;
    double speed;
    double dx, dt;
    double ro_left;
    double ro_right;
    double ro_init;
    double s_left;
    double s_right;
    double s_init;
    double k1;
    int T;
    int x_dots, time_dots;
    bool method, direction;
};

/// @brief Функция пользовательского ввода
/// @param initCondition Ссылка на структуру начальных условий
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

/// @brief Ввод значений начальных условий
/// @param iniStruct Ссылка на структуру начальных условий
void iniFun(calc_data& iniStruct) {
    iniStruct.speed = 10;
    iniStruct.L = 100;
    iniStruct.dx = 5;
    iniStruct.ro_left = 840;
    iniStruct.ro_right = 880;
    iniStruct.ro_init = 860;
    iniStruct.s_left = 0.60;
    iniStruct.s_right = 1.2;
    iniStruct.s_init = 0.90;
    iniStruct.T = 300;
    iniStruct.dt = iniStruct.dx / iniStruct.speed;
    iniStruct.k1 = iniStruct.dt / iniStruct.dx;
    iniStruct.time_dots = (int) (iniStruct.T / iniStruct.dt);
    iniStruct.x_dots = (int)(iniStruct.L / iniStruct.dx + 1);
    iniStruct.direction = false;

    cout << "Начальная плотность:        " << iniStruct.ro_init << endl;
    cout << "Плотность слева:            " << iniStruct.ro_left << endl;
    cout << "Плотность справа:           " << iniStruct.ro_right << endl;
    cout << "Начальное содержание серы:  " << iniStruct.s_init << endl;
    cout << "Содержание серы слева:      " << iniStruct.s_left << endl;
    cout << "Содержание серы справа:     " << iniStruct.s_right << endl;
    cout << "Длина трубопровода:         " << iniStruct.L << endl;
    cout << "Время:                      " << iniStruct.T << endl;
    cout << "Скорость:                   " << iniStruct.speed << endl;
    cout << "Шаг по координате:          " << iniStruct.dx << endl;
    cout << "Шаг по времени:             " << iniStruct.dt << endl;
    cout << "Количество точек T:         " << iniStruct.time_dots << endl;
    cout << "Количество точек X:         " << iniStruct.x_dots << endl;
    cout << "Коэффициент:                " << iniStruct.k1 << endl;
    cout << "Направление:                " << ((iniStruct.direction) ? ("Прямое\n") : ("Обратное\n"));
}

/// @brief 
/// @param strData Ссылка на структуру начальных условий
/// @param layer Ссылка на текущий слой для вывода
/// @param ti Шаг моделирования
/// @param fileName Имя файла для записи
void writeFun(calc_data& strData, custom_buffer_t<layer_template>& buff, int ti, string fileName = "res.csv") {
    ofstream my_file;
    size_t profCount = buff.current().vars.point_double.size();

    //(ti == 0) ? (my_file.open(fileName)) : (my_file.open(fileName, ios::app));
    if (ti == 0) 
    {
        my_file.open(fileName);
        my_file << "time,x,Density,Sulfur" << endl;
    }
    else
    {
        my_file.open(fileName, ios::app);
    }

    for (int i = 0; i < strData.x_dots; i++)
    {
        my_file << ti * strData.dt << "," << i * strData.dx;
        for (size_t p=0;p<profCount;p++)
            my_file << "," << buff.current().vars.point_double[p][i];

        my_file << endl;
    }

    my_file.close();
}

/// @brief Функция расчёта 
/// @param buff Ссылка на буфер
/// @param iniData Ссылка на структуру начальных условий
void characteristics(custom_buffer_t<layer_template>& buff, calc_data& iniData)
{
    writeFun(iniData, buff, 0);

    // Граничные условия
    double boundCond[2][2] { {iniData.ro_left, iniData.ro_right},
                           {iniData.s_left,  iniData.s_right} };

    
    for (int i = 1; i <= iniData.time_dots; i++)
    {
        buff.advance(1);

        // Расчёт по всем имеющимся профилям
        for (int p = 0; p < buff.current().vars.point_double.size(); p++)
        {

            PipeSolver solv(buff.previous().vars.point_double[p], buff.current().vars.point_double[p], iniData.direction);
            if (iniData.method) // 1 - с помощью свойства самоподобия, 0 - метод расчёта уголком
                solv.step(boundCond[p][0], boundCond[p][1]);
            else
                solv.step(boundCond[p][0], boundCond[p][1], iniData.dt, iniData.dx);
        }

        writeFun(iniData, buff, i);
    }
}



int main()
{
    //system("chcp 866");
    //setlocale(LC_ALL, "Russian");
    //#ifdef _WIN32
    //    std::wcout.imbue(std::locale("rus_rus.866"));
    //#endif

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
    custom_buffer_t<layer_template> buffer(2, initial_data.x_dots);
    buffer.current().vars.point_double[0] = vector<double>(initial_data.x_dots, initial_data.ro_init);
    buffer.current().vars.point_double[1] = vector<double>(initial_data.x_dots, initial_data.s_init);

    // Расчёт
    characteristics(buffer, initial_data);

    // Вывод затраченного времени
    printf("Затраченное время: %i ms\n", time_count);

    // Построение графика
    system("py charts.py");

    return 0;
}