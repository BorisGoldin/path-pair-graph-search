#include "ShortestPathHeuristic.h"

ShortestPathHeuristic::ShortestPathHeuristic(const AdjacencyMatrix& adj_matrix,
                                             Idx source_vertex_id)
                                                : adj_matrix(adj_matrix),
                                                  shortest_path_cost(adj_matrix.get_number_of_vertices()+1,
                                                                     Triplet<CostType>({MAX_COST, MAX_COST, MAX_COST})),
                                                  source_vertex_id(source_vertex_id) {
    for (size_t i = 0; i < AMOUNT_OF_COST_FUNCTIONS; ++i) {
        this->compute(i);
    }
}


void ShortestPathHeuristic::compute(size_t cost_idx) {
    SearchNode::less_than_specific_cost_until_now   less_than_specific_cost_until_now(cost_idx);
    SearchNodePtrsSortedByCost                      open(less_than_specific_cost_until_now);

    this->shortest_path_cost[this->source_vertex_id][cost_idx] = 0;

    SearchNodePtr source_node = std::make_shared<SearchNode>(this->source_vertex_id, nullptr, Triplet<CostType>{0,0,0});
    open.insert(source_node);

    while (open.empty() == false) {
        // Pop min from queue and process
        SearchNodePtr node = *(open.begin());
        open.erase(open.begin());

        const std::list<Edge>& outgoing_edges = adj_matrix[node->get_vertex_id()];
        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {
            // Create new node
            SearchNodePtr neighbor = this->extend_node(node, *p_edge);
            if (neighbor == nullptr) {
                continue;
            }

            // Check if search node is dominated
            if (this->shortest_path_cost[neighbor->get_vertex_id()][cost_idx] <=
                    neighbor->get_cost_until_now()[cost_idx]) {
                continue;
            };

            this->shortest_path_cost[neighbor->get_vertex_id()][cost_idx] = neighbor->get_cost_until_now()[cost_idx];
            // Add node to open list
            open.insert(neighbor);
        }
    }
}


SearchNodePtr ShortestPathHeuristic::extend_node(const SearchNodePtr& node, const Edge& edge) {
    if ((edge.source != node->get_vertex_id()) ||
        ((node->get_parent() != nullptr) && (node->get_parent()->get_vertex_id() == edge.target))) {
        return nullptr;
    }

    Triplet<CostType> current_cost = node->get_cost_until_now();
    Triplet<CostType> new_cost = {current_cost[0] + edge.cost[0],
                                  current_cost[1] + edge.cost[1],
                                  current_cost[2] + edge.cost[2]};

    return std::make_shared<SearchNode>(edge.target, node, new_cost, Triplet<CostType>({0,0,0}));
}


Triplet<CostType> ShortestPathHeuristic::operator()(Idx vertex_id) const {
    return Triplet<CostType>({this->shortest_path_cost[vertex_id][0],
                              this->shortest_path_cost[vertex_id][1],
                              this->shortest_path_cost[vertex_id][2]});
}