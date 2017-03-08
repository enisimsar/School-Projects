/*
Student Name: Enis Simsar
Student Number: 2014400219
Project Number: 4
Operating System: Ubuntu (Given VM)
Compile Status: Working
Program Status: Working
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

struct Edge{
    int weight, first, second;
    Edge(int weight_, int first_, int second_){
        weight = weight_, first = first_, second = second_;
    }
};

struct edgeWeightComparator{
    bool operator()(Edge& e1, Edge& e2){
        return e1.weight < e2.weight;
    }
};

vector<bool> vertices;
priority_queue<Edge, vector<Edge>, edgeWeightComparator> edgeList; //Max heap
unsigned long N = 0;
unsigned long M = 0;
long long result = 0;

struct DisjointSet{
    long size;
    vector<int> arr;
    DisjointSet(long size_){
        size = size_;
        for (int i = 0; i < size; ++i) arr.push_back(-1);
    }
    int find(int set){
        if(arr[set] == -1) return set;
        else return find(arr[set]);
    }
    void Union(int set1,int set2){
        if(vertices[set1]) arr[set2] = set1;
        else arr[set1] = set2;
    }
};

void readFile(const string &fileName) {
    ifstream fin(fileName);
    fin >> N >> M;
    for(long i = 0; i < N; ++i) vertices.push_back(0);
    int first, second, weight;
    for(long i=0; i<N-1; i++) {
        fin >> first >> second >> weight;
        edgeList.push(Edge(weight, first, second));
    }
    int distinctNum;
    for (long k = 0; k < M; ++k) {
        fin >> distinctNum;
        vertices[distinctNum] = true;
    }
    fin.close();
}

void execute(){
	DisjointSet ds(N);
    long numOfCut = 0;
  	while (numOfCut != M-1) {
  		Edge temp = edgeList.top();
  		edgeList.pop();
        int root1 = ds.find(temp.first);
        int root2 = ds.find(temp.second);
  		if(vertices[root1] && vertices[root2]){
            result += temp.weight;
            numOfCut++;
        }
        else ds.Union(root1,root2);
    }
}

void writeFile(const string &fileName){
    ofstream outfile(fileName);
    outfile << result;
    outfile.close();
}

int main (int argc, char* argv[]) {
    readFile(argv[1]);
    execute();
    writeFile(argv[2]);
    return 0;
}