#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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

// Computes additional penalty of adding last car in a permutation.
int count_last_windows(Input& I, Sol& S, vector<int>& s_partial, vector<int>& penalties)
{
    int count = 0;
    // For each upgrade.
    for (int j = 0; j < I.M; ++j) {

        // Get window size and the penalty of
        // the last window before adding the last class.
        int n_j = I.n_e[j];
        int pen_j = penalties[j];

        // Count penalty of the last windows.
        for (int k = 0; k < n_j; ++k) {
            pen_j -= S.req[j][I.C - n_j + k];
            if (pen_j > 0)
                count += pen_j;
        }
    }
    return count;
}

void update_penalizations(vector<int>& penalties, Input& I, Sol& S, int k, int& partial_penalty)
{
    // For each upgrade
    for (int i = 0; i < I.M; ++i) {
        int n_i = I.n_e[i];
        penalties[i] += S.req[i][k];

        // When we have already assigned as many classes as the
        // size of the window, we start substracting the classes
        // from the beggining to keep the correct amount of classes
        // in the window that is being considered at the moment.
        if (k >= n_i)
            penalties[i] -= S.req[i][k - n_i];
        // If pen is positive > 0, it means that placing the last car produces
        // a extra penalty (recall the initial state of the
        // vector penalties).
        if (penalties[i] > 0)
            partial_penalty += penalties[i];
    }
}

void restore_penalizations(vector<int>& penalties, Input& I, Sol& S, int k, int& partial_penalty)
{
    // For each upgrade
    for (int i = 0; i < I.M; ++i) {

        // If last change in recursion caused a modification on penalty
        // then this change must be undone.
        if (penalties[i] > 0)
            partial_penalty -= penalties[i];
        int n_i = I.n_e[i];
        penalties[i] -= S.req[i][k];
        // As in the function update_penalizations we have substracted this
        // value when k >= n_i, while restoring the penalties we have
        // to add this value.
        if (k >= n_i)
            penalties[i] += S.req[i][k - n_i];
    }
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

int low_bound(Input& I, Sol& S, int k)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    // We will refer to the lower bound of the penalty as p.
    int p = 0;
    /*
        LOWER BOUND ALGORITHM EXPLANATION

        The idea is to try to organize the remaining classes in such a
        way that we minimize the penalty. If we do this for every update,
        the lower bound obtained will obviously be lower than the real
        lower bound, as we cannot use a different organization of classes
        for each update.

        First we obtain the number of ones and zeros that we have. Then we
        compute the number of perfect windows (that is a sequence of n_i
        classes which do not create a penalty) that we can create with the
        amount of zeros that we have.

        Once we have this number of perfect_windows we determine the number
        of max_ones (the number of cars that require this upgrade) that we can
        organize in such a way that no penalty is generated. Then, at least,
        the penalty generated will be the number of ones that we have
        minus the number of max_ones that we have computed.
    */

    // Empty positions left in the permutation.
    int spaces = C - k - 1;
    // For each upgrade.
    for (int i = 0; i < M; ++i) {
        int uns = 0;
        int n_i = n_e[i], c_i = c_e[i];
        // For each class

        // Compute LOWER BOUND ALGORITHM
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

void opt(Input& I, Sol& S, int k, vector<int>& s_partial, vector<int>& penalties, int partial_penalty, const string& f_o)
{
    auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;

    if (partial_penalty >= penalty)
        return;

    // Check if actual permutation can be optimal in a certain time stamp.
    if (k == I.C) {

        // Count last penalties regarding last windows.
        int a_pen = partial_penalty + count_last_windows(I, S, s_partial, penalties);

        // Update actual optimal answer when needed.
        if (penalty > a_pen) {
            penalty = a_pen;
            permutation = s_partial;

            // Write last best answer found.
            writeIntoFile(f_o, s_partial, a_pen);
        }

    } else {
        // For each class
        for (int i = 0; i < I.K; ++i) {
            // If actual permutation accepts members from class i, try
            // inserting class i on it.
            if (prod[i] > 0) {
                --prod[i];
                s_partial[k] = i;

                // Prepare req for recursion.
                for (int j = 0; j < I.M; ++j)
                    req[j][k] = upgr[i][j];
                // Update the penalties regarding the insertion of i in
                // the permutation.
                update_penalizations(penalties, I, S, k, partial_penalty);
                // If the actual permutation can still be optimal, keep
                // recursing, otherwise, stop and do not recurse anymore
                if (partial_penalty + low_bound(I, S, k) < penalty)
                    opt(I, S, k + 1, s_partial, penalties, partial_penalty, f_o);
                // Undo the penalties that i caused on the permutation
                restore_penalizations(penalties, I, S, k, partial_penalty);
                ++prod[i];
            }
        }
    }
}

int main(int argc, char** argv)
{
    // Obtain the name of the file that contains
    // the input (f_i) & the output file (f_o)
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    // Reading the data from the file
    Input I = read_input(f_i, S);
    vector<int> s_partial(I.C, -1);
    /*
        This vector will contain the penalty of the last window
        that is being considered. It is initialized as -I.c_e[i] because
        this way, whenever it reaches a positive number we will know
        that there is going to be a penalty and that the value that the
        vector contains is going to be the penalty.
    */
    vector<int> penalties(I.M);
    for (int i = 0; i < I.M; ++i)
        penalties[i] = -I.c_e[i];

    // Compute optimal permutation and its respective penalty
    int partial_penalty = 0;
    opt(I, S, 0, s_partial, penalties, partial_penalty, f_o);
}
