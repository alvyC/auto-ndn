#ifndef MAP_HPP
#define MAP_HPP

#include <boost/config.hpp>
#include <iostream>
#include <fstream>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

namespace autondn {

class Map {

public:
  Map() {
    readMap();
    processMap();
  }

  ~Map(){
    delete edge_array;
    delete weights;
  }

  void
  calculatePath( std::string&, std::string& );

  //It is upto the controller to decide when to add a link back
  //after removing it

  //return the link distance
  int
  removeLink( std::string&, std::string& );

  bool
  addLink( std::string&, std::string&, int& );

private:
  void
  readMap();

  void
  processMap();

private:
  typedef adjacency_list < listS, vecS, undirectedS,
    no_property, property < edge_weight_t, int > > graph_t;

  typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
  typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
  typedef std::pair<int, int> Edge;

  int num_nodes, num_links, num_arcs;
  std::vector<std::string> nodes, links;

  std::vector<std::pair<Edge, int>> edgeVector;
  Edge *edge_array;
  int *weights;
};

}
#endif
