/*
Student Name: Enis Simsar
Student Number: 2014400219
Project Number: 2
Operating System: Xubuntu (given VM)
Compile Status: Working
Program Status: Working
Notes:
*/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

using namespace std;

struct Task {
    int cpu_id = 0;
    int io_id = 0;
    string type;
    double activeTime;
    double arrivalTime;
    double ioDuration;
    double cpuDuration;
    double globalTime;
    Task(string type_, double arrivalTime_, double cpuDuration_, double ioDuration_){
        type = type_;
        arrivalTime = arrivalTime_;
        cpuDuration = cpuDuration_;
        ioDuration = ioDuration_;
        globalTime = arrivalTime_;
        activeTime = 0;
    }
    void performIO(double ioQuantumNum, int id){
        if(ioQuantumNum < ioDuration){
            io_id = id;
            activeTime += ioQuantumNum;
            ioDuration -= ioQuantumNum;
            globalTime += ioQuantumNum;
            type = "inIO";
        }else{
            io_id = id;
            globalTime += ioDuration;
            activeTime += ioDuration;
            type = "finish";
        }
    }
    void performCPU(double frequency, int id){
        cpu_id = id;
        cpuDuration /= frequency;
        activeTime += cpuDuration;
        globalTime += cpuDuration;
        type = "cpuDeparture";
    }
};

struct CPU{
    double frequency, activeTime;
    int id;
    Task* temp = 0;
    CPU(double frequency_, int id_) {
        this->frequency = frequency_;
        activeTime = 0;
        this->id = id_;
    }
    void incrementActiveTime(double time){
        activeTime += time;
    }
    void workInCPU(Task &a){
        temp = &a;
        temp->performCPU(this->frequency, this->id);
        incrementActiveTime(temp->cpuDuration);
    }
    bool isBusy(){
        if(temp) return true;
        return false;
    }
    void makeEmpty(){
        temp = 0;
    }
    ~CPU(){
        if(temp) delete temp;
    }
};

struct IO{
    double quantumNum, activeTime;
    int id;
    Task* temp = 0;
    IO(double quantumNum_, int id_) {
        this->quantumNum = quantumNum_;
        activeTime = 0;
        this->id = id_;
    }
    void workInIO(Task &a){
        temp = &a;
        if(quantumNum > temp->ioDuration) activeTime += temp->ioDuration;
        else activeTime += quantumNum;
        temp->performIO(this->quantumNum, this->id);
    }
    bool isBusy(){
        if(temp) return true;
        return false;
    }
    void makeEmpty(){
        temp = 0;
    }
    ~IO(){
        if(temp) delete temp;
    }
};


struct taskCPUWorkHeapComperator{
    bool operator()(Task t1, Task t2){
        return t1.cpuDuration > t2.cpuDuration;
    }
};

struct globalTimeHeapComperator{
    bool operator()(Task t1, Task t2){
        if(t1.globalTime == t2.globalTime){
            if(t2.type == "cpuArrival") return false;
            if(t1.type == "cpuArrival") return true;
            if(t1.type == "inIO" && t2.type == "cpuDeparture") return false;
            if(t1.type == "finish" && t2.type == "cpuDeparture") return false;
            if(t2.type == "inIO" && t1.type == "cpuDeparture") return true;
            if(t2.type == "finish" && t1.type == "cpuDeparture") return true;
            if(t1.type == "cpuDeparture" && t2.type == "cpuDeparture") return t1.cpu_id > t2.cpu_id;
            if(t1.type == "inIO" && t2.type == "inIO") return t1.io_id > t2.io_id;
            if(t1.type == "inIO" && t2.type == "finish") return t1.io_id > t2.io_id;
            if(t1.type == "finish" && t2.type == "inIO") return t1.io_id > t2.io_id;
            if(t1.type == "finish" && t2.type == "finish") return t1.io_id > t2.io_id;
        }
        return t1.globalTime > t2.globalTime;
    }
};

struct taskArrivalTimeHeapComperator{
    bool operator()(Task t1, Task t2){
        return t1.arrivalTime > t2.arrivalTime;
    }
};

vector<CPU> cpus;
vector<IO> io;
priority_queue<Task, vector<Task>, taskArrivalTimeHeapComperator> tasks;
priority_queue<Task, vector<Task>, globalTimeHeapComperator> events;
priority_queue<Task, vector<Task>, taskCPUWorkHeapComperator> tasksInCPU;
queue<Task> tasksInIO;
vector<Task> finishedTasks;
long maxSizeTasksInCPU = 0;
long maxSizeTasksInIO = 0;
long idOfMostWorkCPU = 0;
long idOfMostWorkIO = 0;
double avgWaitTimeOfTasks = 0;
double longestWaitTimeOfTasks = 0;
double avgInSystemOfTasks = 0;

void readFile(string fileName){
    ifstream infile(fileName);

    int numOfCPU;
    infile >> numOfCPU;

    for (int i = 0; i < numOfCPU; ++i){
        double frequencyOfCPU;
        infile >> fixed >> setprecision(12) >> frequencyOfCPU;
        cpus.push_back(CPU(frequencyOfCPU, i+1));
    }

    int numOfIO;
    infile >> numOfIO;

    for (int i = 0; i < numOfIO; ++i){
        double quantumNumOfIO;
        infile >> fixed >> setprecision(12) >> quantumNumOfIO;
        io.push_back(IO(quantumNumOfIO, i+1));
    }

    int numOfTask;
    infile >> numOfTask;

    for (int i = 0; i < numOfTask; ++i){
        double arrivalTime, cpuDuration, ioDuration;
        infile >> fixed >> setprecision(12) >> arrivalTime >> cpuDuration >> ioDuration;
        tasks.push(Task("cpuArrival", arrivalTime, cpuDuration, ioDuration));
    }
    infile.close();
}

