/* Input Format 
Enter the number of candidates: 3
Candidate 1: Alice
Candidate 2: Charlie
Candidate 3: Bob
Number of voters: 2
Rank 1: Alice
Rank 2: Charlie
Rank 3: Bob
Rank 1: Charlie
Rank 2: Alice
Rank 3: Bob

*/
#include <bits/stdc++.h>
using namespace std;
const int MAX = 9;
// Structure to represent a pair of candidates
struct CandidatePair {
    int winner;
    int loser;
};

// Global variables using STL containers
vector<vector<int>> preferences;      // Number of voters who prefer i over j
vector<vector<bool>> locked;          // Locked[i][j] means i is locked over j
vector<string> candidates;            // List of candidate names
vector<CandidatePair> pairs;          // List of candidate pairs
int candidate_count;                  // Number of candidates

// Function declarations
bool vote(int rank, const string& name, vector<int>& ranks);
void record_preferences(const vector<int>& ranks);
void add_pairs();
void sort_pairs();
void lock_pairs();
bool has_cycle(int cycle_start, int cycle_end);
void print_winner();

int main(int argc, char* argv[]) {
  // *******input start ************
 // Prompt user for the number of candidates
 cout << "Enter the number of candidates: ";
 cin >> candidate_count;

 // Check for invalid input
 if (candidate_count < 1) {
     cout << "Error: At least one candidate is required.\n";
     return 1;
 }
 if (candidate_count > MAX) {
     cout << "Maximum number of candidates is " << MAX << "\n";
     return 2;
 }

 // Populate candidates from user input
 candidates.reserve(candidate_count);
 cout << "Enter the names of the candidates:\n";
 for (int i = 0; i < candidate_count; i++) {
     string name;
     cout << "Candidate " << (i + 1) << ": ";
     cin >> name;
     candidates.emplace_back(name);
 }

// *******input end ************

    // Initialize preferences and locked graphs
    preferences.assign(candidate_count, vector<int>(candidate_count, 0));
    locked.assign(candidate_count, vector<bool>(candidate_count, false));

    // Get number of voters
    int voter_count;
    cout << "Number of voters: ";
    cin >> voter_count;

    // Process each voter's preferences
    for (int i = 0; i < voter_count; i++) {
        vector<int> ranks(candidate_count);
        for (int j = 0; j < candidate_count; j++) {
            string name;
            cout << "Rank " << j + 1 << ": ";
            cin >> name;
            if (!vote(j, name, ranks)) {
                cout << "Invalid vote.\n";
                return 3;
            }
        }
        record_preferences(ranks);
        cout << "\n";
    }

    // Compute and output the winner
    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();

    return 0;
}

// Record a voter's preference for a candidate at a given rank
bool vote(int rank, const string& name, vector<int>& ranks) {
    for (int i = 0; i < candidate_count; i++) {
        if (name == candidates[i]) {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update the preferences matrix based on a voter's ranks
void record_preferences(const vector<int>& ranks) {
    for (int i = 0; i < candidate_count; i++) {
        for (int j = i + 1; j < candidate_count; j++) {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
}

// Create pairs of candidates based on preference strength
void add_pairs() {
    for (int i = 0; i < candidate_count; i++) {
        for (int j = i + 1; j < candidate_count; j++) {
            if (preferences[i][j] > preferences[j][i]) {
                pairs.push_back({i, j});
            } else if (preferences[i][j] < preferences[j][i]) {
                pairs.push_back({j, i});
            }
        }
    }
}

// Sort pairs by strength of victory (descending order)
void sort_pairs() {
    sort(pairs.begin(), pairs.end(), [](const CandidatePair& a, const CandidatePair& b) {
        return preferences[a.winner][a.loser] > preferences[b.winner][b.loser];
    });
}

// Check if adding an edge creates a cycle in the graph
bool has_cycle(int cycle_start, int cycle_end) {
    if (locked[cycle_end][cycle_start]) {
        return true;
    }
    for (int i = 0; i < candidate_count; i++) {
        if (locked[cycle_end][i] && has_cycle(cycle_start, i)) {
            return true;
        }
    }
    return false;
}

// Lock pairs into the graph, avoiding cycles
void lock_pairs() {
    for (const auto& p : pairs) {
        if (!has_cycle(p.winner, p.loser)) {
            locked[p.winner][p.loser] = true;
        }
    }
}

// Print the winner(s) - candidate(s) with no incoming edges
void print_winner() {
    for (int i = 0; i < candidate_count; i++) {
        bool is_source = true;
        for (int j = 0; j < candidate_count; j++) {
            if (j != i && locked[j][i]) {
                is_source = false;
                break;
            }
        }
        if (is_source) {
            cout << candidates[i] << endl;
        }
    }
}
