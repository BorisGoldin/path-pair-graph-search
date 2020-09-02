#include "PPA.h"

PPA::PPA(const AdjacencyMatrix &adj_matrix, Pair<EpsType> eps, const LoggerPtr &logger)
    : BestFirstSearch(adj_matrix, logger), eps(eps) {}

bool PPA::check_if_dominated(const PathPairPtr &pp, Idx target_vertex_id, std::vector<CostType> &min_path_cost2) {
    if (pp->get_bottom_right()->get_cost_until_now()[1] >= min_path_cost2[pp->get_bottom_right()->get_vertex_id()]) {
        LOG_INC_PATHS_PRUNED_BY_NODE();
        return true;
    } else if (((1 + this->eps[1]) * pp->get_bottom_right()->get_full_cost()[1]) >= min_path_cost2[target_vertex_id]) {
        LOG_INC_PATHS_PRUNED_BY_SOLUTION();
        return true;
    } else {
        return false;
    }
}

PathPairPtr PPA::extend_path_pair(const PathPairPtr &pp, const Edge edge, Heuristic &heuristic) {
    return PathPair::extend(pp, edge, heuristic(edge.target));
}

void PPA::insert(PathPairPtr &pp, PPQueue &queue) {
    std::list<PathPairPtr> &relevant_pps = queue.get_open_pps(pp->get_vertex_id());
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
                queue.insert(pp);
            }
            return;
        }
    }
    queue.insert(pp);
}

void PPA::merge_to_solutions(const PathPairPtr &pp, PathPair::SolutionsSet &solutions) {
    for (auto iter = solutions.begin(); iter != solutions.end(); ++iter) {
        if (pp->update_nodes_by_merge_if_bounded(*iter, this->eps, true) == true) {
            return;
        }
    }
    solutions.push_back(pp);
}

void PPA::operator()(const Idx source_vertex_id,
                     const Idx target_vertex_id,
                     SearchNode::SolutionsSet &solutions,
                     Heuristic &heuristic) {
    this->log_search_start(source_vertex_id, target_vertex_id, "PPA", {this->eps[0], this->eps[1], 0});

    PPQueue                     open_queue(adj_matrix.get_number_of_vertices());
    std::vector<CostType>       min_path_cost2(adj_matrix.get_number_of_vertices()+1, MAX_COST);
    PathPair::SolutionsSet      pp_solutions;

    // Add source node to open queue
    SearchNodePtr source_node =
        std::make_shared<SearchNode>(source_vertex_id, nullptr, Pair<CostType>({0,0}), heuristic(source_vertex_id));
    PathPairPtr source_pp = std::make_shared<PathPair>(source_node, source_node);
    this->insert(source_pp, open_queue);

    while (open_queue.is_empty() == false) {
        LOG_INC_LOOP_COUNT();

        // Pop min from queue and process
        PathPairPtr pp = open_queue.pop();

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
        const std::list<Edge> &outgoing_edges = adj_matrix[pp->get_vertex_id()];

        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {
            // Create new node
            PathPairPtr neighbor_pp = this->extend_path_pair(pp, *p_edge, heuristic);

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
