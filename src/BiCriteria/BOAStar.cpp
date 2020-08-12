#include <set>
#include <vector>
#include "BOAStar.h"

BOAStar::BOAStar(const AdjacencyMatrix& adj_matrix, Pair<double> eps, LoggerPtr logger) 
    : BestFirstSearch(adj_matrix, logger), eps(eps) {}

bool BOAStar::is_queue_empty(SNQueue& queue) {
    return BOAStar::get_min_heap(queue).empty();
}

BOAStar::SNMinHeap& BOAStar::get_min_heap(SNQueue& queue) {return queue.first;}

BOAStar::SNOpenMap& BOAStar::get_open_map(SNQueue& queue) {return queue.second;}

SearchNodePtr BOAStar::pop_from_queue(SNQueue& queue) {
    // Pop from min heap
    SearchNodePtr node = *(BOAStar::get_min_heap(queue).begin());
    BOAStar::get_min_heap(queue).erase(BOAStar::get_min_heap(queue).begin());

    // Remove from open map
    std::list<SearchNodePtr>& relevant_search_nodes = BOAStar::get_open_map(queue)[node->get_vertex_id()];
    for (auto iter = relevant_search_nodes.begin(); iter != relevant_search_nodes.end(); ++iter) {
        if (node == *iter) {
            relevant_search_nodes.erase(iter);
            break;
        }
    }
    
    return node;
}

void BOAStar::insert_to_queue(SearchNodePtr& node, SNQueue& queue) {
    BOAStar::get_min_heap(queue).insert(node);
    BOAStar::get_open_map(queue)[node->get_vertex_id()].push_back(node);
}

bool BOAStar::check_if_dominated(const SearchNodePtr& node, 
                                 Idx target_vertex_id, 
                                 std::vector<CostType>& min_path_cost2) {
    if (node->get_cost_until_now()[1] >= min_path_cost2[node->get_vertex_id()]) {
        LOG_INC_PATHS_PRUNED_BY_NODE();
        return true;
    } else if ((1 + this->eps[1] * node->get_full_cost()[1] >= min_path_cost2[target_vertex_id])) {
        LOG_INC_PATHS_PRUNED_BY_SOLUTION();
        return true;
    } else {
        return false;
    }
}

SearchNodePtr BOAStar::extend_node(const SearchNodePtr& node, const Edge& edge, Heuristic& heuristic) {
    return SearchNode::extend(node, edge, heuristic(edge.target));
}

void BOAStar::insert(SearchNodePtr& node, SNQueue& queue) {
    BOAStar::insert_to_queue(node, queue);
}

void BOAStar::merge_to_solutions(const SearchNodePtr& node, SearchNode::SolutionsSet& solutions) {
    solutions.push_back(node);
}

void BOAStar::operator()(Idx source_vertex_id, Idx target_vertex_id, SearchNode::SolutionsSet& solutions, Heuristic& heuristic) {
    this->log_search_start(source_vertex_id, target_vertex_id, "BOAStar", {this->eps[0], this->eps[1], 0});

    SNMinHeap                   min_heap;
    SNOpenMap                   open_map(adj_matrix.get_number_of_vertices()+1, std::list<SearchNodePtr>());
    SNQueue                     open_queue(min_heap, open_map);
    std::vector<CostType>       min_path_cost2(adj_matrix.get_number_of_vertices()+1, MAX_COST);

    // Add source node to open queue    
    SearchNodePtr source_node = std::make_shared<SearchNode>(source_vertex_id, nullptr, Triplet<CostType>({0,0,0}), heuristic(source_vertex_id));
    BOAStar::insert_to_queue(source_node, open_queue);

    while (BOAStar::is_queue_empty(open_queue) == false) {
        LOG_INC_LOOP_COUNT();

        // Pop min from queue and process
        SearchNodePtr node = BOAStar::pop_from_queue(open_queue);            

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
        const std::vector<Edge>& outgoing_edges = adj_matrix[node->get_vertex_id()];
        for (size_t i = 0; i < outgoing_edges.size(); ++i) {
            // Create new node
            SearchNodePtr neighbor = this->extend_node(node, outgoing_edges[i], heuristic);
            if (neighbor == nullptr) {
                continue;
            }
            LOG_INC_PATHS_GENERATED(neighbor->get_vertex_id())

            // Check if search node is dominated
            if (this->check_if_dominated(neighbor, target_vertex_id, min_path_cost2) == true) {
                continue;
            };

            // Add node to open list
            BOAStar::insert_to_queue(neighbor, open_queue);
        }
        LOG_INC_PATHS_EXPANDED(node->get_vertex_id());
    }

    this->log_search_finish(solutions);
}
