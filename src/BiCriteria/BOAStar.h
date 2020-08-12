#ifndef BI_CRITERIA_BOA_STAR_H
#define BI_CRITERIA_BOA_STAR_H

#include <set>
#include <vector>
#include <list>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../Utils/BestFirstSearch.h"
#include "../SearchNodes/SearchNode.h"

class BOAStar: public BestFirstSearch
{
private:
    using SNMinHeap = std::multiset<SearchNodePtr, SearchNode::less_than_full_costs>;
    using SNOpenMap = std::vector<std::list<SearchNodePtr>>;
    using SNQueue   = std::pair<SNMinHeap, SNOpenMap>;

    Pair<EpsType>           eps;

    bool is_queue_empty(SNQueue& queue);
    SNMinHeap& get_min_heap(SNQueue& queue);
    SNOpenMap& get_open_map(SNQueue& queue);
    SearchNodePtr pop_from_queue(SNQueue& queue);
    void insert_to_queue(SearchNodePtr& pp, SNQueue& queue);
    
    void merge_to_solutions(const SearchNodePtr& node, SearchNode::SolutionsSet& solutions);
    bool check_if_dominated(const SearchNodePtr& node, Idx target_vertex_id, std::vector<CostType>& min_cost_to_vertex);
    SearchNodePtr extend_node(const SearchNodePtr& node, const Edge& edge, Heuristic& heuristic);
    void insert(SearchNodePtr& node, SNQueue& queue);

public:
    BOAStar(const AdjacencyMatrix& adj_matrix, Pair<EpsType> eps, LoggerPtr logger=nullptr);
    void operator()(Idx source_vertex_id, Idx target_vertex_id, SearchNode::SolutionsSet& solutions, Heuristic& heuristic);
};

#endif //BI_CRITERIA_BOA_STAR_H