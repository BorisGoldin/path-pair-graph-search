#ifndef BI_CRITERIA_PPA_BI_CRITERIA_PPA_H
#define BI_CRITERIA_PPA_BI_CRITERIA_PPA_H

#include <set>
#include <vector>
#include <list>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../Utils/BestFirstSearch.h"
#include "../SearchNodes/SearchNode.h"
#include "../SearchNodes/PathPair.h"

class PPA: public BestFirstSearch
{
private:
    using PPMinHeap = std::multiset<PathPairPtr, PathPair::less_than_full_costs>;
    using PPOpenMap = std::vector<std::list<PathPairPtr>>;
    using PPQueue   = std::pair<PPMinHeap, PPOpenMap>;

    Pair<EpsType>           eps;

    bool is_queue_empty(PPQueue& queue);
    PPMinHeap& get_min_heap(PPQueue& queue);
    PPOpenMap& get_open_map(PPQueue& queue);
    PathPairPtr pop_from_queue(PPQueue& queue);
    void insert_to_queue(PathPairPtr& pp, PPQueue& queue);

    bool check_if_dominated(const PathPairPtr& pp, Idx target_vertex_id, std::vector<CostType>& min_cost2_to_vertex);
    PathPairPtr extend_path_pair(const PathPairPtr& pp, const Edge& edge, Heuristic& heuristic);
    void insert(PathPairPtr& pp, PPQueue& queue);
    void merge_to_solutions(const PathPairPtr& pp, PathPair::SolutionsSet& solutions);

public:
    PPA(const AdjacencyMatrix& adj_matrix, Pair<EpsType> eps, LoggerPtr logger=nullptr);
    void operator()(const Idx source_vertex_id, const Idx target_vertex_id, SearchNode::SolutionsSet& solutions, Heuristic& heuristic);
};

#endif //BI_CRITERIA_PPA_BI_CRITERIA_PPA_H