#ifndef SEARCH_UTILS_MULTI_PP_QUEUE_H
#define SEARCH_UTILS_MULTI_PP_QUEUE_H

#include <set>
#include <vector>
#include <list>
#include "../Utils/Definitions.h"
#include "../SearchUtils/SearchNode.h"
#include "../SearchUtils/PathPair.h"
#include "../SearchUtils/PPQueue.h"

class MultiPPQueue
{
private:
    using PPMinHeap = std::multiset<PathPairPtr, PathPair::less_than_full_costs>;
    using PPOpenMap = std::vector<std::list<PathPairPtr>>;

    std::pair<PPMinHeap, PPOpenMap> queue;

public:
    PPQueue(size_t number_of_vertices);
    bool is_empty();
    PathPairPtr pop();
    void insert(PathPairPtr& pp);
    std::list<PathPairPtr>& get_open_pps(Idx vertex_id);
};

#endif //SEARCH_UTILS_MULTI_PP_QUEUE_H

