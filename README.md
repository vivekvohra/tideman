#  Tideman Electoral System

Tideman’s **Ranked Pairs method** (developed by Nicolaus Tideman) is a **ranked-choice voting algorithm** that selects a single winner by constructing a directed graph of pairwise victories and locking in the strongest preferences while **avoiding cycles**.

This repository contains an **object-oriented, benchmarked, file-driven C++17 implementation**, wrapped with a **Flask API** and a **minimal HTML front end** for interactive demos.



##  Features

*  **CSV input** with support for thousands to millions of ballots
*  **High-resolution benchmarking** using `<chrono>` for performance profiling
*  **Object-oriented design** (`TidemanElection`, `VoteParser`, `Benchmark`)
*  **Robust error handling** (bad input, duplicates, file issues)
*  **Efficient in practice**: runs on millions of ballots with ≤9 candidates in milliseconds
*  **Web service wrapper**: C++ executable exposed via Flask API + HTML page



## Sample Input (CSV Ballots)

Example with 3 candidates (`Alice`, `Bob`, `Charlie`):

```csv
VoterID,Alice,Bob,Charlie
1,1,2,3
2,2,1,3
3,1,3,2
... up to N rows
```

Each row = one voter’s ranked preferences.
Header defines candidate names; numbers = rank order (`1 = top choice`).



##  Sample Output (Benchmark Run)

```text
Recording Preferences took 32109 µs
Adding Pairs took 0 µs
Locking Pairs took 0 µs
Winner: Charlie
Getting Winner took 999 µs
```

Handles **1,000,000 votes** in milliseconds, showing the practicality of the O(n³) algorithm with small `n`.


##  Winner Logic

* Build preference matrix from all ballots
* Sort candidate pairs by margin of victory
* Lock edges **unless they create a cycle**
* Candidate with no incoming edge = **winner**

In the demo below, the Condorcet winner was **Charlie**.



##  Demo (API + Web UI)


https://github.com/user-attachments/assets/54d860ae-3cfd-4674-9f14-4fe466689a97



<details>
<summary> How the demo works</summary>

1. Upload `ballots.csv` from the HTML page
2. Flask API sends CSV to C++ binary (`tideman.exe`)
3. Winner + benchmark timings returned as JSON
4. UI displays winner (green badge) + raw API response

</details>


##  How to Run

### Compile (C++ core)

```bash
g++ -std=c++17 -O2 tideman.cpp -o tideman
```

###  Run directly with CSV

```bash
./tideman --file=ballots.csv
```

###  Or start Flask API (wrapper)

```bash
pip install flask flask-cors
python app.py
```

Then open `index.html` (via VS Code Live Server or browser) and upload ballots.



##  Tech Stack

* **Core**: C++17, STL, DFS cycle detection, chrono benchmarking
* **API**: Python Flask, subprocess IPC, JSON responses
* **UI**: HTML, fetch API, simple file upload

---


