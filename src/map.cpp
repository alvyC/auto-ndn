#include "map.hpp"

namespace autondn{

void
Map::readMap(){
  std::ifstream route_map;

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

  route_map.close();
} //end of readMap

void
Map::processMap() {
  num_nodes = nodes.size();
  num_links = links.size();

  int i = 0, pos = 0;

  auto iter = links.begin();
  while( iter != links.end() ) {
    std::string line = *iter;
    pos = line.find(" ");
    std::string node_one = line.substr(0, pos);
    line.erase(0, pos + 1);
    pos = line.find(" ");
    std::string node_two = line.substr(0, pos);
    line.erase(0, pos + 1);

    int pos1 = find(nodes.begin(), nodes.end(), node_one) - nodes.begin();
    int pos2 = find(nodes.begin(), nodes.end(), node_two) - nodes.begin();

    //Add the edge and the link weight (dist to edge vector)
    edgeVector.push_back( std::make_pair(Edge(pos1, pos2), atoi(line.c_str())) );

    ++iter;
    ++i;
  }
} //end of processMap

void
Map::calculatePath(std::string& start, std::string& end) {
  weights = new int[num_links];
  edge_array = new Edge[num_links];

  //int weights[num_links];
  //Edge edge_array[num_links];

  int i = 0;
  for(auto iter = edgeVector.begin(); iter!= edgeVector.end();  ++iter) {
    edge_array[i] = iter->first;
    weights[i] = iter->second;
    //std::cout << nodes.at(edge_array[i].first) << " " << nodes.at(edge_array[i].second) << " " << weights[i] << std::endl;
    i++;
  }
  //exit(0);
  int startPt = find(nodes.begin(), nodes.end(), start) - nodes.begin();
  int endPt = find(nodes.begin(), nodes.end(), end) - nodes.begin();

  //num_arcs = sizeof(edge_array) / sizeof(Edge);

  num_arcs = links.size();

  graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
  //property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

  std::vector<vertex_descriptor> p(num_vertices(g));
  std::vector<int> d(num_vertices(g));
  //Start at node 0, may need to make it an input variable
  vertex_descriptor s = vertex(startPt, g);

  dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));
  std::vector< graph_traits< graph_t >::vertex_descriptor > path;

  graph_traits< graph_t >::vertex_descriptor current=vertex(endPt, g);

  //This will be inifinite loop if no path to the end
  while(current!=s) {
    path.push_back(current);
    current=p[current];
  }
  path.push_back(s);

  std::vector< graph_traits< graph_t >::vertex_descriptor >::iterator it;
  for (it=path.end()-1; it != path.begin()-1; --it) {
    std::cout << "--> (" << nodes.at(*it) << "-" << weights[*it] << ") ";
  }
  std::cout << std::endl;
  //HOW TO GET THE DIST? -WEIGHTS
} //end of calculatePath

int
Map::removeLink(std::string &node1, std::string &node2)
{
  int pos1 = find(nodes.begin(), nodes.end(), node1) - nodes.begin();
  int pos2 = find(nodes.begin(), nodes.end(), node2) - nodes.begin();

  Edge removeEdge(pos1, pos2);

  int dist = 0;
  int i = 0;
  for(auto iter = edgeVector.begin(); iter!=edgeVector.end(); ++iter) {
    if(iter->first == removeEdge) {
      dist = iter->second;
      edgeVector.erase(edgeVector.begin() + i);
      --num_links;
      return dist;
    }
    i++;
  }
  return 0;
}

bool
Map::addLink(std::string &node1, std::string &node2, int &dist)
{
  size_t not_found = -1;

  for(auto iter = nodes.begin(); iter != nodes.end();  ) {
    if( (*iter).find(node1) == not_found || (*iter).find(node2) == not_found ) {
      //One or both of the nodes are not found
      return false;
    }
  }

  int pos1 = find(nodes.begin(), nodes.end(), node1) - nodes.begin();
  int pos2 = find(nodes.begin(), nodes.end(), node2) - nodes.begin();

  edgeVector.push_back( std::make_pair(Edge(pos1, pos2), dist) );
  ++num_links;
  return true;
}

}// end of namespace
