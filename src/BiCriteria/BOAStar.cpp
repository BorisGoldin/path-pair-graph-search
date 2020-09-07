#include <memory>

#include "BOAStar.h"

BOAStar::BOAStar(const AdjacencyMatrix &adj_matrix, Pair<double> eps, const LoggerPtr logger) :
	adj_matrix(adj_matrix), eps(eps), logger(logger) {}

void BOAStar::operator()(size_t source, size_t target, Heuristic &heuristic, SolutionSet &solutions) {
    this->start_logging(source, target);

    MapNodePtr node;
    MapNodePtr next;

    // Saving all the unused MapNodePtrs in a vector improves performace for some reason
    std::vector<MapNodePtr> closed;

    // Vector to hold mininum cost of 2nd criteria per node
    std::vector<size_t> min_g2(this->adj_matrix.size()+1, MAX_COST);

    // Init open heap
    MapNode::more_than_full_cost more_than;
    std::vector<MapNodePtr> open;
    std::make_heap(open.begin(), open.end(), more_than);

    node = std::make_shared<MapNode>(source, Triplet<size_t>({0,0,0}), heuristic(source));
    open.push_back(node);
    std::push_heap(open.begin(), open.end(), more_than);


    while (open.empty() == false) {
        // Pop min from queue and process
        std::pop_heap(open.begin(), open.end(), more_than);
        node = open.back();
        open.pop_back();

        if ((node->g[1] >= min_g2[node->id]) ||
            (((1+this->eps[1])*(node->g[1]+node->h[1])) >= min_g2[target])){
            closed.push_back(node);
            continue;
        }

        min_g2[node->id] = node->g[1];

        if (node->id == target) {
            solutions.push_back(node);
            continue;
        }

        const std::vector<Edge> &outgoing_edges = adj_matrix[node->id];
        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {

            size_t next_id = p_edge->target;
            Triplet<size_t> next_g = {node->g[0]+p_edge->cost[0], node->g[1]+p_edge->cost[1], 0};
            Triplet<size_t> next_h = heuristic(next_id);

            if ((next_g[1] >= min_g2[next_id]) ||
                (((1+this->eps[1])*(next_g[1]+next_h[1])) >= min_g2[target])) {
                continue;
            }

            next = std::make_shared<MapNode>(next_id, next_g, next_h, node);

            // If not dominated push to queue
            open.push_back(next);
            std::push_heap(open.begin(), open.end(), more_than);

            closed.push_back(node);
        }
    }

    this->end_logging(solutions);
}

void BOAStar::start_logging(size_t source, size_t target) {
    std::stringstream start_info_json;
    start_info_json
        << "{\n"
        <<      "\t\"name\": \"BOAStar\",\n"
        <<      "\t\"eps\": " << this->eps << "\n"
        << "}";

    LOG_START_SEARCH(*this->logger, source, target, start_info_json.str());
}

void BOAStar::end_logging(SolutionSet &solutions) {
    std::stringstream finish_info_json;
    finish_info_json
        << "{\n"
        <<      "\t\"solutions\": [";

    size_t solutions_count = 0;
    for (auto solution = solutions.begin(); solution != solutions.end(); ++solution) {
        if (solution != solutions.begin()) {
            finish_info_json << ",";
        }
        finish_info_json << "\n\t\t" << **solution;
        solutions_count++;
    }

    finish_info_json
        <<      "\n\t],\n"
        <<      "\t\"amount_of_solutions\": " << solutions_count << "\n"
        << "}" <<std::endl;

    LOG_FINISH_SEARCH(*(this->logger), finish_info_json.str());
}