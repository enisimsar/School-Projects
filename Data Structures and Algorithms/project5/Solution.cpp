/*
Student Name: Enis Simsar
Student Number: 2014400219
Project Number: 5
Operating System: given VM
Compile Status: OK
Program Status: OK
Notes:
*/

#include <vector>
#include <queue>
#include <climits>
#include <stdio.h>
using namespace std;

struct Vertex{
    long heuristic, id, distanceFromSource;
    bool closedSetCondition = false;
    vector<pair<long, long>> adjList;
    Vertex(long heuristic_, long id_){
        heuristic = heuristic_, id = id_, distanceFromSource = LONG_MAX;
    }
};

struct vertexTotalComparator{
    bool operator()(Vertex* v1, Vertex* v2){
        return v1->distanceFromSource + v1->heuristic > v2->distanceFromSource + v2->heuristic;
    }
};

FILE *in, *out;
vector<Vertex*> vertices;
priority_queue<Vertex*, vector<Vertex*>, vertexTotalComparator> openSet;

int main(int argc, char *argv[]) {
	if(argc < 3) return -1;
	in = fopen(argv[1], "r"), out = fopen(argv[2], "w");
	long N, M;
	fscanf(in, "%ld %ld", &N, &M);
    for(long i = 0, a; i < N; i++) {
        fscanf(in, "%ld", &a);
        vertices.push_back(new Vertex(a, i));
    }
	for(long i = 0, a, b, c; i < M; i++) {
        fscanf(in, "%ld %ld %ld", &a, &b, &c);
		vertices[a]->adjList.push_back(make_pair(b, c));
        vertices[b]->adjList.push_back(make_pair(a, c));
	}
    long source, target;
    fscanf(in, "%ld %ld", &source, &target);
    openSet.push(vertices[source]);
    vertices[source]->distanceFromSource = 0;
    fclose(in);

    long tentativeScore;
    while(true) {
        Vertex* temp = openSet.top();
        openSet.pop();
        if (temp->id == target) break;
        if (vertices[temp->id]->closedSetCondition) continue;
        vertices[temp->id]->closedSetCondition = true;
        vector<pair<long, long>> &tempAdjList = vertices[temp->id]->adjList;
        for (auto &i : tempAdjList) {
            if (vertices[i.first]->closedSetCondition) continue;
            tentativeScore = i.second + vertices[temp->id]->distanceFromSource;
            if (tentativeScore < vertices[i.first]->distanceFromSource) {
                vertices[i.first]->distanceFromSource = tentativeScore;
                openSet.push(vertices[i.first]);
            }
        }
    }

    fprintf(out, "%ld", vertices[target]->distanceFromSource);
    fclose(out);
    for (int j = 0; j < N; ++j) delete vertices[j];
	return 0;
}