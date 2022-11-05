#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

using upgrades = vector<int>;

struct Input {
    int C;
    int M;
    int K;
    vector<int> c_e;
    vector<int> n_e;
    vector<int> prod; // classe i quantitat a produir de la classe
    vector<vector<int>> upgr; // classe i vector binari que indica si una classe necesita una certa millora
};

Input read_input(const string& a)
{
    // Obrim el fitxer i iterem sobre ell
    string line;
    ifstream myfile(a);

    Input i;

    for (int u = 0; u < 4; ++u) {
        if (u < 3) {
            getline(myfile, line);
            istringstream iss(line);

            if (u == 0) {
                // llegim els tres naturals d'entrada
                iss >> i.C >> i.M >> i.K;
            }

            if (u == 1) {
                // llegim els M enters c_e
                int capacity;
                while (iss >> capacity) {
                    i.c_e.push_back(capacity);
                }
            }

            if (u == 2) {
                // llegim els M enters n_e
                int qty;
                while (iss >> qty) {
                    i.n_e.push_back(qty);
                }
            }
        }
        if (u == 3) {
            // llegim K linies
            while (getline(myfile, line)) {
                istringstream iss1(line);

                // llegim la classe i quantitat a produir
                int classe,
                    qty_prod;
                iss1 >> classe >> qty_prod;
                i.prod.push_back(qty_prod);

                // llegim el vector de millores
                int upgrade;
                upgrades up;
                for (int j = 0; j < i.M; ++j) {
                    iss1 >> upgrade;
                    up.push_back(upgrade);
                }
                i.upgr.push_back(up);
            }
        }
    }
    return i;
}

int main(int argc, char** argv)
{
    string f_i = argv[1];
    string f_o = argv[2];
    Input i = read_input(f_i);
    const auto& [C, M, K, c_e, n_e, prod, upgr] = i;
}
