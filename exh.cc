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

int count_penalty(Input& I, Sol& S, vector<int>& s_partial, vector<int>& penalizations)
{

    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int count = 0;
    for (int j = 0; j < M; ++j) {
        int n_j = n_e[j];
        int pen_j = penalizations[j];

        // count last windows
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

void writeIntoFile(const string& f_o, const vector<int>& s_partial, int a_pen)
{
    ofstream myfile;
    myfile.open(f_o);

    myfile << a_pen << " " << float(clock()) / CLOCKS_PER_SEC << endl;

    for (int a : s_partial)
        myfile << a << " ";
    myfile << endl;

    myfile.close();
}

int low_bound(Input& I, Sol& S, int k)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    int p = 0;
    int spaces = C - k - 1;
    for (int i = 0; i < M; ++i) {
        int uns = 0;
        int n_i = n_e[i], c_i = c_e[i];
        for (int j = 0; j < K; ++j) {
            uns += prod[j] * upgr[j][i];
        }
        int zeros = spaces - uns;
        int perfect_windows = zeros / (n_i - c_i);

        int max_uns = (perfect_windows + 1) * c_i;
        // Afegir 1 de penalty per cada un de mes
        p += max(uns - max_uns, 0);
    }
    return p;
}

void opt(Input& I, Sol& S, int k, vector<int>& s_partial, vector<int>& penalizations, int partial_penalty, const string& f_o)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;

    if (partial_penalty >= penalty)
        return;

    // check if actual permutation can be optimal in a certain time stamp
    if (k == C) {

        // count last penalizations regarding last windows
        int a_pen = partial_penalty + count_penalty(I, S, s_partial, penalizations);

        // update actual optimal answer
        if (penalty > a_pen) {
            penalty = a_pen;
            permutation = s_partial;

            // write last best answer found
            writeIntoFile(f_o, s_partial, a_pen);
        }

    } else {
        for (int i = 0; i < K; ++i) {
            // if actual permutation accepts members from class i, try inserting
            // i on it
            if (prod[i] > 0) {
                --prod[i];
                s_partial[k] = i;

                // prepare
                for (int j = 0; j < M; ++j)
                    req[j][k] = upgr[i][j];
                update_penalizations(penalizations, I, S, k, partial_penalty);
                if (partial_penalty + low_bound(I, S, k) < penalty)
                    opt(I, S, k + 1, s_partial, penalizations, partial_penalty, f_o);
                restore_penalizations(penalizations, I, S, k, partial_penalty);
                ++prod[i];
            }
        }
    }
}

int main(int argc, char** argv)
{
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    Input I = read_input(f_i, S);
    vector<int> s_partial(I.C, -1);
    vector<int> penalizations(I.M);
    for (int i = 0; i < I.M; ++i)
        penalizations[i] = -I.c_e[i];
    int partial_penalty = 0;
    opt(I, S, 0, s_partial, penalizations, partial_penalty, f_o);
    cout << S.penalty << endl;
    for (int& e : S.permutation)
        cout << e << " ";
    cout << endl;
}
