#ifndef TESTING_SHORTEST_PATH_HEURISTIC_H
#define TESTING_SHORTEST_PATH_HEURISTIC_H

#include <set>
#include "../Utils/Definitions.h"
#include "../SearchUtils/SearchNode.h"

class ShortestPathHeuristic
{
private:
    using SearchNodePtrsSortedByCost = std::multiset<SearchNodePtr, SearchNode::less_than_specific_cost_until_now>;

    const AdjacencyMatrix&              adj_matrix;
    std::vector<Triplet<CostType>>      shortest_path_cost;
    Idx                                 source_vertex_id;

    void compute(size_t cost_idx);
    SearchNodePtr extend_node(const SearchNodePtr& node, const Edge& edge);

public:
    ShortestPathHeuristic(const AdjacencyMatrix& adj_matrix, Idx source_vertex_id);
    Triplet<CostType> operator()(Idx vertex_id) const;
};

#endif //TESTING_SHORTEST_PATH_HEURISTIC_H