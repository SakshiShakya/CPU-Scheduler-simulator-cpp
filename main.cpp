// CPU Scheduling Simulation in C++
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    int priority;
    int start;
    int finish;
    int remaining;
    int response;
    bool started = false;
};

void calculateMetrics(vector<Process>& p) {
    double totalTAT = 0, totalWT = 0, totalRT = 0;
    for (auto& proc : p) {
        int tat = proc.finish - proc.arrival;
        int wt = tat - proc.burst;
        totalTAT += tat;
        totalWT += wt;
        totalRT += proc.response - proc.arrival;
    }
    int n = p.size();
    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time: " << totalTAT / n;
    cout << "\nAverage Waiting Time: " << totalWT / n;
    cout << "\nAverage Response Time: " << totalRT / n << endl;
}

void FCFS(vector<Process> p) {
    cout << "\n--- FCFS Scheduling ---" << endl;
    sort(p.begin(), p.end(), [](auto& a, auto& b) { return a.arrival < b.arrival; });
    int time = 0;
    for (auto& proc : p) {
        if (time < proc.arrival) time = proc.arrival;
        proc.start = time;
        proc.response = time;
        time += proc.burst;
        proc.finish = time;
        cout << "P" << proc.pid << " [" << proc.start << "-" << proc.finish << "] ";
    }
    calculateMetrics(p);
}

void SJF_NonPreemptive(vector<Process> p) {
    cout << "\n--- SJF Non-Preemptive Scheduling ---" << endl;
    int time = 0;
    vector<Process> done;
    while (!p.empty()) {
        vector<Process>::iterator shortest = p.end();
        for (auto it = p.begin(); it != p.end(); ++it) {
            if (it->arrival <= time && (shortest == p.end() || it->burst < shortest->burst)) {
                shortest = it;
            }
        }
        if (shortest == p.end()) { time++; continue; }
        Process proc = *shortest;
        p.erase(shortest);
        proc.start = time;
        proc.response = time;
        time += proc.burst;
        proc.finish = time;
        done.push_back(proc);
        cout << "P" << proc.pid << " [" << proc.start << "-" << proc.finish << "] ";
    }
    calculateMetrics(done);
}

void SJF_Preemptive(vector<Process> p) {
    cout << "\n--- SJF Preemptive (SRTF) Scheduling ---" << endl;
    int time = 0, n = p.size();
    vector<Process> done;
    for (auto& proc : p) proc.remaining = proc.burst;

    while (done.size() < n) {
        int idx = -1, minBurst = 1e9;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0 && p[i].remaining < minBurst) {
                idx = i;
                minBurst = p[i].remaining;
            }
        }
        if (idx == -1) { time++; continue; }

        if (!p[idx].started) {
            p[idx].start = time;
            p[idx].response = time;
            p[idx].started = true;
        }

        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            p[idx].finish = time;
            done.push_back(p[idx]);
            cout << "P" << p[idx].pid << " [" << p[idx].start << "-" << p[idx].finish << "] ";
        }
    }
    calculateMetrics(done);
}

void Priority_NonPreemptive(vector<Process> p) {
    cout << "\n--- Priority Non-Preemptive Scheduling ---" << endl;
    int time = 0;
    vector<Process> done;
    while (!p.empty()) {
        vector<Process>::iterator highest = p.end();
        for (auto it = p.begin(); it != p.end(); ++it) {
            if (it->arrival <= time && (highest == p.end() || it->priority < highest->priority)) {
                highest = it;
            }
        }
        if (highest == p.end()) { time++; continue; }
        Process proc = *highest;
        p.erase(highest);
        proc.start = time;
        proc.response = time;
        time += proc.burst;
        proc.finish = time;
        done.push_back(proc);
        cout << "P" << proc.pid << " [" << proc.start << "-" << proc.finish << "] ";
    }
    calculateMetrics(done);
}

void Priority_Preemptive(vector<Process> p) {
    cout << "\n--- Priority Preemptive Scheduling ---" << endl;
    int time = 0, n = p.size();
    vector<Process> done;
    for (auto& proc : p) proc.remaining = proc.burst;

    while (done.size() < n) {
        int idx = -1, high = 1e9;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0 && p[i].priority < high) {
                high = p[i].priority;
                idx = i;
            }
        }
        if (idx == -1) { time++; continue; }

        if (!p[idx].started) {
            p[idx].start = time;
            p[idx].response = time;
            p[idx].started = true;
        }

        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            p[idx].finish = time;
            done.push_back(p[idx]);
            cout << "P" << p[idx].pid << " [" << p[idx].start << "-" << p[idx].finish << "] ";
        }
    }
    calculateMetrics(done);
}

void RoundRobin(vector<Process> p, int quantum) {
    cout << "\n--- Round Robin Scheduling ---" << endl;
    queue<int> q;
    int time = 0, n = p.size();
    for (auto& proc : p) proc.remaining = proc.burst;
    vector<bool> inQueue(n, false);
    vector<Process> done;

    while (done.size() < n) {
        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrival <= time && p[i].remaining > 0) {
                q.push(i);
                inQueue[i] = true;
            }
        }
        if (q.empty()) { time++; continue; }

        int idx = q.front(); q.pop();
        inQueue[idx] = false;

        if (!p[idx].started) {
            p[idx].start = time;
            p[idx].response = time;
            p[idx].started = true;
        }

        int exec = min(quantum, p[idx].remaining);
        time += exec;
        p[idx].remaining -= exec;

        cout << "P" << p[idx].pid << " [" << time - exec << "-" << time << "] ";

        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrival <= time && p[i].remaining > 0)
                q.push(i), inQueue[i] = true;
        }
        if (p[idx].remaining > 0) q.push(idx), inQueue[idx] = true;
        else { p[idx].finish = time; done.push_back(p[idx]); }
    }
    calculateMetrics(done);
}

int main() {
    int n, quantum;
    cout << "Enter number of processes: ";
    cin >> n;
    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        cout << "Process " << i+1 << " (Arrival Burst Priority): ";
        cin >> processes[i].arrival >> processes[i].burst >> processes[i].priority;
    }
    cout << "Enter time quantum (for Round Robin): ";
    cin >> quantum;

    FCFS(processes);
    SJF_NonPreemptive(processes);
    SJF_Preemptive(processes);
    Priority_NonPreemptive(processes);
    Priority_Preemptive(processes);
    RoundRobin(processes, quantum);

    return 0;
}
