#  Tideman Electoral System

Tideman’s **Ranked Pairs method** (developed by Nicolaus Tideman) is a **ranked-choice voting algorithm** that selects a single winner by constructing a directed graph of pairwise victories and locking in the strongest preferences while **avoiding cycles**.

This repository provides an **object-oriented, benchmarked, file-driven C++17 implementation**, wrapped by a **Flask API** and a **minimal HTML front end**. It’s now **Dockerized** (multi-stage) and **EC2-ready** with **Gunicorn** for a production-style run.



##  Features

* **CSV input** with support from thousands up to **millions of ballots**
* **High-resolution benchmarking** using `<chrono>`
* **Object-oriented design** (`TidemanElection`, `VoteParser`, `Benchmark`)
* **Robust error handling** (bad input, duplicates, file issues)
* **Efficient in practice**: millions of ballots with ≤9 candidates in milliseconds
* **Web service wrapper**: C++ executable exposed via **Flask API** + **HTML**
* **Containerized**: Multi-stage **Dockerfile**, **Gunicorn** server, **EC2-ready** (port 80 → 5000)



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




## Deploy on AWS EC2 (Ubuntu)

1. **Launch EC2** (Ubuntu), open **Security Group** inbound rules:

   * **22** (SSH)
   * **80** (HTTP)

2. **Install Docker + Git**:

```bash
sudo apt update
sudo apt install -y docker.io git
```

3. **Clone & build**:

```bash
git clone <your-repo-url>
cd <repo>
sudo docker build -t tideman:latest .
```

4. **Run container**:

```bash
sudo docker run -d --rm -p 80:5000 --name tideman tideman:latest
```

5. **Test**:

   * On EC2: `curl http://localhost/healthz`
   * From your browser: `http://<EC2-Public-IP>` (port 80 → 5000)

---


