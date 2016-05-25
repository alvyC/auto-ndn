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

  }

  ~Map(){
    delete edge_array, weights;
  }

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

  Edge *edge_array;
  int *weights;
};

}
#endif
