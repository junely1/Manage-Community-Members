//
//  Community member managing program using SCC
//  Created on 5/25/18.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include <ctime>
using namespace std;

struct Vertex {
public:
	int id;
	string name;
	string phone;
	Vertex* leader;
	int status;         //unvisited or visited or finished
	Vertex() {
		id = 0;
		name = "";
		phone = "";
		leader = NULL;
		status = 0;         //status = unvisited
	}
	Vertex(int, string, string);
};

Vertex::Vertex(int id, string name, string phone) {
	this->id = id;
	this->name = name;
	this->phone = phone;
}

class SCCgraph {
public:
	int SCCnum;                                 //number of Strongly Connected Components
	bool isEdge;                                //if there is an edge between 2 vertices or not
	SCCgraph* transpose;
	vector <int> *adjList;                      //adjacency list
	vector <int> *adjList2;                     //adjacency list of transposed graph
	vector <Vertex> members;                    //vertexes with members information
	stack <int> stck;                           //stack
	void findSCC();                             //find Strongly Connected Component
	void DFS1(int);                             //DFS for the graph in the first phase
	void DFS2(int, int);                        //DFS for the transposed graph in the second phase
	void getLeader(int);
	void modify(int, int);
};

void SCCgraph::findSCC() {
	int i, vertex;
	for (i = 0; i < members.size(); i++) {       //initilize as unvisited
		members[i].status = 0;
	}
	//phase 1 - DFS the graph
	for (i = 0; i < members.size(); i++) {       //DFS from 0 id
		if (members[i].status == 0) {             //if not visited
			DFS1(i);                            //first time DFS
		}
	}
	//phase 2 -  DFS the transposed graph
	for (i = 0; i < members.size(); i++) {        //initialize as unvisited
		members[i].status = 0;
	}
	SCCnum = 0;                                 //initialize number of SCC
	while (!stck.empty()) {
		vertex = stck.top();                    //get the vertex of the top of the stack
		stck.pop();                             //pop
		if (members[vertex].status == 0) {       //if the vertex on the top of stack is not visited
			DFS2(vertex, vertex);               //2nd time DFS
			SCCnum++;                           //update number of SCC
		}
	}
}

//phase 1 - DFS the graph
void SCCgraph::DFS1(int id) {
	int i;
	members[id].status = 1;                         //visited vertex
	for (i = 0; i < adjList[id].size(); i++) {
		if (members[adjList[id][i]].status == 0)     //if not visited
			DFS1(adjList[id][i]);                   //recursively traverse its adjacency vertexes
	}
	stck.push(id);                                  //push to the stack after vertex is finished
}

//phase 2 - transpose the graph and DFS
void SCCgraph::DFS2(int id, int leadV) {
	int i;
	members[id].leader = &members[leadV];           //update leader
	members[id].status = 1;                         //mark as visited vertex
	for (i = 0; i < adjList2[id].size(); i++) {
		if (members[adjList2[id][i]].status == 0)    //if not visited
			DFS2(adjList2[id][i], leadV);           //recursively traverse its adjacency vertexes
	}
}

//print out information of a vertex's leader
void SCCgraph::getLeader(int v1) {
	cout << members[v1].leader->id << " " << members[v1].leader->name << " " << members[v1].leader->phone << endl;
}

//modify edges as follow or unfollow
void SCCgraph::modify(int v1, int v2) {
	isEdge = 0;
	for (int i = 0; i < adjList[v1].size(); i++) {                    //if there is an edge, unfollow
		if (adjList[v1][i] == v2) {
			adjList[v1].erase(adjList[v1].begin() + i);             //delete the edge from the graph
			for (i = 0; i < adjList[v2].size(); i++) {                //if there is an edge, unfollow also in the transposed graph
				if (adjList[v2][i] == v1)
					adjList[v2].erase(adjList[v2].begin() + i);     //delete the edge from the transposed graph
			}
			isEdge = 1;
		}
	}
	if (!isEdge) {                          //if no edge exists
		adjList[v1].push_back(v2);        //follow in the graph
		adjList2[v2].push_back(v1);       //follow in the transposed graph also
	}
	findSCC();                            //update SCC and leaders
	cout << members[v1].leader->id << endl;
}


int main(int argc, const char * argv[]) {
	int n, m, i;
	int v1, v2;
	int id;
	string name, phone;
	string filename;
	char menu;
	SCCgraph graph;

	/*freopen("query_1000.txt", "r", stdin);
	freopen("output.txt", "w", stdout);*/

	cout << "입력파일의 이름을 입력하세요 : ";
	cin >> filename;                              //get file name
	fstream infile(filename);                   //open file
	if (!infile.is_open()) {
		cout << "Unable to Open\n";
		return 0;
	}
	infile >> n >> m;
	graph.members.resize(n);                    //initialize the members vector
	graph.adjList = new vector<int>[n];         //intialize the adjacency list for the graph
	graph.adjList2 = new vector<int>[n];        //intialize the adjacency list for the transposed graph
	for (i = 0; i < n; i++) {
		infile >> id >> name >> phone;
		Vertex v(id, name, phone);              //initialize a member
		graph.members[id] = v;                  //insert to member vector
	}
	for (i = 0; i < m; i++) {
		infile >> v1 >> v2;
		graph.adjList[v1].push_back(v2);        //insert the edge to adjacency list
		graph.adjList2[v2].push_back(v1);       //insert the edge to transposed adjacency list
	}
	graph.findSCC();                            //find SCC and leaders

	while (cin >> menu) {
		switch (menu) {
		case 'T':                           //print leader of the vertex
			cin >> v1;
			graph.getLeader(v1);            //get leader information
			break;
		case 'A':                           //follow or unfollow an edge
			cin >> v1 >> v2;
			graph.modify(v1, v2);           //modify edges
			break;
		default:                            //quit the program
			cout << graph.SCCnum << endl;       //print the number of SCC in the graph
			infile.close();
			return 0;
		}
	}
	infile.close();
	return 0;
}
