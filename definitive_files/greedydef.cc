#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

/*
  C, M, K integers containing the respective values of the input.
  c_e[i] indicates the c_e of class i.
  n_e[i] indicates the n_e of class i.
  prod[i] indicates the quantity left to produce of class i.
  upgr[i][j] indicates whether class i requires upgrade j.
*/
struct Input {
    int C, M, K;
    vector<int> c_e, n_e, prod;
    vector<vector<int>> upgr;
};
/*
  penalty stores the penalty of the best solution so far.
  permutation is the best permutation so far.
  req[i][j] contains a 0 if the class located in the j-th position of
  the permutation doesn't need upgrade i. Otherwise, it contains a 1.
*/
struct Sol {
    int penalty = INT_MAX;
    vector<int> permutation = {};
    vector<vector<int>> req;
};

Input readInput(const string& a, Sol& S)
{
    // Open file and iterate through it.
    string line;
    ifstream my_file(a);
    Input I;

    for (int u = 0; u < 4; ++u) {
        if (u < 3) {
            getline(my_file, line);
            istringstream iss(line);

            if (u == 0) {
                // Read the three natural numbers cointained in the input.
                iss >> I.C >> I.M >> I.K;
            }

            else if (u == 1) {
                // Read the M integers c_e.
                int capacity;
                while (iss >> capacity) {
                    I.c_e.push_back(capacity);
                }
            }

            else {
                // Read the M integers n_e.
                int qty;
                while (iss >> qty) {
                    I.n_e.push_back(qty);
                }
            }
        } else {
            // Read K lines.
            while (getline(my_file, line)) {
                istringstream iss1(line);

                // Read the class and quantity to produce of that class.
                int c, qty_prod;
                iss1 >> c >> qty_prod;
                I.prod.push_back(qty_prod);

                // Fill the upgrade vector.
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
    // Initialize req matrix.
    S.req = vector<vector<int>>(I.M, vector<int>(I.C, -1));
    return I;
}

vector<vector<int>> buildReq(Sol& S, Input& I)
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

void writeIntoFile(const string& f_o, const vector<int>& s_partial, int a_pen)
{
    // Open the file that we will store our solutions on.
    ofstream my_file;
    my_file.open(f_o);
    my_file.setf(ios::fixed);
    my_file.precision(1);
    /*
        We write the minimum penalty that we have found and the time
        taken to find this penalty.
    */
    my_file << a_pen << " " << float(clock()) / CLOCKS_PER_SEC << endl;

    /*
        Write the corresponding permutation to the optimal solution found
        until this precise moment.
    */
    for (int a : s_partial)
        my_file << a << " ";
    my_file << endl;
    // Close the file.
    my_file.close();
}

/*
   Returns the id of the class that is going to be placed in the first
   position of the permutation.
   In order to choose the first element, we simply choose the element
   that requires more production.
*/
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

/*
   Returns the penalty that adds up to the total penalty when adding
   class with id car into the permutation.
*/
int computePenalty(vector<int>& permutation, Input& I, int car)
{
    // The penalty will be the sum of the penalties for each upgrade.
    int assigned = permutation.size();
    int total_penalty = 0;
    for (int i = 0; i < I.M; ++i) {
        // First we compute the maximum size of the windows that we will treat.
        int max_window = min(I.n_e[i], assigned + 1);
        /*
           We have to take into account all the possible windows that can
           generate a penalty (without regarding future schedulings).
        */
        int j = 1;
        for (int size = max_window; size > I.c_e[i]; --size) {
            int partial_penalty = -I.c_e[i] + I.upgr[car][i];
            for (int pos = assigned - size + 1; pos < assigned; ++pos) {
                // If it requires that upgrade, then increase penalty.
                if (I.upgr[permutation[pos]][i])
                    ++partial_penalty;
            }
            /*
               If partial penalty is negative, no new penalty has been added.
               If it's bigger than 0, we need to add that up.
            */
            total_penalty += max(0, partial_penalty);
            ++j;
        }
    }
    return total_penalty;
}

void generatePerm(Input& I, vector<int>& permutation, vector<int>& prod, int K)
{
    // We choose the first class from the schedule.
    int first_class = chooseFirstElement(permutation, prod, I.K);

    // Update variables according to the choice.
    permutation.push_back(first_class);
    --prod[first_class];

    // We choose a class for each remaining position of the schedule.
    for (int pos = 1; pos < I.C; ++pos) {
        int min_penalty = INT_MAX;
        int best_class = -1;

        /*
           We compare the penalty that would generate scheduling
           each class to the actual position.
        */
        for (int car = 0; car < I.K; ++car) {
            // If there are no more cars to produce, skip.
            if (prod[car] == 0)
                continue;
            /*
               We compute the penalty that this class would generate
               given the previous assignments.
            */
            int penalty_class = computePenalty(permutation, I, car);
            int actual_class = car;

            /*
               We update the variables in case it is necessary that is when
               we find a class which minimizes the penalty even more.
            */
            if (penalty_class < min_penalty) {
                min_penalty = penalty_class;
                best_class = actual_class;
            }
            // In case of a tie we choose the one with higher production.
            else if (penalty_class == min_penalty and prod[actual_class] > prod[best_class]) {
                min_penalty = penalty_class;
                best_class = actual_class;
            }
        }

        // We assign the actual position to the best class found.
        permutation.push_back(best_class);
        --prod[best_class];
    }
}

/*
   Given a permutation, calculates the total penalty of that permutation.
*/
int countPenalty(Input& I, Sol& S)
{
    const auto& [C, M, K, c_e, n_e, prod, upgr] = I;
    auto& [penalty, permutation, req] = S;
    int count = 0;

    // For every station (upgrade) :
    for (int j = 0; j < M; ++j) {
        int n_j = n_e[j];
        int c_j = c_e[j];
        int pen = -c_j;

        // For every car :
        for (int k = 0; k < C; ++k) {

            // Update penalties.
            pen += req[j][k];
            if (k >= n_j)
                pen -= req[j][k - n_j];
            if (pen > 0)
                count += pen;
        }

        /*
           Count penalty of the last windows.
           Note that these are only the incomplete windows at the end of
           each permutation.
        */
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

    // Greedy algorithm -> Generates a decent schedule.
    generatePerm(I, permutation, prod, K);

    // Prepare what is needed to count the final penalty.
    S.req = buildReq(S, I);
    penalty = countPenalty(I, S);

    // Write the greedy answer into the given file.
    writeIntoFile(f_o, permutation, penalty);
}

int main(int argc, char** argv)
{
    // Read the input ib the given format.
    string f_i = argv[1];
    string f_o = argv[2];
    Sol S;
    Input I = readInput(f_i, S);

    // Compute the greedy answer.
    greedy(S, I, f_o);
}
