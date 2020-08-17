#ifndef SEARCH_UTILS_PPA_INSTANCE_SET_H
#define SEARCH_UTILS_PPA_INSTANCE_SET_H

#include <set>
#include <vector>
#include <algorithm>
#include "../Utils/Definitions.h"
#include "../SearchUtils/SearchNode.h"
#include "../SearchUtils/PathPair.h"
#include "../SearchUtils/PPQueue.h"

struct PPAInstance;
using PPAInstancePtr = std::shared_ptr<PPAInstance>;

struct PPAInstance
{
	CostType 				cost3_min;
	CostType 				cost3_max;
	PPQueue 				open_queue;
	std::vector<CostType>	min_path_cost2;
	PathPair::SolutionsSet  solutions;

	PPAInstance(CostType cost3_min,
				CostType cost3_max,
				PPQueue open_queue,
				std::vector<CostType> min_path_cost2,
				PathPair::SolutionsSet solutions)
		  : cost3_min(cost3_min),
			cost3_max(cost3_max),
			open_queue(open_queue),
			min_path_cost2(min_path_cost2),
			solutions(solutions) {};
	bool can_extend(CostType cost3_1, CostType cost3_2, EpsType eps3);

	struct less_than_min_cost {
        bool operator()(const PPAInstancePtr a, const PPAInstancePtr b) const;
    };
};

class PPAInstanceSet
{
private:
	EpsType						eps3;
	size_t						number_of_vertices;
	std::set<PPAInstancePtr> 	ppa_instances;

public:
    PPAInstanceSet(size_t number_of_vertices, EpsType eps3)
    	: number_of_vertices(number_of_vertices), eps3(eps3) {};
    bool is_empty();
    std::pair<PathPairPtr, PPAInstancePtr> pop();
    bool is_split_needed(PathPairPtr pp);
    PPAInstancePtr get_matching_instance(PathPairPtr pp);
    PathPair::SolutionsSet get_solutions();
};

#endif //SEARCH_UTILS_PPA_INSTANCE_SET_H

