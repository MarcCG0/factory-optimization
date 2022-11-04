#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

using extras = vector<int>;

struct Input {
    int C;
    int M;
    int K;
    vector<int> c_e, n_e;
    map<int, int> prod; // classe i quantitat a produir de la classe
    map<int, extras> upgr; // classe i vector binari que indica si una classe necesita una certa millora
};

Input read_input()
{
    Input i;
    // Obrim el fitxer i iterem sobre ell
    string line;
    ifstream myfile("example.txt");
    getline(myfile, line);

    // obtenim els tres naturals d'entrada
    istringstream iss(line);
    iss >> i.C >> i.M >> i.K;

    // llegim els M enters c_e
    getline(myfile, line);
    int capacity;
    while (iss >> capacity)
        i.c_e.push_back(capacity);

    // llegim els M enters n_e
    getline(myfile, line);
    int qty;
    while (iss >> qty)
        i.n_e.push_back(qty);

    // llegim K linies
    for (int i = 0; i < K; ++i) {
        getline(myfile, line);

        // llegim la classe i quantitat a produir
        int classe, qty_prod;
        iss >> classe << qty_prod;
        i.prod[classe] = qty_prod;

        // llegim el vector de millores
        int extra;
        extras e;
        for (int j = 0; j < M; ++j) {
            iss >> extra;
            e.push_back(extra);
        }
        i.upgr[classe] = e;
    }
    myfile.close();
    return i;
}

int main()
{
    Input i = read_input();
    const auto& [C, M, K, c_e, n_e, prod, upgr] = i;
}
