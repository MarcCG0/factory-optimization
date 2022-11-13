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
    vector<int> permutation = {};
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

    cout << "Penalty is: " << a_pen << endl;
    for (int a : s_partial)
        cout << a << " ";
    cout << endl;
}

void prepare_relations(vector<vector<int>>& R, const vector<vector<int>>& upgr)
{

    // generates symmetric matrix in which (i,j) indicates
    // how much upgrades classes i and j have in common
    for (uint k = 0; k < upgr.size(); ++k) {
        for (uint i = k; i < upgr.size(); ++i) {
            for (uint j = 0; j < upgr[0].size(); ++j) {
                if (upgr[k][j] == upgr[i][j] && upgr[k][j] != 0) {
                    R[k][i] += 1;
                    if (k != i)
                        R[i][k] += 1;
                }
            }
        }
    }
}

void choose_first_el(vector<int>& permutation, const vector<vector<int>>& R)
{
    int best = INT_MIN;
    int i_b;

    // inspect who requires more upgrades
    for (uint i = 0; i < R.size(); ++i) {
        if (best < R[i][i]) {
            i_b = i;
            best = R[i][i];
        }
    }
    // add that class to the permutation
    permutation.push_back(i_b);
}

int get_similarity(const Input& I, int classe, int class_i, int upgr)
{
    // returns wheter classes classe & class_i both need upgrade upgr
    if (I.upgr[classe][upgr] == I.upgr[class_i][upgr] && I.upgr[classe][upgr] != 0) {
        return 1;
    } else {
        return 0;
    }
}

int get_similarities(const Input& I, const vector<vector<int>>& R, const vector<int>& permutation, int classe)
{
    int n = permutation.size(); // # elements already placed
    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;

    int best_actual = 0;
    int penalty = 0;

    for (int i = 0; i < M; ++i) {
        int actual_window = n_e[i];

        if (n - actual_window < 0)
            actual_window = n;

        for (int j = n - actual_window; j < n; ++j) {
            best_actual += get_similarity(I, classe, permutation[j], i);
            if (best_actual > c_e[i])
                penalty += 1;
        }
    }

    return penalty;
}

void generate_perm(Input& I, const vector<vector<int>>& R, vector<int>& permutation, vector<int>& prod, int K)
{
    int n = I.C;

    choose_first_el(permutation, R);

    --prod[permutation[0]];

    // for all cars
    for (int i = 1; i < n; ++i) {
        int index;
        int penalty_min = INT_MAX;
        int qty_p = -1; // qty left for production

        // for all possible classes
        for (int j = 0; j < K; ++j) {

            // if all j-th cars have been produced, skip
            if (prod[j] == 0)
                continue;

            int penalty = get_similarities(I, R, permutation, j);

            if (prod[j] > qty_p) {
                qty_p = prod[j];
                index = j;
                penalty_min = penalty;
            } else if (prod[j] == qty_p && penalty_min > penalty) {
                qty_p = prod[j];
                index = j;
                penalty_min = penalty;
            }
        }
        // update consequences of having add index to the permutation
        --prod[index];
        permutation.push_back(index);
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

    prepare_relations(R, upgr);

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
