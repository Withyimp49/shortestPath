#include <iostream>
#include <string>
//uses graaf library to create, calculate, and output graph.
//https://github.com/bobluppes/graaf?tab=readme-ov-file
#include <graaflib/graph.h>
#include <graaflib/io/dot.h>
#include <graaflib/algorithm/shortest_path/dijkstra_shortest_path.h>

//declare global variables
const short int MAX_NUM_OF_VERTICES = 100;

//our own implementation of Dijkstra's Shortest Path Algorithm
std::list<graaf::vertex_id_t> DijkstraShortestPathAlgorithm(graaf::undirected_graph<int, unsigned int>& graph, 
		short int& start, short int& end, unsigned int& shortestTime) {
	//declare local variables
	graaf::vertex_id_t currentVertex = graph.get_vertex(start);
	graaf::vertex_id_t previousVertices[MAX_NUM_OF_VERTICES]; 
	graaf::vertex_id_t pathHolder[MAX_NUM_OF_VERTICES];
	std::list<graaf::vertex_id_t> shortestPath;
	graaf::vertex_id_t nextVertex = currentVertex;
	short int pathLength = 0;
	unsigned int minimumTimes[MAX_NUM_OF_VERTICES];
	bool visitedVertices[MAX_NUM_OF_VERTICES];
	const std::unordered_map<graaf::edge_id_t, unsigned int, graaf::edge_id_hash> edges = graph.get_edges();

	//iterate through and initialize arrays. Will be indeterminate values otherwise, breaking everything.
	for (int i = 0; i < MAX_NUM_OF_VERTICES; i++) {
		previousVertices[i] = currentVertex; //setting all vertices to point to start vertex is temporary to create determinate values.
		minimumTimes[i] = 0; //0 will represent infinity, except for start, since time/weights are unsigned
		visitedVertices[i] = false; 
		pathHolder[i] = currentVertex; //setting all start vertex is temporary to create determinate values.
	}

	//keep going until we reach our destination
	while (currentVertex != graph.get_vertex(end)) {
		//vertex variables can be used as their ID's value, which we have as their int index value. So we can use them as indexes for arrays.
		visitedVertices[currentVertex] = true;
		unsigned int nextShortestPath = 0;
		//go through edge connections and update adjacent shortest times and find next path.
		for (auto& edge : edges) {
			//edge is represented as (pair<node1, node2>, edge_weight)
			if (edge.first.first == currentVertex || edge.first.second == currentVertex) {
				//determine which node is the "other" node in the pair
				graaf::vertex_id_t otherVertex = edge.first.first == currentVertex ? edge.first.second : edge.first.first;

				//check if time to other node is less than time stored in other node and update other nodes time and previous
				if (minimumTimes[otherVertex] == 0 && otherVertex != graph.get_vertex(start) 
						|| edge.second + minimumTimes[currentVertex] < minimumTimes[otherVertex]) {
					minimumTimes[otherVertex] = edge.second + minimumTimes[currentVertex];
					previousVertices[otherVertex] = currentVertex;
				}

				//determine if current edge is potenitally next shortest path to travel. 
				// Remember, 0 is infinity, which is our shortest known path at start before checking neighbors.
				if (nextShortestPath == 0 && !visitedVertices[otherVertex] || edge.second < nextShortestPath && !visitedVertices[otherVertex]) {
					nextShortestPath = edge.second;
					nextVertex = otherVertex;
				}
			}
		}
		if (nextVertex == currentVertex) {
			std::cout << "ERROR: No valid path found!";
			return shortestPath;
		}
		//jump to next vertex, updating the time we've traveled and our current node accordingly
		currentVertex = nextVertex;
	}

	//From the end, work backwards to find shortest path now that previousVertices all now point backwards in the optimal direction.
	//Since values are backwards at first, we store in temporary array before putting it in correct order into paramter array path[]
	for (int i = 0; currentVertex != graph.get_vertex(start); i++) {
		pathHolder[i] = currentVertex;
		currentVertex = previousVertices[currentVertex];
		pathLength++;
	}
	while (pathLength >= 0) {
		shortestPath.push_back(pathHolder[pathLength]);
		pathLength--;
	}

	//return the shortest time and path by returning the list<graaf::vertex_id_t> and replacing shortestTime paramter variable.
	shortestTime = minimumTimes[end];
	return shortestPath;
}

//main method
int main() {
	//declare local variables
	graaf::undirected_graph<int, unsigned int> graph;
	std::list<graaf::vertex_id_t> shortestPath;
	int numberOfNodes = 0;
	short int start = 0;
	short int end = 0;
	unsigned int edgeWeight = 0;
	unsigned int shortestTime = 0;


	//indicate to user that program has started
	std::cout << "STARTING\n";

	//ask user to input number of nodes and create them
	std::cout << "How many vertices/nodes would you like (max size of " << MAX_NUM_OF_VERTICES << "): \n";
	std::cin >> numberOfNodes;
	for (int i = 0; i < numberOfNodes && i < MAX_NUM_OF_VERTICES; i++) {
		graph.add_vertex(i);
	}
	
	//ask user to specify edges and create them
	while (start >= 0 && end >= 0 && edgeWeight >= 0) {
		std::cout << "Put start, end, and edge weight for connection (e.g. to connect node index 0 and 3 with weight of 5, type: 0 3 5)."
			<< "\nType a negative number for any value to stop making connections.\n";
		std::cin >> start >> end >> edgeWeight;
		if (start > numberOfNodes - 1 || end > numberOfNodes - 1 || edgeWeight == 0) {
			std::cout << "Please put in VALID start, end, and edge weight values\n";
		}
		if (start >= 0 && end >= 0 && edgeWeight >= 0) {
			graph.add_edge(start, end, edgeWeight);
		}
	}

	//generate graph
	graaf::io::to_dot(graph, "./Graph.dot");
	std::cout << "Graph generated! put the Graph.dot file code into http://www.webgraphviz.com/ to view the graph\n";

	//ask user for start and end index values of shortest path to find
	start = -1;
	end = -1;
	while (start < 0 || start > numberOfNodes - 1 || end < 0 || end > numberOfNodes - 1 || start == end) {
		std::cout << "please put the valid index of the nodes you want to find the shortest path of "
			<< "(e.g. for finding the shortest path from index nodes 1 to 8, type: 1 8).\n";
		std::cin >> start >> end;
	}

	//show GRAAF solution
	shortestPath = graaf::algorithm::dijkstra_shortest_path(graph, start, end).value().vertices;

	std::cout << "GRAAF shortest path vertices: ";	
	for (graaf::vertex_id_t vertex : shortestPath) {
		std::cout << vertex << ", ";
	}

	//show our solution
	shortestPath = DijkstraShortestPathAlgorithm(graph, start, end, shortestTime);

	//return of empty path means no valid path was found and program should fail.
	if (shortestPath.empty())
		return -1;

	std::cout << "\nOur shortest path vertices: ";
	for (graaf::vertex_id_t vertex : shortestPath) {
		std::cout << vertex << ", ";
	}

	std::cout << "\nOur shortest path time: " << shortestTime << std::endl;

	//end the program
	std::cout << "ENDING\n";
	return 0;
}