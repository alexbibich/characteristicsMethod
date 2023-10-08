#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void characteristics()
{
    double u = 10, L = 100, dx = 10, ro_left =840, curr_x;
    int time_dots = 300 / (dx / u), x_dots = L / dx + 1;
    double dt = dx / u, k1 = (-u * dt + dx) / dx, k2 = u * dt / dx;
    unsigned int ord = 0;

    ofstream my_file;
    my_file.open("res.csv");

    vector<vector<double>> layers(2, vector<double>(x_dots, 860));


    for (int i = 0; i < time_dots; i++)
    {
        curr_x = 0;
        layers[ord][0] = ro_left;
        my_file << curr_x << ";" << layers[ord][0] << endl;
        cout << curr_x << "--" << layers[ord][0] << endl;

        for (int j = 1; j < x_dots; j++)
        {
            curr_x += dx;
            layers[ord][j] = k1 * layers[ord^1][j] + k2 * layers[ord^1][j-1];
            my_file << curr_x << ";" << layers[ord][j] << endl;
            cout << curr_x << "--" << layers[ord][j] << endl;
        }

        //ord = !ord;
        ord ^= 1;
    }

    my_file.close();
}

int main()
{
    characteristics();
    cout << "Hello World!\n";
}

