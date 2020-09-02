#include "BOAStar.h"

BOAStar::BOAStar(const AdjacencyMatrix &adj_matrix, Pair<double> eps, const LoggerPtr &logger)
    : BestFirstSearch(adj_matrix, logger), eps(eps) {}

bool BOAStar::check_if_dominated(const SearchNodePtr &node,
                                 Idx target_vertex_id,
                                 std::vector<CostType> &min_path_cost2) {
    if (node->get_cost_until_now()[1] >= min_path_cost2[node->get_vertex_id()]) {
        LOG_INC_PATHS_PRUNED_BY_NODE();
        return true;

    } else if (((1 + this->eps[1]) * node->get_full_cost()[1]) >= min_path_cost2[target_vertex_id]) {
        LOG_INC_PATHS_PRUNED_BY_SOLUTION();
        return true;

    } else {
        return false;
    }
}

SearchNodePtr BOAStar::extend_node(const SearchNodePtr &node, const Edge edge, Heuristic &heuristic) {
    return SearchNode::extend(node, edge, heuristic(edge.target));
}

void BOAStar::insert(SearchNodePtr &node, SNQueue &queue) {
    queue.insert(node);
}

void BOAStar::merge_to_solutions(const SearchNodePtr &node, SearchNode::SolutionsSet &solutions) {
    solutions.push_back(node);
}

void BOAStar::operator()(Idx source_vertex_id, Idx target_vertex_id, SearchNode::SolutionsSet &solutions, Heuristic &heuristic) {
    this->log_search_start(source_vertex_id, target_vertex_id, "BOAStar", {this->eps[0], this->eps[1], 0});

    SNQueue                     open_queue(adj_matrix.get_number_of_vertices());
    std::vector<CostType>       min_path_cost2(adj_matrix.get_number_of_vertices()+1, MAX_COST);

    // Add source node to open queue
    SearchNodePtr source_node = std::make_shared<SearchNode>(source_vertex_id, nullptr, Pair<CostType>({0,0}), heuristic(source_vertex_id));
    this->insert(source_node, open_queue);

    while (open_queue.is_empty() == false) {
        LOG_INC_LOOP_COUNT();

        // Pop min from queue and process
        SearchNodePtr node = open_queue.pop();

        LOG_INC_PATHS_POPPED(node->get_vertex_id());

        // Check if search node is dominated
        if (this->check_if_dominated(node, target_vertex_id, min_path_cost2) == true) {
            continue;
        };
        min_path_cost2[node->get_vertex_id()] = node->get_cost_until_now()[1];


        // If target vertex merge to solutions
        if (node->get_vertex_id() == target_vertex_id) {
            this->merge_to_solutions(node, solutions);
            continue;
        }

        // Go over all neighbors and extend
        const std::list<Edge>& outgoing_edges = adj_matrix[node->get_vertex_id()];
        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {
            // Create new node
            SearchNodePtr neighbor = this->extend_node(node, *p_edge, heuristic);
            if (neighbor == nullptr) {
                continue;
            }

            // Check if search node is dominated
            if (this->check_if_dominated(neighbor, target_vertex_id, min_path_cost2) == true) {
                continue;
            };

            LOG_INC_PATHS_GENERATED(neighbor->get_vertex_id())

            // Add node to open list
            this->insert(neighbor, open_queue);
        }
        LOG_INC_PATHS_EXPANDED(node->get_vertex_id());
    }

    this->log_search_finish(solutions);
}
