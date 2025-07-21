#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

using namespace std;

int main() {
    const vector<string> candidates = {"Alice", "Bob", "Charlie"};
    const int num_voters = 1'000'000;
    const string filename = "ballots_1M_cpp.csv";

    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for writing.\n";
        return 1;
    }

    // Write CSV header
    file << "VoterID";
    for (const string& name : candidates) {
        file << "," << name;
    }
    file << "\n";

    // Random engine
    random_device rd;
    mt19937 g(rd());
    vector<int> ranks = {1, 2, 3};

    // Generate and write each vote
    for (int voter_id = 1; voter_id <= num_voters; ++voter_id) {
        shuffle(ranks.begin(), ranks.end(), g);
        file << voter_id;
        for (int rank : ranks) {
            file << "," << rank;
        }
        file << "\n";
    }

    file.close();
    cout << "✅ CSV file generated: " << filename << "\n";
    return 0;
}
