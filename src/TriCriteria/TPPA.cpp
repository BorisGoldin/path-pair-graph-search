#include "TPPA.h"

void TPPA::operator()(const Idx source_vertex_id,
                      const Idx target_vertex_id,
                      SearchNode::SolutionsSet& solutions,
                      Heuristic& heuristic) {
    this->log_search_start(source_vertex_id, target_vertex_id, "TPPA", {this->eps[0], this->eps[1], this->eps3});

    PPAInstanceSet  ppa_instance_set(adj_matrix.get_number_of_vertices(), this->eps3);

    // Add source node to open queue    
    SearchNodePtr source_node = 
        std::make_shared<SearchNode>(source_vertex_id, nullptr, Triplet<CostType>({0,0,0}), heuristic(source_vertex_id));
    PathPairPtr source_pp = std::make_shared<PathPair>(source_node, source_node);

    PPAInstancePtr init_ppa_instance = ppa_instance_set.get_matching_instance(source_pp);
    this->insert(source_pp, init_ppa_instance->open_queue);

    while (ppa_instance_set.is_empty() == false) {
        LOG_INC_LOOP_COUNT();

        // Pop min from queue and process            
        std::pair<PathPairPtr, PPAInstancePtr> search_context = ppa_instance_set.pop();
        PathPairPtr     pp = search_context.first;
        PPAInstancePtr  ppa_instance = search_context.second;

        // Optimization: PathPairs are being deactivated instead of being removed so we skip them.
        if (pp->is_active() == false) {
            continue;
        }

        LOG_INC_PATHS_POPPED(pp->get_vertex_id());

        // Check if path pair is dominated in pp ppa_instance
        if (this->check_if_dominated(pp, target_vertex_id, ppa_instance->min_path_cost2) == true) {
            continue;
        };

        // Update to new minimal cost (pp is minimal because it wasn't dominated) in pp ppa_instance
        ppa_instance->min_path_cost2[pp->get_vertex_id()] = pp->get_bottom_right()->get_cost_until_now()[1];

        // If target vertex merge to solutions of pp ppa_instance
        if (pp->get_vertex_id() == target_vertex_id) {
            this->merge_to_solutions(pp, ppa_instance->solutions);
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

            // Check if search node is dominated in pp ppa_instance
            if (this->check_if_dominated(neighbor_pp, target_vertex_id, ppa_instance->min_path_cost2) == true) {
                continue;
            };

            // Check if split is needed
            if (ppa_instance_set.is_split_needed(neighbor_pp)) {
                PathPairPtr top_left_pp = std::make_shared<PathPair>(neighbor_pp->get_top_left(), neighbor_pp->get_top_left());
                PathPairPtr bottom_right_pp = std::make_shared<PathPair>(neighbor_pp->get_bottom_right(), neighbor_pp->get_bottom_right());

                PPAInstancePtr top_left_ppa_instance = ppa_instance_set.get_matching_instance(top_left_pp);
                PPAInstancePtr bottom_right_ppa_instance = ppa_instance_set.get_matching_instance(bottom_right_pp);

                if (this->check_if_dominated(neighbor_pp, target_vertex_id, top_left_ppa_instance->min_path_cost2) == false) {
                    this->insert(top_left_pp, top_left_ppa_instance->open_queue);
                }
                if (this->check_if_dominated(neighbor_pp, target_vertex_id, bottom_right_ppa_instance->min_path_cost2) == false) {
                    this->insert(bottom_right_pp, bottom_right_ppa_instance->open_queue);
                }

            } else {
                PPAInstancePtr neighbor_ppa_instance = ppa_instance_set.get_matching_instance(neighbor_pp);
                if (this->check_if_dominated(neighbor_pp, target_vertex_id, neighbor_ppa_instance->min_path_cost2) == false) {
                    this->insert(neighbor_pp, neighbor_ppa_instance->open_queue);
                }
            }
        }
        LOG_INC_PATHS_EXPANDED(pp->get_vertex_id());
    }

    solutions = PathPair::solutions_from_path_pair_solutions(ppa_instance_set.get_solutions());

    this->log_search_finish(solutions);
}
