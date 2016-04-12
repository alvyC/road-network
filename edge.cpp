class Edge {
public:
  int m_name;
  int m_from;
  int m_to;

  Edge(int name, int from, int to) {
    m_name = name;
    m_from = from;
    m_to = to;
  }
};