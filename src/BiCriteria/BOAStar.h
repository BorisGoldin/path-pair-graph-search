#ifndef BI_CRITERIA_BOA_STAR_H
#define BI_CRITERIA_BOA_STAR_H

#include <vector>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../SearchUtils/BestFirstSearch.h"
#include "../SearchUtils/SearchNode.h"
#include "../SearchUtils/SNQueue.h"

class BOAStar: public BestFirstSearch
{
private:
    Pair<EpsType>           eps;
    
    void merge_to_solutions(const SearchNodePtr& node, SearchNode::SolutionsSet& solutions);
    bool check_if_dominated(const SearchNodePtr& node, Idx target_vertex_id, std::vector<CostType>& min_cost_to_vertex);
    SearchNodePtr extend_node(const SearchNodePtr& node, const Edge& edge, Heuristic& heuristic);
    void insert(SearchNodePtr& node, SNQueue& queue);

public:
    BOAStar(const AdjacencyMatrix& adj_matrix, Pair<EpsType> eps, LoggerPtr logger=nullptr);
    void operator()(Idx source_vertex_id, Idx target_vertex_id, SearchNode::SolutionsSet& solutions, Heuristic& heuristic);
};

#endif //BI_CRITERIA_BOA_STAR_H