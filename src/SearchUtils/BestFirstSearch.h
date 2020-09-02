#ifndef SEARCH_UTILS_BEST_FIRST_SEARCH_H
#define SEARCH_UTILS_BEST_FIRST_SEARCH_H

#include <sstream>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "SearchNode.h"
#include "PathPair.h"

class BestFirstSearch
{
protected:
    using GeneralSearchNodePtr = SearchNodePtr;

    const AdjacencyMatrix   &adj_matrix;
    const LoggerPtr         logger = nullptr;

    #if (DEBUG >= 2)
    size_t                  loop_count = 0;
    size_t                  num_of_paths_pruned_by_node = 0;
    size_t                  num_of_paths_pruned_by_solution = 0;
    std::map<Idx, size_t>   num_of_paths_generated;
    std::map<Idx, size_t>   num_of_paths_popped;
    std::map<Idx, size_t>   num_of_paths_expanded;
    #endif

    void log_search_start(Idx source_vertex_id, Idx target_vertex_id, std::string name, Triplet<EpsType> eps) {
    #if (DEBUG >= 2)
        this->loop_count = 0;
        this->num_of_paths_pruned_by_node = 0;
        this->num_of_paths_pruned_by_solution = 0;
        this->num_of_paths_generated.clear();
        this->num_of_paths_popped.clear();
        this->num_of_paths_expanded.clear();
    #endif
        std::stringstream start_info_json;
        start_info_json
            << "{\n"
            <<      "\t\"name\": \"" << name << "\",\n"
            <<      "\t\"eps\": " << eps << ",\n"
            << "}";
        LOG_START_SEARCH(*this->logger, source_vertex_id, target_vertex_id, start_info_json.str());
    };

    void log_search_finish(SearchNode::SolutionsSet solutions) {
        std::stringstream finish_info_json;
        finish_info_json
            << "{" << std::endl;

        #if (DEBUG >= 2)
            finish_info_json
                <<      "\t\"solutions\": [" << std::endl;
            size_t solutions_count = 0;
            for (auto solution = solutions.begin(); solution != solutions.end(); ++solution) {
                finish_info_json << "\t\t" << **solution << "," << std::endl;
                solutions_count++;
            }

            finish_info_json
                <<      "\t],\n"
                <<      "\t\"amount_of_solutions\": " << solutions_count << "," << std::endl;

            size_t sum_num_of_paths_generated = 0;
            size_t sum_num_of_paths_popped  = 0;
            size_t sum_num_of_paths_expanded = 0;

            for (auto iter = num_of_paths_generated.begin(); iter != num_of_paths_generated.end(); ++iter) {
                sum_num_of_paths_generated += iter->second;
            }
            for (auto iter = num_of_paths_popped.begin(); iter != num_of_paths_popped.end(); ++iter) {
                sum_num_of_paths_popped += iter->second;
            }
            for (auto iter = num_of_paths_expanded.begin(); iter != num_of_paths_expanded.end(); ++iter) {
                sum_num_of_paths_expanded += iter->second;
            }

            finish_info_json
                <<      "\t\"loop_count\": " << this->loop_count << ",\n"
                <<      "\t\"num_of_paths_pruned_by_node\": " << this->num_of_paths_pruned_by_node << ",\n"
                <<      "\t\"num_of_paths_pruned_by_solution\": " << this->num_of_paths_pruned_by_solution << ",\n"
                <<      "\t\"sum_num_of_paths_generated\": " << sum_num_of_paths_generated << ",\n"
                <<      "\t\"sum_num_of_paths_popped\": " << sum_num_of_paths_popped << ",\n"
                <<      "\t\"sum_num_of_paths_expanded\": " << sum_num_of_paths_expanded << "," << std::endl;
        #endif

        finish_info_json
            << "}";

        LOG_FINISH_SEARCH(*(this->logger), finish_info_json.str());
    }


public:
    BestFirstSearch(const AdjacencyMatrix &adj_matrix, const LoggerPtr &logger=nullptr) : adj_matrix(adj_matrix), logger(logger) {};
    void operator()(const Idx source_vertex_id, const  Idx target_vertex_id, SearchNode::SolutionsSet &solutions, Heuristic &heuristic);
};

#if (DEBUG >= 2)
#define LOG_INC_LOOP_COUNT()                (++(this->loop_count));
#define LOG_INC_PATHS_PRUNED_BY_NODE()      (++(this->num_of_paths_pruned_by_node));
#define LOG_INC_PATHS_PRUNED_BY_SOLUTION()  (++(this->num_of_paths_pruned_by_solution));
#define LOG_INC_PATHS_GENERATED(vertex_id)  (++(this->num_of_paths_generated[(vertex_id)]));
#define LOG_INC_PATHS_POPPED(vertex_id)     (++(this->num_of_paths_popped[(vertex_id)]));
#define LOG_INC_PATHS_EXPANDED(vertex_id)   (++(this->num_of_paths_expanded[(vertex_id)]));
#else
#define LOG_INC_LOOP_COUNT()                ;
#define LOG_INC_PATHS_PRUNED_BY_NODE()      ;
#define LOG_INC_PATHS_PRUNED_BY_SOLUTION()  ;
#define LOG_INC_PATHS_GENERATED(vertex_id)  ;
#define LOG_INC_PATHS_POPPED(vertex_id)     ;
#define LOG_INC_PATHS_EXPANDED(vertex_id)   ;
#endif

#endif //SEARCH_UTILS_BEST_FIRST_SEARCH_H