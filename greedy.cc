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

/* Explanation of req Matrix
Given a permutation, given (i, j) a position in req
it contains a 0, we know that the class located in the j-th position of the permutation
doesn't need upgrade i.
If it contains a 1, it does need it.
*/
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

    vector<vector<int>> init1(I.M, vector<int>(I.C, -1));
    S.req = init1;
    S.permutation = vector<int>(I.C, -1);
    return I;
}

void writeIntoFile(const string& f_o, const vector<int>& s_partial, int a_pen)
{
    ofstream myfile;
    // open file, write demanded output and close file
    myfile.open(f_o);
    myfile << a_pen << " " << float(clock()) / CLOCKS_PER_SEC << endl;
    for (int a : s_partial)
        myfile << a << " ";
    myfile << endl;
    myfile.close();
}

void generate_perm(Input& I, const vector<vector<int>>& R, vector<int>& permutation, vector<int>& prod, int K)
{
    int n = I.C;
    // for all cars
    for (int i = 0; i < n / 2; ++i) {
        // for all possible classes
        int j = -1;
        int max_cars = -1;
        for (int u = 0; u < K; ++u) {
            if (prod[u] > max_cars) {
                max_cars = prod[u];
                j = u;
            }
        }
        // if all j-th cars have been produced, skip
        if (prod[j] != 0) {

            if (prod[j] >= 2) {
                permutation[i] = j;
                permutation[n - i - 1] = j;
                prod[j] -= 2;
            }
            // we know that prod[j] == 1
            else {
                // place one and get another with the greater number of pending fabrications
                permutation[i] = j;
                --prod[j];
                int max_prod = -1;
                int index = 0;
                for (int k = 0; k < K; ++k) {
                    if (prod[k] >= max_prod) {
                        max_prod = prod[k];
                        index = k;
                    }
                }
                --prod[index];
                permutation[n - i - 1] = index;
            }
        }
    }
}

int count_penalty(Input& I, Sol& S, vector<int>& s_partial)
{

    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int count = 0;

    // for every station (upgrade)
    for (int j = 0; j < M; ++j) {
        int n_j = n_e[j];
        int c_j = c_e[j];
        int pen = -c_j;

        // for every car
        for (int k = 0; k < C; ++k) {

            //update penalties
            pen += req[j][k];
            if (k >= n_j)
                pen -= req[j][k - n_j];
            if (pen > 0)
                count += pen;
        }

        // count last window's penalties
        for (int k = 0; k < n_j; ++k) {
            pen -= req[j][C - n_j + k];
            if (pen > 0)
                count += pen;
        }
    }
    return count;
}

vector<vector<int>> build_req(Sol& S, Input& I)
{
    // prepare req in order to count penalizations

    vector<vector<int>> v(I.M, vector<int>(I.C, 0));

    for (int i = 0; i < I.M; ++i) {
        for (int j = 0; j < I.C; ++j) {

            if (I.upgr[S.permutation[j]][i])
                v[i][j] = 1;
        }
    }
    return v;
}

void greedy(Sol& S, Input& I, const string& f_o)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;

    vector<vector<int>> R(K, vector<int>(K, 0));

    // greedy algorithm: generate a decent answer
    generate_perm(I, R, permutation, prod, K);

    // prepare counting penalties
    S.req = build_req(S, I);
    penalty = count_penalty(I, S, permutation);

    // write greedy answer
    writeIntoFile(f_o, permutation, penalty);
}

int main(int argc, char** argv)
{
    // read input in the given format
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    Input I = read_input(f_i, S);
    // compute greedy answer
    greedy(S, I, f_o);
}
