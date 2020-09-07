#include <limits>
#include <memory>

#include "ShortestPathHeuristic.h"

ShortestPathHeuristic::ShortestPathHeuristic(size_t source, size_t graph_size, const AdjacencyMatrix &adj_matrix)
    : source(source), all_nodes(graph_size+1, nullptr) {
    size_t i = 0;
    for (auto node_iter = this->all_nodes.begin(); node_iter != this->all_nodes.end(); node_iter++) {
        *node_iter = std::make_shared<MapNode>(i++, Triplet<size_t>({0,0,0}), Triplet<size_t>({MAX_COST,MAX_COST,MAX_COST}));
    }

    compute(0, adj_matrix);
    compute(1, adj_matrix);
    compute(2, adj_matrix);
}


Triplet<size_t> ShortestPathHeuristic::operator()(size_t node_id) {
    return this->all_nodes[node_id]->h;
}


void ShortestPathHeuristic::compute(size_t cost_idx, const AdjacencyMatrix &adj_matrix) {
    for (auto node_iter = this->all_nodes.begin(); node_iter != this->all_nodes.end(); node_iter++) {
        (*node_iter)->h[cost_idx] = MAX_COST;
    }

    MapNodePtr node;
    MapNodePtr next;

    // Init open heap
    MapNode::more_than_specific_heurisitic_cost more_than(0);
    std::vector<MapNodePtr> open;
    std::make_heap(open.begin(), open.end(), more_than);

    this->all_nodes[this->source]->h[cost_idx] = 0;
    open.push_back(this->all_nodes[this->source]);
    std::push_heap(open.begin(), open.end(), more_than);


    while (open.empty() == false) {
        // Pop min from queue and process
        std::pop_heap(open.begin(), open.end(), more_than);
        node = open.back();
        open.pop_back();

        const std::vector<Edge> &outgoing_edges = adj_matrix[node->id];
        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {

            next = this->all_nodes[p_edge->target];

            if (next->h[cost_idx] <= (node->h[cost_idx] + p_edge->cost[cost_idx])) {
                continue;
            }

            // If not dominated push to queue
            next->h[cost_idx] = node->h[cost_idx] + p_edge->cost[cost_idx];
            open.push_back(next);
            std::push_heap(open.begin(), open.end(), more_than);
        }
    }
}
