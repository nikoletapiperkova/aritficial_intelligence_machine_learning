#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <limits>
#include <chrono>
#include <iomanip>


using namespace std;
using vi = vector<int>;
//using clock_t = chrono::high_resolution_clock;

int conflicts_for(const vi& queens, int col, int row) {
    int n = queens.size();
    int conflicts = 0;
    for (int j = 0; j < n; ++j) {
        if (j == col) {
            continue;
        }
        int current = queens[j];
        if (current == row) {
            conflicts++; 
        }                      
        if (abs(current - row) == abs(j - col)) {
            conflicts++; 
        } 
    }
    return conflicts;
}

int total_conflicts(const vi& queens) {
    int n = queens.size();
    int sum = 0;
    for (int c = 0; c < n; ++c)
        sum += conflicts_for(queens, c, queens[c]);
    return sum / 2; 
}

bool min_conflicts_once(int n, int max_steps, vi& solution, mt19937& rng) {
    uniform_int_distribution<int> rowDist(0, n - 1);

    vi queens(n);
    for (int c = 0; c < n; ++c) queens[c] = rowDist(rng);

    for (int step = 0; step < max_steps; ++step) {
        if (total_conflicts(queens) == 0) {
            solution = queens;
            return true;
        }

        
        vector<int> colConf(n);
        int maxConf = 0;
        for (int c = 0; c < n; ++c) {
            colConf[c] = conflicts_for(queens, c, queens[c]);
            if (colConf[c] > maxConf) maxConf = colConf[c];
        }

        
        vector<int> candCols;
        for (int c = 0; c < n; ++c)    if (colConf[c] == maxConf) candCols.push_back(c);
            uniform_int_distribution<int> pickColDist(0, (int)candCols.size() - 1);
            int chosenCol = candCols[pickColDist(rng)];
      

      
        int bestConf = numeric_limits<int>::max();
        vector<int> bestRows;
        for (int r = 0; r < n; ++r) {
            int cfs = conflicts_for(queens, chosenCol, r);
            if (cfs < bestConf) {
                bestConf = cfs;
                bestRows.clear();
                bestRows.push_back(r);
            }
            else if (cfs == bestConf) {
                bestRows.push_back(r);
            }
        }

        uniform_int_distribution<int> pickRowDist(0, (int)bestRows.size() - 1);
        int chosenRow = bestRows[pickRowDist(rng)];

        queens[chosenCol] = chosenRow;
    }

    return false;
}

void print_solution(const vi& sol) {
    cout << "[";
    for (size_t i = 0; i < sol.size(); ++i) {
        cout << sol[i];
        if (i + 1 < sol.size()) cout << ", ";
    }
    cout << "]\n";
}


void print_board(const vi& sol) {
    int n = (int)sol.size();
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (sol[c] == r) cout << "*";
            else cout << "_";
            if (c + 1 < n) cout << " ";
        }
        cout << "\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    bool boardFlag = 0;
    bool timeFlag = 0;

    int n = 8;
    int k = 100; 
    if (!(cin >> n)) return 0;
    

    if (n <= 3) {
        cout << "-1\n";
        return 1; 
    }

    int max_steps = k * n;
    int max_restarts = 50;

    //-----------
 
      //да оправя входа за таговете
    //-------------------------

  
    mt19937 rng((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());

    auto t0 = chrono::high_resolution_clock::now();

    vi solution;
    bool found = false;
    for (int attempt = 0; attempt < max_restarts && !found; ++attempt) {
        if (min_conflicts_once(n, max_steps, solution, rng)) {
            found = true;
            break;
        }
    }

    auto t1 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = t1 - t0;

    if (!found) {
        cout << "NO_SOLUTION\n";
        return 0;
    }

    const int BOARD_PRINT_LIMIT = 200; 
    const bool PRINT_BOARD = (n <= BOARD_PRINT_LIMIT);

    if (PRINT_BOARD) {
        print_solution(solution);
        if (boardFlag) { print_board(solution); }
        if (timeFlag){
            cout << fixed << setprecision(6);
            cout << "time_seconds: " << elapsed.count() << '\n';
        }
    }

    else {
        cout << fixed << setprecision(6);
        cout << "time_seconds: " << elapsed.count() << '\n';
    }
    

    return 0;
}
