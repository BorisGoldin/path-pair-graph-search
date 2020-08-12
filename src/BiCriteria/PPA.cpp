#include "PPA.h"

PPA::PPA(const AdjacencyMatrix& adj_matrix, Pair<double> eps, LoggerPtr logger)
    : BestFirstSearch(adj_matrix, logger), eps(eps) {}

bool PPA::is_queue_empty(PPQueue& queue) {
    return PPA::get_min_heap(queue).empty();
}

PPA::PPMinHeap& PPA::get_min_heap(PPQueue& queue) {return queue.first;}

PPA::PPOpenMap& PPA::get_open_map(PPQueue& queue) {return queue.second;}

PathPairPtr PPA::pop_from_queue(PPQueue& queue) {
    // Pop from min heap
    PathPairPtr pp = *(PPA::get_min_heap(queue).begin());
    PPA::get_min_heap(queue).erase(PPA::get_min_heap(queue).begin());

    // Remove from open map
    std::list<PathPairPtr>& relevant_pps = PPA::get_open_map(queue)[pp->get_vertex_id()];    
    for (auto iter = relevant_pps.begin(); iter != relevant_pps.end(); ++iter) {
        if (pp == *iter) {
            relevant_pps.erase(iter);
            break;
        }
    }

    return pp;
}

void PPA::insert_to_queue(PathPairPtr& pp, PPQueue& queue) {
    PPA::get_min_heap(queue).insert(pp);
    PPA::get_open_map(queue)[pp->get_vertex_id()].push_back(pp);
}

bool PPA::check_if_dominated(const PathPairPtr& pp, Idx target_vertex_id, std::vector<CostType>& min_path_cost2) {
    if (pp->get_bottom_right()->get_cost_until_now()[1] >= min_path_cost2[pp->get_bottom_right()->get_vertex_id()]) {
        LOG_INC_PATHS_PRUNED_BY_NODE();
        return true;
    } else if ((1 + this->eps[1] * pp->get_bottom_right()->get_full_cost()[1] >= min_path_cost2[target_vertex_id])) {
        LOG_INC_PATHS_PRUNED_BY_SOLUTION();
        return true;
    } else {
        return false;
    }
}

PathPairPtr PPA::extend_path_pair(const PathPairPtr& pp, const Edge& edge, Heuristic& heuristic) {
    Triplet<CostType> heuristic_cost = heuristic(edge.target);
    return PathPair::extend(pp, edge, {heuristic_cost[0], heuristic_cost[1]});
}

void PPA::insert(PathPairPtr& pp, PPQueue& queue) {
    std::list<PathPairPtr> &relevant_pps = PPA::get_open_map(queue)[pp->get_vertex_id()];
    for (auto p_existing_pp = relevant_pps.begin(); p_existing_pp != relevant_pps.end(); ++p_existing_pp) {
        if ((*p_existing_pp)->is_active() == false) {
            continue;
        }
        if (pp->update_nodes_by_merge_if_bounded(*p_existing_pp, this->eps, true) == true) {
            // pp and *p_existing_pp were merged successfuly into pp
            if ((pp->get_top_left() != (*p_existing_pp)->get_top_left()) || 
                (pp->get_bottom_right() != (*p_existing_pp)->get_bottom_right())) {
                // merged_pp != (*p_existing_pp), therefore we remove (*p_existing_pp) and insert merged_pp instead 
                // (pp is not fully dominated by an opened pp)
                (*p_existing_pp)->deactivate();
                PPA::insert_to_queue(pp, queue);
            }
            return;
        }
    }
    PPA::insert_to_queue(pp, queue);
}

void PPA::merge_to_solutions(const PathPairPtr& pp, PathPair::SolutionsSet& solutions) {
    for (auto iter = solutions.begin(); iter != solutions.end(); ++iter) {
        if (pp->update_nodes_by_merge_if_bounded(*iter, this->eps, true) == true) {
            return;
        }
    }
    solutions.push_back(pp);
}

void PPA::operator()(const Idx source_vertex_id,
                     const Idx target_vertex_id,
                     SearchNode::SolutionsSet& solutions,
                     Heuristic& heuristic) {
    this->log_search_start(source_vertex_id, target_vertex_id, "PPA", {this->eps[0], this->eps[1], 0});

    PPMinHeap                   min_heap;
    PPOpenMap                   open_map(adj_matrix.get_number_of_vertices()+1, std::list<PathPairPtr>());
    PPQueue                     open_queue(min_heap, open_map);
    std::vector<CostType>       min_path_cost2(adj_matrix.get_number_of_vertices()+1, MAX_COST);
    PathPair::SolutionsSet      pp_solutions;

    // Add source node to open queue    
    SearchNodePtr source_node = 
        std::make_shared<SearchNode>(source_vertex_id, nullptr, Triplet<CostType>({0,0,0}), heuristic(source_vertex_id));
    PathPairPtr source_pp = std::make_shared<PathPair>(source_node, source_node);
    this->insert(source_pp, open_queue);

    while (PPA::is_queue_empty(open_queue) == false) {
        LOG_INC_LOOP_COUNT();

        // Pop min from queue and process            
        PathPairPtr pp = PPA::pop_from_queue(open_queue);

        // Optimization: PathPairs are being deactivated instead of being removed so we skip them.
        if (pp->is_active() == false) {
            continue;
        }

        LOG_INC_PATHS_POPPED(pp->get_vertex_id());

        // Check if path pair is dominated
        if (this->check_if_dominated(pp, target_vertex_id, min_path_cost2) == true) {
            continue;
        };

        // Update to new minimal cost (pp is minimal because it wasn't dominated)
        min_path_cost2[pp->get_vertex_id()] = pp->get_bottom_right()->get_cost_until_now()[1];

        // If target vertex merge to solutions
        if (pp->get_vertex_id() == target_vertex_id) {
            this->merge_to_solutions(pp, pp_solutions);
            continue;
        }

        // Go over all neighbors and extend 
        const std::vector<Edge>& outgoing_edges = adj_matrix[pp->get_vertex_id()];
        for (size_t j = 0; j < outgoing_edges.size(); ++j) {
            // Create new node
            PathPairPtr neighbor_pp = this->extend_path_pair(pp, outgoing_edges[j], heuristic);

            if (neighbor_pp == nullptr) {
                continue;
            }
            LOG_INC_PATHS_GENERATED(neighbor_pp->get_vertex_id())

            // Check if search node is dominated
            if (this->check_if_dominated(neighbor_pp, target_vertex_id, min_path_cost2) == true) {
                continue;
            };

            // Add node to open list
            this->insert(neighbor_pp, open_queue);
        }
        LOG_INC_PATHS_EXPANDED(pp->get_vertex_id());
    }
    solutions = PathPair::solutions_from_path_pair_solutions(pp_solutions);

    this->log_search_finish(solutions);
}
