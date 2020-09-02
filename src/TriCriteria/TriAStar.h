#ifndef BI_CRITERIA_TRI_A_STAR_H
#define BI_CRITERIA_TRI_A_STAR_H

#if 0

#include <vector>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../SearchUtils/BestFirstSearch.h"
#include "../SearchUtils/SearchNode.h"
#include "../SearchUtils/SNQueue.h"

class TriAStar: public BestFirstSearch
{
private:
    Triplet<EpsType>           eps;

    void merge_to_solutions(const SearchNodePtr& node, SearchNode::SolutionsSet& solutions) {
    	solutions.push_back(node);
	};

    bool check_if_dominated(const SearchNodePtr& node, SNQueue& closed_queue, SearchNode::SolutionsSet& solutions) {
    	bool is_dom_by_sol = false;
    	if (solutions.empty() == false) {
			for (auto iter = solutions.begin(); iter != solutions.end(); ++iter) {
				if (node->is_dominated_by(*iter, this->eps, true))
					is_dom_by_sol = true;
					break;
			}
    	}

    	bool is_dom_by_closed = false;
		std::list<SearchNodePtr>& closed_nodes = closed_queue.get_open_for_node(node);
		if (closed_nodes.empty() == false) {
			//go over closed list and see if dominated or dominating
			for (auto iter = closed_nodes.begin(); iter != closed_nodes.end(); ++iter) {
				if (node->is_dominated_by(*iter, this->eps)) {
					is_dom_by_closed = true;
					break;
				}
				if ((*iter)->is_dominated_by(node, this->eps)) {
					break;
				}
			}
		}

    	return (is_dom_by_sol || is_dom_by_closed);
	};

    SearchNodePtr extend_node(const SearchNodePtr& node, const Edge& edge, Heuristic& heuristic) {
	    return SearchNode::extend(node, edge, heuristic(edge.target));
	};

    void insert(SearchNodePtr& node, SNQueue& queue) {
	    queue.insert(node);
	};

public:
    TriAStar(const AdjacencyMatrix& adj_matrix, Triplet<EpsType> eps, LoggerPtr logger=nullptr)
    	: BestFirstSearch(adj_matrix, logger), eps(eps) {};

    void operator()(Idx source_vertex_id, Idx target_vertex_id, SearchNode::SolutionsSet& solutions, Heuristic& heuristic) {
    this->log_search_start(source_vertex_id, target_vertex_id, "TriAStar", {this->eps});

    SNQueue                     open_queue(adj_matrix.get_number_of_vertices());
    SNQueue						closed_queue(adj_matrix.get_number_of_vertices());
    // Add source node to open queue
    SearchNodePtr source_node = std::make_shared<SearchNode>(source_vertex_id, nullptr, Triplet<CostType>({0,0,0}), heuristic(source_vertex_id));
	    this->insert(source_node, open_queue);

	    while (open_queue.is_empty() == false) {
	        LOG_INC_LOOP_COUNT();

	        // Pop min from queue and process
	        SearchNodePtr node = open_queue.pop();

	        LOG_INC_PATHS_POPPED(node->get_vertex_id());

	        // Check if search node is dominated
	        if (this->check_if_dominated(node, closed_queue, solutions) == true) {
	            continue;
	        };


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
	            if (this->check_if_dominated(neighbor, closed_queue, solutions) == true) {
	                continue;
	            };

	            // Add node to open list
	            this->insert(neighbor, open_queue);
	        }
	        LOG_INC_PATHS_EXPANDED(node->get_vertex_id());
	       	this->insert(node, closed_queue);
	    }

	    this->log_search_finish(solutions);
	};

};

#endif

#endif //BI_CRITERIA_TRI_A_STAR_H