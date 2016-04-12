#include <vector>

using namespace std;

class OdPair {
public:
  int m_origin;
  int m_dest;
  vector<vector<int> > paths;
  vector<Edge> pathEdge;

  OdPair(int origin, int dest){
    m_origin = origin;
    m_dest = dest;
  }
};