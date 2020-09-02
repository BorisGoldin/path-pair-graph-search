#ifndef BI_CRITERIA_PPA_H
#define BI_CRITERIA_PPA_H

#include <vector>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../SearchUtils/BestFirstSearch.h"
#include "../SearchUtils/SearchNode.h"
#include "../SearchUtils/PathPair.h"
#include "../SearchUtils/PPQueue.h"

class PPA: public BestFirstSearch
{
protected:
    Pair<EpsType>           eps;

    bool check_if_dominated(const PathPairPtr &pp, Idx target_vertex_id, std::vector<CostType> &min_cost2_to_vertex);
    PathPairPtr extend_path_pair(const PathPairPtr &pp, const Edge edge, Heuristic &heuristic);
    void insert(PathPairPtr &pp, PPQueue &queue);
    void merge_to_solutions(const PathPairPtr &pp, PathPair::SolutionsSet &solutions);

public:
    PPA(const AdjacencyMatrix &adj_matrix, Pair<EpsType> eps, const LoggerPtr &logger=nullptr);
    void operator()(const Idx source_vertex_id, const Idx target_vertex_id, SearchNode::SolutionsSet &solutions, Heuristic &heuristic);
};

#endif //BI_CRITERIA_PPA_H