void writeFile(string fileName){
    ofstream ofs(fileName);
    ofs << fixed << setprecision(6);
    ofs << maxSizeTasksInCPU << "\n" << maxSizeTasksInIO << "\n" << idOfMostWorkCPU << "\n" << idOfMostWorkIO << endl;
    ofs << avgWaitTimeOfTasks << "\n" << longestWaitTimeOfTasks << "\n" << avgInSystemOfTasks << endl;
    ofs.close();
}

void findIdOfMostWorkCpu(){
    double maxActiveTime = 0;
    long id = 0;
    for (long i = cpus.size()-1; i >=0 ; i--) {
        if(maxActiveTime <= cpus[i].activeTime){
            id = i+1;
            maxActiveTime = cpus[i].activeTime;
        }
    }
    if(id != 0) idOfMostWorkCPU = id;
}

void findIdOfMostWorkIO(){
    double maxActiveTime = 0;
    long id = 0;
    for (long i = io.size()-1; i >= 0 ; i--) {
        if(maxActiveTime <= io[i].activeTime){
            id = i+1;
            maxActiveTime = io[i].activeTime;
        }
    }
    if(id != 0) idOfMostWorkIO = id;
}

void findLongestWaitTimeOfTasks(){
    double maxWaitTime = 0;
    for (long i = 0; i < finishedTasks.size(); ++i) {
        double waitTime = (finishedTasks[i].globalTime - finishedTasks[i].arrivalTime) - finishedTasks[i].activeTime;
        if(waitTime > maxWaitTime) maxWaitTime = waitTime;
    }
    longestWaitTimeOfTasks = maxWaitTime;
}

void findAvgWaitTimeOfTasks(){
    double sum = 0;
    for (long i = 0; i < finishedTasks.size(); ++i) {
        sum += (finishedTasks[i].globalTime - finishedTasks[i].arrivalTime) - finishedTasks[i].activeTime;
    }
    avgWaitTimeOfTasks = sum / finishedTasks.size();
}

void findAvgInSystemOfTasks(){
    double sum = 0;
    for (long i = 0; i < finishedTasks.size(); ++i) {
        sum += finishedTasks[i].globalTime - finishedTasks[i].arrivalTime;
    }
    avgInSystemOfTasks = sum / finishedTasks.size();
}

void findStatistics(){
    findIdOfMostWorkCpu();
    findIdOfMostWorkIO();
    findAvgInSystemOfTasks();
    findAvgWaitTimeOfTasks();
    findLongestWaitTimeOfTasks();
}

void execute(){
    if(!tasks.empty()){
        events.push(tasks.top());
        tasks.pop();
    }
    while(!events.empty()){
        Task temp = events.top();
        events.pop();
        if(temp.type == "cpuArrival"){
            for(int i = 0; i < cpus.size(); i++){
                if(!cpus[i].isBusy()){
                    cpus[i].workInCPU(temp);
                    events.push(temp);
                    break;
                }
            }
            if(temp.type == "cpuArrival"){
                tasksInCPU.push(temp);
            }
        }else if(temp.type == "cpuDeparture"){
            cpus[temp.cpu_id - 1].makeEmpty();
            bool cond = true;
            if(!tasksInIO.empty()){
                temp.type = "inQueue";
                tasksInIO.push(temp);
                cond = false;
            }
            if(!tasksInCPU.empty()){
                Task taskOfCPU = tasksInCPU.top();
                taskOfCPU.globalTime = temp.globalTime;
                events.push(taskOfCPU);
                tasksInCPU.pop();
            }
            if(cond) {
                for (int i = 0; i < io.size(); i++) {
                    if (!io[i].isBusy()) {
                        io[i].workInIO(temp);
                        events.push(temp);
                        break;
                    }
                }
                if (temp.type == "cpuDeparture") {
                    temp.type = "inQueue";
                    tasksInIO.push(temp);
                }
            }
        }else if(temp.type == "inIO"){
            io[temp.io_id - 1].makeEmpty();
            if(!tasksInIO.empty()){
                temp.type = "inQueue";
                Task taskOfIO = tasksInIO.front();
                taskOfIO.globalTime = temp.globalTime;
                events.push(taskOfIO);
                tasksInIO.pop();
                tasksInIO.push(temp);
            }else{
                io[temp.io_id - 1].workInIO(temp);
                events.push(temp);
            }
        }else if(temp.type == "inQueue"){
            for(int i = 0; i < io.size(); i++){
                if(!io[i].isBusy()){
                    io[i].workInIO(temp);
                    events.push(temp);
                    break;
                }
            }
        }else if(temp.type == "finish") {
            io[temp.io_id - 1].makeEmpty();
            if(!tasksInIO.empty()){
                Task taskOfIO = tasksInIO.front();
                taskOfIO.globalTime = temp.globalTime;
                events.push(taskOfIO);
                tasksInIO.pop();
            }
            finishedTasks.push_back(temp);
        }
        if(!tasks.empty()){
            while(!tasks.empty() && temp.globalTime > tasks.top().globalTime){
                events.push(tasks.top());
                tasks.pop();
            }
        }

        if(tasksInCPU.size() > maxSizeTasksInCPU) maxSizeTasksInCPU = tasksInCPU.size();
        if(tasksInIO.size() > maxSizeTasksInIO) maxSizeTasksInIO = tasksInIO.size();
    }
    findStatistics();
}

int main (int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Run the code with the following command: ./project2 [input_file] [output_file]" << endl;
        return 1;
    }

    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    readFile(argv[1]);

    execute();

    writeFile(argv[2]);

    return 0;
}