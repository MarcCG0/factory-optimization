#include <algorithm>
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

Input read_input(const string& a, Sol& S, vector<pair<int, int>>& qty2)
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
            int index = 0;
            while (getline(myfile, line)) {
                istringstream iss1(line);

                int count = 0;

                // llegim la classe i quantitat a produir
                int classe, qty_prod;
                iss1 >> classe >> qty_prod;
                I.prod.push_back(qty_prod);

                // llegim el vector de millores
                int upgrade;
                upgrades up;
                for (int j = 0; j < I.M; ++j) {
                    iss1 >> upgrade;
                    count += upgrade;
                    up.push_back(upgrade);
                }
                qty2.push_back({ count, index });
                ++index;
                I.upgr.push_back(up);
            }
            sort(qty2.begin(), qty2.end());
        }
    }
    vector<int> init(I.C, -1);
    S.permutation = init;

    vector<vector<int>> init1(I.M, vector<int>(I.C, -1));
    S.req = init1;
    return I;
}

int count_penalty(Input& I, Sol& S, vector<int>& s_partial, vector<int>& penalizations)
{

    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int count = 0;
    for (int j = 0; j < M; ++j) {
        int n_j = n_e[j];
        int pen_j = penalizations[j]; // si la penalty excedeix en k a 0, significa que la penalty és k.
        // for (int k = 0; k < C; ++k) {
        //     pen += req[j][k];
        //     if (k >= n_j)
        //         pen -= req[j][k - n_j];
        //     if (pen > 0)
        //         count += pen;
        // }

        // comptar últimes finestres
        for (int k = 0; k < n_j; ++k) {
            pen_j -= req[j][C - n_j + k];
            if (pen_j > 0)
                count += pen_j;
        }
    }
    return count;
}

void update_penalizations(vector<int>& penalizations, Input& I, Sol& S, int k, int& partial_penalty)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    for (int i = 0; i < M; ++i) {
        int n_i = n_e[i];
        penalizations[i] += req[i][k];
        if (k >= n_i)
            penalizations[i] -= req[i][k - n_i];
        if (penalizations[i] > 0)
            partial_penalty += penalizations[i];
    }
    //     cout << "-------------------" << endl;
    //     for (int& e : penalizations)
    //         cout << e << " ";
    //     cout << endl;
    //     cout << "Index -> " << k << endl;
    //     for (vector<int>& v : req) {
    //         for (int& e : v)
    //             cout << e << " ";
    //         cout << endl;
    //     }
    //     cout << "Partial Penalty -> " << partial_penalty << endl;
    //     cout << "-------------------" << endl;
    // }
}

void restore_penalizations(vector<int>& penalizations, Input& I, Sol& S, int k, int& partial_penalty)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    for (int i = 0; i < M; ++i) {
        if (penalizations[i] > 0)
            partial_penalty -= penalizations[i];
        int n_i = n_e[i];
        penalizations[i] -= req[i][k];
        if (k >= n_i)
            penalizations[i] += req[i][k - n_i];
    }
}

void print(vector<int>& s_partial, ofstream& myfile)
{
    for (int a : s_partial)
        myfile << a << " ";
    myfile << endl;
}

void opt(Input& I, Sol& S, int k, vector<int>& s_partial, vector<int>& penalizations, int partial_penalty, const string& f_o, vector<pair<int, int>>& ordered_classes)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    if (k == C) {
        int a_pen = partial_penalty + count_penalty(I, S, s_partial, penalizations);
        if (penalty > a_pen) {
            penalty = a_pen;
            permutation = s_partial;
            // escriure a fitxer
            ofstream myfile;
            myfile.open(f_o);
            myfile << a_pen << " 3.4" << endl;
            print(s_partial, myfile);
            myfile.close();
        }

    } else {
        for (int i = 0; i < K; ++i) {
            int c = ordered_classes[i].second;
            if (prod[c] > 0) {
                --prod[c];
                s_partial[k] = c;
                for (int j = 0; j < M; ++j)
                    req[j][k] = upgr[c][j];
                update_penalizations(penalizations, I, S, k, partial_penalty);
                if (partial_penalty < penalty)
                    opt(I, S, k + 1, s_partial, penalizations, partial_penalty, f_o, ordered_classes);
                restore_penalizations(penalizations, I, S, k, partial_penalty);
                ++prod[c];
            }
        }
    }
}

int main(int argc, char** argv)
{
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    vector<pair<int, int>> ordered_classes;
    Input I = read_input(f_i, S, ordered_classes);
    vector<int> s_partial(I.C, -1);
    vector<int> penalizations(I.M);
    for (int i = 0; i < I.M; ++i)
        penalizations[i] = -I.c_e[i];
    int partial_penalty = 0;

    opt(I, S, 0, s_partial, penalizations, partial_penalty, f_o, ordered_classes);
    cout << S.penalty << endl;
    for (int& e : S.permutation)
        cout << e << " ";
    cout << endl;
}
