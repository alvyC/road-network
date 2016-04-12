#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include "edge.cpp"
#include "oDPair.cpp"

using namespace std;

int noOfVertices;
int noOfEdges;
//vector<vector<int> > adjacencyList(noOfEdges + 1);
vector<Edge> edges;
vector<int> vertices(2000);
vector<int> origin;
vector<int> destination;
vector<int> other;
vector<bool> isOriginDestin;
vector<vector<double> > demand(2000);
vector<OdPair> oDPairs;
vector<vector<int> > MatrixA;
vector<double> MatrixB(2000);
vector<vector<int> > LinkPathMatrix;

void showGraph() {
  /*for (int i = 1; i < adjacencyList.size(); i++) {
    std::cout << i << ": ";
    for (int j = 0; j < adjacencyList[i].size(); j++) {
      std::cout << adjacencyList[i][j] << " ";
    }
    std::cout << std::endl;
  }*/
  cout << "Showing graph: " << endl;
  for (vector<Edge>::iterator it = edges.begin(); it != edges.end(); ++it) {
    cout << it->m_name << ": " << it->m_from << "->"<< it->m_to << endl;
  }
  //std::cout << edges.size() << std::endl;
}

void loadGraph() {
  cout << "data_net/bi_direc_net2/link.txt" << endl;
  freopen("data_net/bi_direc_net2/link.txt", "r", stdin);

  int node1, node2;
  noOfVertices = 0;
  noOfEdges = 1;
  while(scanf("%d %d", &node1, &node2) == 2) {
    //adjacencyList[node1].push_back(node2);
    Edge edge(noOfEdges, node1, node2);
    edges.push_back(edge);
    noOfEdges++;

    if (vertices[node1] == 0) {
      vertices[node1] = node1;
      noOfVertices++;
    }

    if (vertices[node2] == 0) {
      vertices[node2] = node2;
      noOfVertices++;
    }
  }
  vertices.resize(noOfVertices);
  cout << "# of edges: " << edges.size() << ", # of vertices: " << vertices.size() << endl;;
}

void showMatrix(vector<int>& matrix) {
  for (int i = 0; i < matrix.size(); i++) {
    std::cout << matrix[i] << " ";
  }
  cout << endl;
}

void loadOrigin() {
  freopen("data_net/bi_direc_net2/origin_nums.txt", "r", stdin);

  int node, nOrigins = 0;
  while(scanf("%d", &node) == 1) {
    origin.push_back(node);
    nOrigins++;
  }
  origin.resize(nOrigins);
}

void loadDestination() {
  freopen("data_net/bi_direc_net2/destin_nums.txt", "r", stdin);

  int node, nDestinations = 0;
  while(scanf("%d", &node) == 1) {
    destination.push_back(node);
    nDestinations++;
  }
  destination.resize(nDestinations);
}

void loadOther() {
  //cout << "noOfVertices: " << vertices.size() << endl;
  isOriginDestin.resize(noOfVertices + 1, false);
  int nOthers = 0;
  for (int i = 1; i <= noOfVertices; i++) {
    for (int j = 0; j < origin.size(); j++) {
      if (vertices[i] == origin[j]) {
        isOriginDestin[i] = true;
      }
    }
    for (int j = 0; j < destination.size(); j++) {
      if (vertices[i] == destination[j]) {
        isOriginDestin[i] = true;
      }
    }
    if (!isOriginDestin[i]) {
      other.push_back(vertices[i]);
      nOthers++;
    }
  }
  other.resize(nOthers);
}

void createOdPairs() {
  cout << "createOdPairs" << endl;
  for (int i = 0; i < origin.size(); i++) {
    for (int j = 0; j < destination.size(); j++) {
      if (origin[i] != destination[j]) {
        OdPair odPair(origin[i], destination[j]);
        oDPairs.push_back(odPair);
      }
    }
  }
}

