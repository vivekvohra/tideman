// tideman_oop.cpp
#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

// Constants
const int MAX = 9; // Only used for a friendly warning; vectors are dynamically sized

// ----------------------------------------------------------------------------
// Utility: tiny benchmark helper (handy during local testing)
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// Core Tideman (Ranked Pairs) election logic
//
// preferences[i][j] = #voters who prefer candidate i over candidate j
// locked[i][j]      = directed edge i -> j chosen when locking pairs
// pairs             = (winner, loser) pairs sorted by strength
// ----------------------------------------------------------------------------
class TidemanElection {
private:
    int candidate_count;
    vector<string> candidates;
    vector<vector<int>> preferences;
    vector<vector<bool>> locked;
    vector<pair<int, int>> pairs;

    // Strength of victory (margin)
    int margin(int w, int l) const {
        return preferences[w][l] - preferences[l][w];
    }

    // Would adding edge u -> v create a cycle in the current locked graph?
    // We check whether a path v ... -> u already exists.
    bool creates_cycle(int u, int v) {
        vector<int> stack = {v};
        vector<bool> seen(candidate_count, false);
        while (!stack.empty()) {
            int x = stack.back(); stack.pop_back();
            if (x == u) return true; // found path v..->u, so u->v would close a cycle
            for (int nxt = 0; nxt < candidate_count; ++nxt) {
                if (locked[x][nxt] && !seen[nxt]) {
                    seen[nxt] = true;
                    stack.push_back(nxt);
                }
            }
        }
        return false;
    }

    // Sort pairs by:
    // 1) descending margin of victory
    // 2) descending raw winner->loser vote count
    // 3) deterministic tie-break on candidate indices
    void sort_pairs() {
        // stable_sort is fine; with our explicit tiebreakers stability doesn’t matter, but it’s harmless.
        stable_sort(pairs.begin(), pairs.end(), [&](const pair<int,int>& A, const pair<int,int>& B) {
            int aw = A.first, al = A.second;
            int bw = B.first, bl = B.second;

            int mA = margin(aw, al);
            int mB = margin(bw, bl);
            if (mA != mB) return mA > mB;

            int votesA = preferences[aw][al];
            int votesB = preferences[bw][bl];
            if (votesA != votesB) return votesA > votesB;

            // Deterministic tie-break: by indices
            if (aw != bw) return aw < bw;
            return al < bl;
        });
    }


public:
    explicit TidemanElection(const vector<string>& names) : candidates(names) {
        candidate_count = (int)names.size();
        if (candidate_count > MAX) {
            cerr << "[warn] candidate_count > MAX(" << MAX << "); vectors still handle it.\n";
        }
        preferences.assign(candidate_count, vector<int>(candidate_count, 0));
        locked.assign(candidate_count, vector<bool>(candidate_count, false));
    }

    // ranks[k] = candidate index at rank position k (k=0 is top choice)
    void record_vote(const vector<int>& ranks) {
        for (int i = 0; i < candidate_count; i++) {
            for (int j = i + 1; j < candidate_count; j++) {
                preferences[ranks[i]][ranks[j]]++;
            }
        }
    }

    void add_pairs() {
        pairs.clear();
        for (int i = 0; i < candidate_count; i++) {
            for (int j = 0; j < candidate_count; j++) if (i != j) {
                if (preferences[i][j] > preferences[j][i]) {
                    pairs.emplace_back(i, j);
                }
            }
        }
        sort_pairs();
    }

    void lock_pairs() {
        for (const auto& p : pairs) {
            int w = p.first, l = p.second;
            if (!creates_cycle(w, l)) {
                locked[w][l] = true;
            }
        }
    }

    string get_winner() const {
        // Source node: no incoming edges
        for (int i = 0; i < candidate_count; i++) {
            bool is_source = true;
            for (int j = 0; j < candidate_count; j++) {
                if (locked[j][i]) { is_source = false; break; }
            }
            if (is_source) return candidates[i];
        }
        return "No winner";
    }

