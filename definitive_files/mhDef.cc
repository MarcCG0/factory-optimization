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
    // Binary vector which indicates whether a class
    // requires an upgrade or not (for each class).
    // At the j-th column there are the cars thar require the j-th upgrade.
};

struct Sol {
    int penalty = INT_MAX;
    vector<int> permutation;
    vector<vector<int>> req;
};

/*
Prepares data structures for the Algorithm.
*/
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

/*
  Given an input, computes a greedy permutation. (= greedy defined in greedy.cc)
*/
void generate_perm(Input& I, Sol& S)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int n = I.C;
    // For all cars.
    for (int i = 0; i < n / 2; ++i) {
        // For all possible classes.
        int j = -1;
        int max_cars = -1;
        for (int u = 0; u < K; ++u) {
            if (prod[u] > max_cars) {
                max_cars = prod[u];
                j = u;
            }
        }
        // If all j-th cars have been produced, skip.
        if (prod[j] != 0) {

            if (prod[j] >= 2) {
                permutation[i] = j;
                permutation[n - i - 1] = j;
                prod[j] -= 2;
            }
            // We know that prod[j] == 1.
            else {
                // Place one and get another with the greater number of pending fabrications.
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

/*
    Returns the req matrix for a given permutation of cars. This matrix is
    built as follows. Let c1, c2, ..., cn be a car permutation, where each
    car belongs to a certain class. Then we can give the permutation in terms
    of the classes. This will be k1, k2, ..., kn, where for any i, j, the
    class ki can be the same as kj. Then, each column of the matrix will be a
    vector of size M (M is the number of upgrades) where at each position
    we have wether the class requires the upgrade or not. Consequently, each
    row (refering to an upgrde) is a vector of size n where at each position
    we have wether the car assigned to that position of the permutation
    requires the upgrade or not.
*/
vector<vector<int>> build_req(Sol& S, Input& I)
{
    // Prepare req in order to count penalties.

    vector<vector<int>> v(I.M, vector<int>(I.C, 0));

    for (int i = 0; i < I.M; ++i) {
        for (int j = 0; j < I.C; ++j) {

            if (I.upgr[S.permutation[j]][i])
                v[i][j] = 1;
        }
    }
    return v;
}

/*
    Returns the penalty that a given distribution generates.
*/
int count_penalty(Input& I, Sol& S)
{

    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int count = 0;

    // For every station (upgrade).
    for (int j = 0; j < M; ++j) {
        int n_j = n_e[j];
        int c_j = c_e[j];
        int pen = -c_j;

        // For every class.
        for (int k = 0; k < C; ++k) {

            //  Update penalties.
            pen += req[j][k];
            if (k >= n_j)
                pen -= req[j][k - n_j];
            if (pen > 0)
                count += pen;
        }

        // Count last window's penalties.
        for (int k = 0; k < n_j; ++k) {
            pen -= req[j][C - n_j + k];
            if (pen > 0)
                count += pen;
        }
    }
    return count;
}

/*
    Writes into the given file the best penalty found until that moment and
    the permutation that achieves this penalty. It also specifies the time
    taken to find this particular solution.
*/
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
/*
    Returns whether a solution is upgradeable
*/
bool improve(Input& I, Sol& S)
{
    int init_p = S.penalty;
    // For every possible swap in the permutation.
    for (uint i = 0; i < I.C; ++i) {
        for (uint j = 0; j < I.C; ++j) {
            // If i-th number and j-th number are different.
            if (i != j && S.permutation[i] != S.permutation[j]) {
                // Try to swap them and check whether this upgrades the
                // solution.
                swap(S.permutation[i], S.permutation[j]);
                S.req = build_req(S, I);
                int c_pen = count_penalty(I, S);
                // If this modification causes an upgrade, make the change.
                // Otherwise undo the swap and return to the last permutation.
                if (S.penalty > c_pen)
                    S.penalty = c_pen;
                else
                    swap(S.permutation[i], S.permutation[j]);
            }
        }
    }
    return init_p > S.penalty;
}

/*
    Looks for the best permutation that 'minimizes' the penalty given an initial
    distribution
*/
void opt(Input& I, Sol& S, const string& f_o, int& best_penalty, vector<int>& best_perm)
{
    // We build the initial req matrix for the distribution.
    S.req = build_req(S, I);
    // We compute initial penalty of the distribution.
    S.penalty = count_penalty(I, S);
    // We keep improving the distribution by permutations until no
    // no improvement can be done.
    while (improve(I, S))
        ;
    // We update the best penalty and permutation when necessary.
    if (S.penalty < best_penalty) {
        best_perm = S.permutation;
        best_penalty = S.penalty;
        // We write the new solution into the file.
        writeIntoFile(f_o, best_perm, best_penalty);
    }
}

/*
    Computes the 'minimum' penalty given a certain Input and the permutation that achieves this penalty.
*/
void mh(Input& I, Sol& S, const string& f_o)
{
    // We generate an initial distribution, which is the greedy one.
    generate_perm(I, S);
    // We initialize the values for the best penalty and permutation.
    int best_penalty = INT_MAX;
    vector<int> best_perm(I.C, -1);
    // We keep optimizing the values for different distributions.
    while (true) {
        // We optimize the given distribution.
        opt(I, S, f_o, best_penalty, best_perm);
        // We get two random positions, the ones that we will change.
        int pos1 = rand() % I.C;
        int pos2 = pos1;
        while (pos2 == pos1) {
            pos2 = rand() % I.C; // [0, ..., I.C-1]
        }
        // We swap the positions calculated above (from the best permutation
        // obtained in the last distribution).
        swap(S.permutation[pos1], S.permutation[pos2]);
    }
}

int main(int argc, char** argv)
{
    // We get the input files.
    string f_i = argv[1];
    string f_o = argv[2];
    // We generate a random seed for the rand() function.
    srand(time(NULL));
    Sol S;
    // Read input in the given format.
    Input I = read_input(f_i, S);
    // Execute the methaheuristics algorithms.
    mh(I, S, f_o);
}
