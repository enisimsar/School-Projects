/*
Student Name: Enis Simsar
Student Number: 2014400219
Project Number: 3
Operating System: Ubuntu (Given VM)
Compile Status: Working
Program Status: Working
Notes: This program writes the results with no new line after the result and fixed the precision 6 digit after dot. Please, notice it.
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <iomanip>

using namespace std;

/*
 * Keeps the information about vertex such as process time, finish time, in degree and out degree
 */
struct Vertex{
    double processTime, finishTime;
    int in, out;
    Vertex(double processTime_){
        processTime = processTime_;
        finishTime = 0;
        in = 0;
        out = 0;
    }
    /*
     * When process occurs, it takes maximum time because maximum time gives us the all processes are done before this vertex.
     */
    void changeDuration(double otherFinishTime){
        if(otherFinishTime > this->finishTime) this->finishTime = otherFinishTime;
    }
};

/*
 * Keeps all vertices
 */
vector<Vertex> vertices;
/*
 * The adj list of the Graph
 */
vector<vector<int>> adjList;
unsigned long numOfV = 0;
double result = -1;
int numOfRemovedVertices = 0;
queue<int> inZeroDegree;
queue<int> outZeroDegree;

void readFile(const string &fileName) {

    ifstream infile(fileName);

    infile >> numOfV;
    adjList.resize(numOfV);

    int numOfE;
    infile >> numOfE;

    for (int i = 0; i < numOfV; ++i) {
        double vertex;
        infile >> vertex;
        vertices.push_back(Vertex(vertex));
    }

    for (int i = 0; i < numOfE; ++i) {
        int first, second;
        infile >> first >> second;
        vertices[first].out++;
        vertices[second].in++;
        adjList[first].push_back(second);
    }

    for (int i = 0; i < numOfV; i++) {
        if (vertices[i].in == 0) inZeroDegree.push(i);
        if (vertices[i].out == 0) outZeroDegree.push(i);
    }
    infile.close();
}

/*
 * Executes all process
 */
void execute(){

    /*
     * Processes until finishing the all vertices that have 0 out degree
     */
    while (!inZeroDegree.empty()) {
        numOfRemovedVertices++;
        int vertexID = inZeroDegree.front();
        inZeroDegree.pop();
        for(int i=0; i<adjList[vertexID].size(); i++){
            vertices[adjList[vertexID][i]].in--;
            vertices[adjList[vertexID][i]].changeDuration(vertices[vertexID].finishTime + vertices[vertexID].processTime);
            if(vertices[adjList[vertexID][i]].in==0) inZeroDegree.push(adjList[vertexID][i]);
        }
    }

    /*
     * Checks the graph is cyclic or not
     */
    if(numOfRemovedVertices < numOfV) return;

    /*
     * Checks all finished vertices and takes the maximum finish time because all process are done before the last vertex that have out degree 0.
     */
    while(!outZeroDegree.empty()){
        int vertexID = outZeroDegree.front();
        outZeroDegree.pop();
        vertices[vertexID].finishTime += vertices[vertexID].processTime;
        if(vertices[vertexID].finishTime > result) result = vertices[vertexID].finishTime;
    }
}

void writeFile(const string &fileName){
    ofstream outfile(fileName);
    outfile << fixed << setprecision(6) << result;
    outfile.close();
}

int main (int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Run the code with the following command: ./project3 [input_file] [output_file]" << endl;
        return 1;
    }

    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    readFile(argv[1]);
    execute();
    writeFile(argv[2]);

    return 0;
}