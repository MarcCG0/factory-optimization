#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

struct Input {
    int C, M, K;
    vector<int> c_e, n_e, prod;
    vector<vector<int>> upgr;
};

struct Sol {
    int penalty = INT_MAX;
    vector<int> permutation = {};
    vector<vector<int>> req;
};

Input readInput(const string& a, Sol& S)
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
                vector<int> up;
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

vector<vector<int>> buildReq(Sol& S, Input& I)
{
    // Prepare req in order to count penalizations
    vector<vector<int>> v(I.M, vector<int>(I.C, 0));

    for (int i = 0; i < I.M; ++i) {
        for (int j = 0; j < I.C; ++j) {

            if (I.upgr[S.permutation[j]][i])
                v[i][j] = 1;
        }
    }
    return v;
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

int chooseFirstElement(vector<int>& permutation, vector<int>& prod, int K)
{
    int first_class = -1;
    int max_prod = 0;
    for (int i = 0; i < K; ++i) {
        if (prod[i] > max_prod) {
            max_prod = prod[i];
            first_class = i;
        }
    }
    return first_class;
}

int computePenalty(vector<int>& permutation, Input& I, int car)
{
    // cout << "---------------------" << endl;

    // cout << "Current permutation:" << endl;
    // for (int& i : permutation)
    //     cout << i << " ";
    // cout << endl;

    // The penalty will be the sum of the penalties for each upgrade
    int assigned = permutation.size();
    int total_penalty = 0;
    for (int i = 0; i < I.M; ++i) {
        // cout << "   Upgrade " << i << endl;
        // First we compute the maximum size of the windows that we will treat
        int max_window = min(I.n_e[i], assigned + 1);
        // We have to take into account all the possible windows that can generate a penalty (without regarding futur schedulings)
        int j = 1;
        for (int size = max_window; size > I.c_e[i]; --size) {
            // cout << "       Window " << j << " of size " << size << ": ";
            int partial_penalty = -I.c_e[i] + I.upgr[car][i];
            for (int pos = assigned - size + 1; pos < assigned; ++pos) {
                if (I.upgr[permutation[pos]][i])
                    ++partial_penalty;
            }
            // cout << max(0, partial_penalty) << endl;
            // cout << "       Partial penalty was initialized to: " << -I.c_e[i] + I.upgr[car][i] << endl;

            total_penalty += max(0, partial_penalty);
            ++j;
        }
    }
    // cout << "Total penalty for class " << car << " is " << total_penalty << endl;
    // cout << "---------------------" << endl;
    return total_penalty;
}

void generatePerm(Input& I, vector<int>& permutation, vector<int>& prod, int K)
{
    // We choose the first class from the schedule
    int first_class = chooseFirstElement(permutation, prod, I.K);

    // Update variables according to the choice
    permutation.push_back(first_class);
    --prod[first_class];

    // We choose a class for each remaining position of the schedule
    for (int pos = 1; pos < I.C; ++pos) {
        int min_penalty = INT_MAX;
        int best_class = -1;

        // We compare the penalty that would generate scheduling each class to the actual position
        for (int car = 0; car < I.K; ++car) {
            // If there are no more cars to produce, skip
            if (prod[car] == 0)
                continue;

            // We compute the penalty that this class would generate given the previous assignments
            int penalty_class = computePenalty(permutation, I, car);
            int actual_class = car;

            // We update the variables in case it is necessary
            // When we have found a class which minimizes the penalty even more
            if (penalty_class < min_penalty) {
                min_penalty = penalty_class;
                best_class = actual_class;
            }
            // In case of a tie we choose the one with higher production
            else if (penalty_class == min_penalty and prod[actual_class] > prod[best_class]) {
                min_penalty = penalty_class;
                best_class = actual_class;
            }
        }

        permutation.push_back(best_class);
        --prod[best_class];
    }
}

int countPenalty(Input& I, Sol& S)
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

void greedy(Sol& S, Input& I, const string& f_o)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;

    // Greedy algorithm -> Generates a decent schedule
    generatePerm(I, permutation, prod, K);

    // Prepare what is needed to count the final penalty
    S.req = buildReq(S, I);
    penalty = countPenalty(I, S);

    // Write the greedy answer into the given file
    writeIntoFile(f_o, permutation, penalty);
}

int main(int argc, char** argv)
{
    // Read the input ib the given format
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    Input I = readInput(f_i, S);

    // Compute the greedy answer
    greedy(S, I, f_o);
}
