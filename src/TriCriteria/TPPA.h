#ifndef TRI_CRITERIA_TPPA_H
#define TRI_CRITERIA_TPPA_H

#include <set>
#include <vector>
#include <list>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../SearchUtils/SearchNode.h"
#include "../SearchUtils/PathPair.h"
#include "../SearchUtils/PPAInstanceSet.h"
#include "../BiCriteria/PPA.h"

class TPPA: public PPA
{
private:
    CostType eps3;

public:
    TPPA(const AdjacencyMatrix& adj_matrix, Triplet<EpsType> eps, LoggerPtr logger=nullptr) 
        : PPA(adj_matrix, {eps[0], eps[1]}, logger), eps3(eps[2]) {};
    void operator()(const Idx source_vertex_id, const Idx target_vertex_id, SearchNode::SolutionsSet& solutions, Heuristic& heuristic);
};

#endif //TRI_CRITERIA_TPPA_H