void createMatrixA() {
  cout << "createMatrixA" << endl;
  int row = vertices.size() * 2;
  int col = edges.size() + 1;
  MatrixA.resize(row, vector<int>(col));

  /*std::cout << MatrixA.size() << std::endl;
    cout << MatrixA[0].size() << endl;
    cout << MatrixA[1].size() << endl;*/

  for (vector<Edge>::iterator it = edges.begin(); it != edges.end(); ++it) {
    int size = 0;
    for (int i = 0; i < origin.size(); i++) {
      if (origin[i] == it->m_from) {
        MatrixA[size][it->m_name] = 1;
      }
      size++;
    }

    for (int i = 0; i < destination.size(); i++) {
      if (destination[i] == it->m_to) {
        MatrixA[size][it->m_name] = 1;
      }
      size++;
    }

    for (int i = 0; i < other.size(); i++) {
      if (other[i] == it->m_from) {
        MatrixA[size][it->m_name] = -1;
      }

      if (other[i] == it->m_to) {
        MatrixA[size][it->m_name] = 1;
      }
      size++;
    }
    row = size;
     MatrixA.resize(row, vector<int>(col));
  }

  cout << "Print Matrix A: " << endl;
  //print Matrix A
  cout << MatrixA.size() << endl;
  for (int i = 0; i < row; i++) {
    for (int j = 1; j < col; j++) {
      cout << MatrixA[i][j] << " ";
    }
    cout << endl;
  }
}

vector<int> getNeighbors(int src) {
  vector<int> neighbors;
  for (int i = 0; i < edges.size(); i++) {
    if (src == edges[i].m_from) {
      neighbors.push_back(edges[i].m_to);
    }
  }
  return neighbors;
}

void findAllPaths(int src, int dest, vector<bool>& visited,
                  vector<int> path, int& path_index, int odPairIndex) {
  //cout << "findAllPaths" << endl;
  visited[src] = true;
  path[path_index] = src;
  path_index++;

  if (src == dest) {
    path.resize(path_index);
    oDPairs[odPairIndex].paths.push_back(path);
    /*cout << odPairIndex << ": " << endl;
    for (int j = 0; j < path_index; j++)
            cout << " " << path[j] << " ";
    cout << endl;*/
  }
  else {
    vector<int> neighbors = getNeighbors(src);
    for (int j = 0; j < neighbors.size(); j++) {
      if (!visited[neighbors[j]]) {
        findAllPaths(neighbors[j], dest, visited, path, path_index, odPairIndex);
      }
    }
  }

  path_index--;
  visited[src] = false;
}

void findPathSForOdP2air() {
  cout << "findPathSForOdP2air" << endl;
  for (int i = 0; i < oDPairs.size(); i++) {
    vector<bool> visited(noOfVertices + 1, false);
    vector<int> path(noOfVertices + 1);
    int path_index = 0;
    //cout << "(" << oDPairs[i].m_origin << ", " << oDPairs[i].m_dest << ")" << endl;
    findAllPaths(oDPairs[i].m_origin, oDPairs[i].m_dest, visited, path, path_index, i);
  }
}

void printPathSForOdP2air() {
  cout << "Pringting path for odpair" << endl;
  vector<OdPair>::iterator it = oDPairs.begin();
  for(; it != oDPairs.end(); it++) {
    cout << "(" << it->m_origin << ", " << it->m_dest << ")" << endl;
    for (int i = 0; i < it->paths.size(); i++) {
      for (int j = 0; j < it->paths[i].size(); j++) {
        cout << it->paths[i][j] << " ";
      }
      cout << endl;
    }
  }
}

void createLinkPathMatrix() {
  cout << "createLinkPathMatrix" << endl;
  int row = 0;
  for(vector<OdPair>::iterator it = oDPairs.begin(); it != oDPairs.end(); it++) {
      row += it->paths.size();
  }

  int col = edges.size() + 1;
  MatrixA.resize(row, vector<int>(col));
  LinkPathMatrix.resize(row, vector<int>(col));

  row = 0;
  for(vector<OdPair>::iterator it = oDPairs.begin(); it != oDPairs.end(); it++) {
    for (int i = 0; i < it->paths.size(); i++) {
      for (int j = 0; j < it->paths[i].size() - 1; j++) {
        for (int k = 0; k < edges.size(); k++) {
          if (it->paths[i][j] == edges[k].m_from && it->paths[i][j + 1] == edges[k].m_to) {
            LinkPathMatrix[row][edges[k].m_name] = 1;
          }
        }
      }
      row++;
    }
  }
}

