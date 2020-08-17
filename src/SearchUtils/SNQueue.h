#ifndef SEARCH_UTILS_SN_QUEUE_H
#define SEARCH_UTILS_SN_QUEUE_H

#include <set>
#include <vector>
#include <list>
#include "../Utils/Definitions.h"
#include "../SearchUtils/SearchNode.h"

class SNQueue
{
private:
    using SNMinHeap = std::multiset<SearchNodePtr, SearchNode::less_than_full_costs>;
    using SNOpenMap = std::vector<std::list<SearchNodePtr>>;

    std::pair<SNMinHeap, SNOpenMap> queue;

public:
    SNQueue(size_t number_of_vertices);
    bool is_empty();
    SearchNodePtr pop();
    void insert(SearchNodePtr& pp);
};

#endif //SEARCH_UTILS_SN_QUEUE_H