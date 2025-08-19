## 🗳️ Tideman Electoral System

Tideman's Alternative Method is a **ranked-choice voting system** developed by Nicolaus Tideman. It selects a **single winner** using ballots where voters rank candidates in order of preference. The algorithm constructs a directed graph based on pairwise victories and locks in the strongest preferences while avoiding cycles.

This repository contains an **object-oriented, benchmarked, and file-driven C++ implementation** of the Tideman algorithm.

---

## ⚙️ Features of This Implementation

- 🔁 Input from CSV files with thousands to millions of votes
- ⏱️ `chrono`-based benchmarking for performance measurement
- 🧱 Object-oriented structure using C++17 (TidemanElection, VoteParser, Benchmark)
- 🧾 Exception handling for bad inputs or file issues
- ✅ Efficient for real-world scenarios with small candidate count (≤9)

---

## 📥 Sample Input (1 Million Votes)

The program was tested on a randomly generated CSV file with **1,000,000 votes** and **3 candidates**: `Alice`, `Bob`, and `Charlie`.

Each vote ranked candidates uniquely. Example format:

```csv
VoterID,Alice,Bob,Charlie
1,1,2,3
2,2,1,3
3,1,3,2
... (up to 1,000,000 rows)
```

We use 1 million votes, each ranked in patterns **similar to the example below**:

<img width="831" alt="condorcet_1" src="https://github.com/vivekvohra/tideman/assets/112391833/7ded3ee3-8393-496f-ba73-ce2bcd7f12b6">

---

## 💻 Sample Output (Benchmark Results)

```text
Recording Preferences took 32109 us
Adding Pairs took 0 us
Locking Pairs took 0 us
Winner: Charlie
Getting Winner took 999 us
```

✅ The program processed 1 million ballots in just milliseconds, proving the practicality of the O(n³) algorithm when `n` (candidates) is small.

---

## 🧠 Winner Logic

In this run, the algorithm determined that **Charlie** was the winner. This depends on the majority preferences between each pair of candidates.


---

## 🧪 How to Run

### 🛠️ Compile
```bash
g++ -std=c++17 -O2 tideman.cpp -o tideman
```

### ▶️ Execute with CSV Input
```bash
./tideman --file=ballots_1M_cpp.csv
```