void printLinkPathMatrix() {
  cout << "Printing Link-Path matrix" << endl;
  for (int i = 0; i < LinkPathMatrix.size(); i++) {
    for (int j = 1; j < LinkPathMatrix[i].size(); j++) {
      cout << LinkPathMatrix[i][j] << " ";
    }
    cout << endl;
  }
}

void loadDemand() {
  // load demand
  cout << "Load demand" << endl;
  ifstream file("data_net/bi_direc_net2/demand.txt");
  if (file.good()) {
    string str;
    int index = 0, row = 0;
    while(getline(file, str)) {
      istringstream ss(str);
      double num;
      int col = 0;
      while (ss >> num) {
        demand[index].push_back(num);
        //cout << num << " ";
        col++;
      }
      //cout << endl;
      demand[index].resize(col);
      row++;
      index++;
    }
    demand.resize(row);
  }
}

void showDemand() {
  for (int i = 0; i < demand.size(); i++) {
    for(int j = 0; j < demand[i].size(); j++) {
      cout << demand[i][j] << " ";
    }
    cout << endl;
  }
}

void showMatrixB() {
  cout << "matrixB size: " << MatrixB.size() << endl;
  for (int i = 0; i < MatrixB.size(); i++) {
    cout << MatrixB[i] << endl;
  }
}

void createMatrixB() {
  // create MatrixB: summation of rows
  int index = 0;
  for (int i = 0; i < demand.size(); i++) {
    for (int j = 0; j < demand[i].size(); j++) {
      //cout << demand[i][j] << " ";
      MatrixB[index] += demand[i][j];
    }
    index++;
    //cout << endl;
  }

  // create MatrixB: summation of columns
  for (int i = 0; i < demand[0].size(); i++) {
    for (int j = 0; j < demand.size(); j++) {
      //cout << demand[j][i] << " ";
      MatrixB[index] += demand[j][i];
    }
    //cout << endl;
    index++;
  }

  for (int i = 1; i <= noOfVertices; i++) {
    if (!isOriginDestin[i]) {
      MatrixB[index] = 0;
      index++;
    }
  }

  MatrixB.resize(index);
}

void writeMatrixB() {
  ofstream output_file;
  output_file.open("output/matrixB.txt");
  for (int i = 0; i < MatrixB.size(); i++) {
    output_file << MatrixB[i] << '\n';
  }
}

void writeMatrixA() {
  ofstream output_file;
  output_file.open("output/matrixA.txt");
  for (int i = 0; i < MatrixA.size(); i++) {
    for (int j = 1; j < MatrixA[i].size(); j++) {
      output_file << MatrixA[i][j] << "\t";
    }
    output_file << '\n';
  }
}

void writeLinkPathMatrix() {
  ofstream output_file;
  output_file.open("output/linkPathMatrix.txt");
  for (int i = 0; i < LinkPathMatrix.size(); i++) {
    for (int j = 1; j < LinkPathMatrix[i].size(); j++) {
      output_file << LinkPathMatrix[i][j] << "\t";
    }
    output_file << "\n";
  }
}

int main() {
  // load & show graph
  loadGraph();
  showGraph();

  // load origin, destination, demand
  loadOrigin();
  loadDestination();
  loadOther();
  loadDemand();

  // show origin, destination, other, demand
  cout << "Show origin matrix: " << endl;
  showMatrix(origin);
  cout << "Show destination matrix:" << endl;
  showMatrix(destination);
  cout << "Show other: " << endl;
  showMatrix(other);
  cout << "Show demand: " << endl;
  showDemand();

  // create and show MatrixB
  createMatrixB();
  //cout << "Showing Matrix B:" << endl;
  //showMatrixB();

  // create and show Matrix A
  createMatrixA();

  // create & print oDpairs
  createOdPairs();
  findPathSForOdP2air();
  //printPathSForOdP2air();

  // create and print LinkPathMatrix
  createLinkPathMatrix();
  //printLinkPathMatrix();



  // write MatrixA, MatrixB, and LinkPathMatrix to files
  writeMatrixB();
  writeMatrixA();
  writeLinkPathMatrix();
}