    // Optional getters for debugging/visualization
    const auto& get_preferences() const { return preferences; }
    const auto& get_locked() const { return locked; }
    const auto& get_pairs() const { return pairs; }
};

// ----------------------------------------------------------------------------
// CSV Vote Parser
// Expected CSV format:
//   VoterID, Alice, Bob, Charlie, ...
//   101,     2,     1,   3
// where each number is the RANK assigned to that column’s candidate.
// Example above: Bob=1st, Alice=2nd, Charlie=3rd
//
// We produce for each voter a vector<int> rank_of_pos where:
//   rank_of_pos[0] == candidate index of top choice
//   rank_of_pos[1] == candidate index of second choice
//   ...
// i.e., index = rank position, value = candidate index
// ----------------------------------------------------------------------------
class VoteParser {
    // small trimming helper
    static void trim_inplace(string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        size_t b = s.find_last_not_of(" \t\r\n");
        s = (a == string::npos) ? "" : s.substr(a, b - a + 1);
    }

public:
    static vector<vector<int>> parse_votes(const string& filename, vector<string>& out_candidates) {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Could not open file: " + filename);

        string line, cell;
        vector<vector<int>> votes;

        // --- Parse header: first cell is "VoterID", rest are candidate names in order
        if (!getline(file, line)) {
            throw runtime_error("Empty CSV file: " + filename);
        }
        {
            stringstream header(line);
            // Skip VoterID header cell
            if (!getline(header, cell, ',')) {
                throw runtime_error("CSV header missing VoterID column");
            }
            while (getline(header, cell, ',')) {
                trim_inplace(cell);
                if (!cell.empty()) out_candidates.push_back(cell);
            }
            if (out_candidates.empty()) {
                throw runtime_error("No candidate names found in header");
            }
        }

        const int C = (int)out_candidates.size();

        // --- Parse each voter row
        while (getline(file, line)) {
            // Skip empty/whitespace-only lines
            if (line.find_first_not_of(" \t\r\n") == string::npos) continue;

            stringstream ss(line);

            // Skip VoterID value
            if (!getline(ss, cell, ',')) continue;

            vector<int> rank(C, -1);     // rank[pos] = candidate index at that rank position (0-based)
            vector<bool> seen_pos(C, 0); // to validate duplicates in rank positions
            int idx = 0;                 // idx = candidate index in header order

            while (idx < C && getline(ss, cell, ',')) {
                trim_inplace(cell);
                if (cell.empty()) { idx++; continue; } // tolerate blank, but keeps slot for this candidate

                int pos = stoi(cell); // expected 1..C
                if (pos < 1 || pos > C) {
                    throw runtime_error("Rank position out of range in row: " + line);
                }
                if (seen_pos[pos - 1]) {
                    throw runtime_error("Duplicate rank position in row: " + line);
                }
                rank[pos - 1] = idx;        // *** FIX: directly map rank position -> candidate index
                seen_pos[pos - 1] = true;
                idx++;
            }

            // If row had fewer cells than candidates, consume missing cells (treated as blanks)
            while (idx < C) { idx++; }

            // Basic validation: all rank positions should be assigned a candidate index
            for (int p = 0; p < C; ++p) {
                if (rank[p] == -1) {
                    throw runtime_error("Incomplete ranking row (missing positions) in row: " + line);
                }
            }

            votes.push_back(std::move(rank));
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
            for (const auto& vote : votes) {
                election.record_vote(vote);
            }
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

        // Optional: print pairs and margins for debugging
        /*
        for (auto [w, l] : election.get_pairs()) {
            cout << candidates[w] << " > " << candidates[l]
                 << " (margin " << (int)(election.get_preferences()[w][l] - election.get_preferences()[l][w]) << ")\n";
        }
        */

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
