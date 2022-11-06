#include <climits>
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
        // a la columna j-essima hi ha els cotxes que necessiten la millora j-essima
};

struct Sol {
    int penalty = INT_MAX;
    vector<int> permutation;
    vector<vector<int>> req;
};

Input read_input(const string& a, Sol& S)
{
    // Obrim el fitxer i iterem sobre ell
    string line;
    ifstream myfile(a);

    Input I;

    for (int u = 0; u < 4; ++u) {
        if (u < 3) {
            getline(myfile, line);
            istringstream iss(line);

            if (u == 0) {
                // llegim els tres naturals d'entrada
                iss >> I.C >> I.M >> I.K;
            }

            if (u == 1) {
                // llegim els M enters c_e
                int capacity;
                while (iss >> capacity) {
                    I.c_e.push_back(capacity);
                }
            }

            if (u == 2) {
                // llegim els M enters n_e
                int qty;
                while (iss >> qty) {
                    I.n_e.push_back(qty);
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
                I.prod.push_back(qty_prod);

                // llegim el vector de millores
                int upgrade;
                upgrades up;
                for (int j = 0; j < I.M; ++j) {
                    iss1 >> upgrade;
                    up.push_back(upgrade);
                }
                I.upgr.push_back(up);
            }
        }
    }
    vector<int> init(I.C, -1);
    S.permutation = init;

    vector<vector<int>> init1(I.M, vector<int>(I.C, -1));
    S.req = init1;
    return I;
}

int count_penalty(Input& I, Sol& S, vector<int>& s_partial, int limit)
{

    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int count = 0;
    for (int j = 0; j < M; ++j) {
        int n_j = n_e[j];
        int c_j = c_e[j];
        int pen = -c_j; // si la penalty excedeix en k a 0, significa que la penalty és k.
        for (int k = 0; k < limit; ++k) {
            pen += req[j][k];
            if (k >= n_j)
                pen -= req[j][k - n_j];
            if (pen > 0)
                count += pen;
        }

        if (limit == C) {
            // comptar últimes finestres
            for (int k = 0; k < n_j; ++k) {
                pen -= req[j][C - n_j + k];
                if (pen > 0)
                    count += pen;
            }
        }
    }
    return count;
}

void opt(Input& I, Sol& S, int k, vector<int>& s_partial)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    if (k == C) {
        int a_pen = count_penalty(I, S, s_partial, k);
        if (S.penalty > a_pen) {
            S.penalty = a_pen;
            S.permutation = s_partial;
        }
    } else {
        for (int i = 0; i < K; ++i) {
            if (prod[i] > 0) {
                --prod[i];
                s_partial[k] = i;
                for (int j = 0; j < M; ++j)
                    S.req[j][k] = I.upgr[i][j];
                int partial_penalty = count_penalty(I, S, s_partial, k);
                if (partial_penalty < S.penalty)
                    opt(I, S, k + 1, s_partial);
                for (int j = 0; j < M; ++j)
                    S.req[j][k] = -1;
                ++prod[i];
                s_partial[k] = -1;
            }
        }
    }
}

int main(int argc, char** argv)
{
    string f_i = argv[1];
    //string f_o = argv[2];
    Sol S;
    Input I = read_input(f_i, S);
    vector<int> s_partial(I.C, -1);
    opt(I, S, 0, s_partial);
    cout << S.penalty << endl;
    for (int& e : S.permutation)
        cout << e << " ";
    cout << endl;
}
