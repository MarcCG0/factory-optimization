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
    vector<int> prod; // Quantity of cars to produce (for each class).
    vector<vector<int>> upgr;
    // Binary vector which indicates wether a class
    // requires an upgrade or not (for each class).
    // At the j-th column there are the cars thar require the j-th upgrade.
};

/*
    Explanation of req Matrix
    Given a permutation, given (i, j) a position in req
    it contains a 0, we know that the class located in the j-th
    position of the permutation
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
    // Open the file and iterate on it.
    string line;
    ifstream myfile(a);

    Input I;

    for (int u = 0; u < 4; ++u) {
        if (u < 3) {
            getline(myfile, line);
            istringstream iss(line);

            if (u == 0) {
                // Read three input integers.
                iss >> I.C >> I.M >> I.K;
            }

            if (u == 1) {
                // Read the M integers corresponding to c_e.
                int capacity;
                while (iss >> capacity) {
                    I.c_e.push_back(capacity);
                }
            }

            if (u == 2) {
                // Read the M integers corresponding to n_e.
                int qty;
                while (iss >> qty) {
                    I.n_e.push_back(qty);
                }
            }
        }
        if (u == 3) {
            // Read K lines.
            while (getline(myfile, line)) {
                istringstream iss1(line);

                // Read i-th class and qty to produce of i-th class.
                int classe,
                    qty_prod;
                iss1 >> classe >> qty_prod;
                I.prod.push_back(qty_prod);

                // Read upgrades vector.
                int upgrade;
                vector<int> up; // Store upgrades.
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

void writeIntoFile(const string& f_o, const vector<int>& s_partial, int a_pen)
{
    // Open the file that we will store our solutions on.
    ofstream myfile;
    myfile.open(f_o);
    // We write the minimum penalty that we have found and the time
    // taken to find this penalty.
    myfile << a_pen << " " << float(clock()) / CLOCKS_PER_SEC << endl;

    // Write the corresponding permutation to the optimal solution found
    // until this precise moment.
    for (int a : s_partial)
        myfile << a << " ";
    myfile << endl;
    // Close the file.
    myfile.close();
}

void generate_perm(Input& I, const vector<vector<int>>& R, vector<int>& permutation, vector<int>& prod, int K)
{
    int n = I.C;
    // For all cars (We only iterate until n/2 because we assign the
    // same class for the i-th position and the (n-i-1)-th position).
    for (int i = 0; i < n / 2; ++i) {
        int j = -1;
        int max_cars = -1;
        // We iterate through all the classes to get the one with
        // the most cars to produce at the moment.
        for (int u = 0; u < K; ++u) {
            if (prod[u] > max_cars) {
                max_cars = prod[u];
                j = u;
            }
        }
        // If all j-th cars have been produced, skip.
        if (prod[j] != 0) {
            // As we have at least two cars to produce of this class
            // we assign them to the positions i-th and (n-i-1)-th.
            if (prod[j] >= 2) {
                permutation[i] = j;
                permutation[n - i - 1] = j;
                prod[j] -= 2;
            }
            // We know that prod[j] == 1.
            else {
                // Place one and get another with the greater
                // number of pending fabrications.
                permutation[i] = j;
                --prod[j];
                int max_prod = -1;
                int index = 0;

                // Choose the other class with greater pending fabrications.
                for (int k = 0; k < K; ++k) {
                    if (prod[k] >= max_prod) {
                        max_prod = prod[k];
                        index = k;
                    }
                }
                // Insert that class to the permutation.
                --prod[index];
                permutation[n - i - 1] = index;
            }
        }
    }
}

int count_penalty(Input& I, Sol& S, vector<int>& s_partial)
{
    int pen = 0;
    // For every station (upgrade).
    for (int j = 0; j < I.M; ++j) {
        int n_j = I.n_e[j];
        int c_j = I.c_e[j];
        int actual_pen = -c_j;

        // For every car.
        for (int k = 0; k < I.C; ++k) {

            // Update penalties.
            actual_pen += S.req[j][k];
            if (k >= n_j)
                actual_pen -= S.req[j][k - n_j];
            if (actual_pen > 0)
                pen += actual_pen;
        }

        // Count last window's penalties.
        for (int k = 0; k < n_j; ++k) {
            actual_pen -= S.req[j][I.C - n_j + k];
            if (actual_pen > 0)
                pen += actual_pen;
        }
    }
    return pen;
}

vector<vector<int>> build_req(Sol& S, Input& I)
{
    // Prepare req in order to count penalizations.
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
    auto& [penalty, permutation, req] = S;

    // Greedy algorithm: generate a decent answer.
    vector<vector<int>> R(I.K, vector<int>(I.K, 0));
    generate_perm(I, R, permutation, I.prod, I.K);

    // Prepare counting penalties.
    req = build_req(S, I);
    penalty = count_penalty(I, S, permutation);

    // Write greedy answer.
    writeIntoFile(f_o, permutation, penalty);
}

int main(int argc, char** argv)
{
    // Read input in the given format.
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    Input I = read_input(f_i, S);
    // Compute greedy solution.
    greedy(S, I, f_o);
}
