#include "map.hpp"

namespace autondn{

void
Map::processMap(){
  std::ifstream route_map;
  int x, y;

  route_map.open("route_map");

  if( !route_map ) {
    std::cout << "[map] route_map file does not exists" << std::endl;
    exit(-1);
  }

  std::string line;
  std::string section;
  while (std::getline(route_map, line))
  {
    if(line == "[nodes]") { section = "nodes"; continue; }
    if(line == "[links]") { section = "links"; continue; }

    if(section == "nodes") {
      //std::cout << "Nodes: " << line << std::endl;
      nodes.push_back(line);
    } else {
      links.push_back(line);
      //std::cout << "Links: " << line << std::endl;
    }

  }

  num_nodes = nodes.size();
  num_links = links.size();

  edge_array = new Edge[num_links];
  weights = new int[num_links];

  int i = 0, pos = 0;

  auto iter = links.begin();
  while( iter != links.end() ) {
    line = *iter;
    pos = line.find(" ");
    std::string node_one = line.substr(0, pos);
    line.erase(0, pos + 1);
    pos = line.find(" ");
    std::string node_two = line.substr(0, pos);
    line.erase(0, pos + 1);

    int pos1 = find(nodes.begin(), nodes.end(), node_one) - nodes.begin();
    int pos2 = find(nodes.begin(), nodes.end(), node_two) - nodes.begin();

    edge_array[i] = Edge(pos1, pos2);

    weights[i] = atoi( line.c_str() );

    ++iter;
    ++i;
  }

  //num_arcs = sizeof(edge_array) / sizeof(Edge);

  num_arcs = links.size();

  graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
  //property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

  std::vector<vertex_descriptor> p(num_vertices(g));
  std::vector<int> d(num_vertices(g));
  //Start at node 0, may need to make it an input variable
  vertex_descriptor s = vertex(0, g);

  dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

//--------------------------------------------------------------------

  std::vector< graph_traits< graph_t >::vertex_descriptor > path;
  //the node we are going to is the last one, 7
  graph_traits< graph_t >::vertex_descriptor current=vertex(7,g);

  while(current!=s) {
    path.push_back(current);
    current=p[current];
  }
  path.push_back(s);

  std::vector< graph_traits< graph_t >::vertex_descriptor >::iterator it;
  for (it=path.end()-2; it != path.begin()-1; --it) {
    std::cout << nodes.at(*it) << std::endl;
  }

//--------------------------------------------------------------------
  /*std::cout << "distances and parents:" << std::endl;
  graph_traits < graph_t >::vertex_iterator vi, vend;
  for (tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    std::cout << "distance(" << name[*vi] << ") = " << d[*vi] << ", ";
    std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::
      endl;
  } */

} //end of process map

}// end of namespace
