#ifndef EXAMPLE_SHORTEST_PATH_HEURISTIC_H
#define EXAMPLE_SHORTEST_PATH_HEURISTIC_H

#include "../Utils/Definitions.h"

class ShortestPathHeuristic {
private:
	size_t 					source;
	std::vector<MapNodePtr> all_nodes;

	void compute(size_t cost_idx, const AdjacencyMatrix& adj_matrix);
public:
	ShortestPathHeuristic(size_t source, size_t graph_size, const AdjacencyMatrix &adj_matrix);
	Triplet<size_t> operator()(size_t node_id);
};

#endif // EXAMPLE_SHORTEST_PATH_HEURISTIC_H