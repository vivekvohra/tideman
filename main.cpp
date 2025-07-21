// tideman_oop.cpp
#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

// Constants
const int MAX = 9;

// Utility class for benchmarking
class Benchmark {
public:
    template<typename Func>
    static void time(const string& label, Func f) {
        auto start = high_resolution_clock::now();
        f();
        auto end = high_resolution_clock::now();
        cout << label << " took "
             << duration_cast<microseconds>(end - start).count()
             << " µs\n";
    }
};

// Core Tideman Election Logic
class TidemanElection {
private:
    int candidate_count;
    vector<string> candidates;
    vector<vector<int>> preferences;
    vector<vector<bool>> locked;
    vector<pair<int, int>> pairs;

    bool has_cycle(int start, int end) {
        if (locked[end][start]) return true;
        for (int i = 0; i < candidate_count; i++) {
            if (locked[end][i] && has_cycle(start, i)) return true;
        }
        return false;
    }

    void sort_pairs() {
        sort(pairs.begin(), pairs.end(), [&](const auto& a, const auto& b) {
            return preferences[a.first][a.second] > preferences[b.first][b.second];
        });
    }

public:
    TidemanElection(const vector<string>& names) : candidates(names) {
        candidate_count = names.size();
        if (candidate_count > MAX) throw invalid_argument("Too many candidates");
        preferences.assign(candidate_count, vector<int>(candidate_count, 0));
        locked.assign(candidate_count, vector<bool>(candidate_count, false));
    }

    void record_vote(const vector<int>& ranks) {
        for (int i = 0; i < candidate_count; i++) {
            for (int j = i + 1; j < candidate_count; j++) {
                preferences[ranks[i]][ranks[j]]++;
            }
        }
    }

    void add_pairs() {
        for (int i = 0; i < candidate_count; i++) {
            for (int j = 0; j < candidate_count; j++) {
                if (i != j && preferences[i][j] > preferences[j][i]) {
                    pairs.emplace_back(i, j);
                }
            }
        }
    }

    void lock_pairs() {
        for (const auto& p : pairs) {
            if (!has_cycle(p.first, p.second)) {
                locked[p.first][p.second] = true;
            }
        }
    }

    string get_winner() {
        for (int i = 0; i < candidate_count; i++) {
            bool is_source = true;
            for (int j = 0; j < candidate_count; j++) {
                if (locked[j][i]) {
                    is_source = false;
                    break;
                }
            }
            if (is_source) return candidates[i];
        }
        return "No winner";
    }
};

// Vote Parser
class VoteParser {
public:
    static vector<vector<int>> parse_votes(const string& filename, vector<string>& out_candidates) {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Could not open file: " + filename);

        string line;
        vector<vector<int>> votes;

        // First line: header with candidate names
        getline(file, line);
        stringstream header(line);
        string cell;
        getline(header, cell, ','); // Skip VoterID
        unordered_map<string, int> name_to_index;

        while (getline(header, cell, ',')) {
            out_candidates.push_back(cell);
            name_to_index[cell] = out_candidates.size() - 1;
        }

        // Read each voter line
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, cell, ','); // Skip VoterID
            vector<int> rank(out_candidates.size());
            int idx = 0;
            while (getline(ss, cell, ',')) {
                int pos = stoi(cell);
                rank[pos - 1] = idx++;
            }
            vector<int> ranks;
            ranks.resize(out_candidates.size());
            for (int i = 0; i < out_candidates.size(); ++i) {
                ranks[rank[i]] = i;
            }
            votes.push_back(ranks);
        }

        return votes;
    }
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) throw invalid_argument("Usage: ./tideman_oop --file=ballots.csv");
        string filename;

        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            if (arg.rfind("--file=", 0) == 0) {
                filename = arg.substr(7);
            }
        }

        if (filename.empty()) throw invalid_argument("Missing --file flag");

        vector<string> candidates;
        auto votes = VoteParser::parse_votes(filename, candidates);

        TidemanElection election(candidates);

        Benchmark::time("Recording Preferences", [&]() {
            for (const auto& vote : votes)
                election.record_vote(vote);
        });

        Benchmark::time("Adding Pairs", [&]() {
            election.add_pairs();
        });

        Benchmark::time("Locking Pairs", [&]() {
            election.lock_pairs();
        });

        Benchmark::time("Getting Winner", [&]() {
            cout << "Winner: " << election.get_winner() << "\n";
        });

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
