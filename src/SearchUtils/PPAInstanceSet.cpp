#include "PPAInstanceSet.h"

bool PPAInstance::can_extend(CostType cost3_1, CostType cost3_2, EpsType eps3) {
	CostType extended_min = std::min({this->cost3_min, cost3_1, cost3_2});
	CostType extended_max = std::max({this->cost3_max, cost3_1, cost3_2});

	// Extention is possible if instance is still eps3 bounded after extention
	return ((extended_min * (1 + eps3)) >= extended_max);
}

bool PPAInstance::less_than_min_cost::operator()(const PPAInstancePtr a, const PPAInstancePtr b) const {
	return (a->cost3_min < b->cost3_min);
}

bool PPAInstanceSet::is_empty() {
	for (auto instance = this->ppa_instances.begin(); instance != this->ppa_instances.end(); ++instance) {
		if ((*instance)->open_queue.is_empty() == false) {
			return false;
		}
	}
	return true;
}

std::pair<PathPairPtr, PPAInstancePtr> PPAInstanceSet::pop() {
	PPAInstancePtr instance_to_pop = nullptr;
	for (auto instance = this->ppa_instances.begin(); instance != this->ppa_instances.end(); ++instance) {
		if ((instance_to_pop == nullptr) || 
			PathPair::less_than_full_costs()((*instance)->open_queue.peek(), instance_to_pop->open_queue.peek())) {
			instance_to_pop = *instance;
		}
	}
	return {instance_to_pop->open_queue.pop(), instance_to_pop};
}

bool PPAInstanceSet::is_split_needed(PathPairPtr pp) {
	CostType pp_top_left_cost3 		= pp->get_top_left()->get_cost_until_now()[2];
	CostType pp_bottom_right_cost3 	= pp->get_bottom_right()->get_cost_until_now()[2];
    CostType pp_cost3_min 			= std::min(pp_top_left_cost3, pp_bottom_right_cost3);
    CostType pp_cost3_max 			= std::max(pp_top_left_cost3, pp_bottom_right_cost3);

    if ((pp_cost3_min * (1 + this->eps3)) <= pp_cost3_max) {
		// CASE 0: Split if not bounded
		// [     - eps3 -      ]
		return true;
    }

    for (auto instance = this->ppa_instances.begin(); instance != this->ppa_instances.end(); ++instance) {
		auto next_instance = std::next(instance, 1);

    	CostType current_cost3_min = (*instance)->cost3_min;
		CostType current_cost3_max = (*instance)->cost3_max;    	
    	CostType next_cost3_min;
		CostType next_cost3_max;

		if (next_instance == this->ppa_instances.end()) {
			next_cost3_min = MAX_COST;
			next_cost3_max = MAX_COST;
		} else {
			next_cost3_min = (*next_instance)->cost3_min;
			next_cost3_max = (*next_instance)->cost3_max;
		}

		if ((current_cost3_min <= pp_cost3_min) && (pp_cost3_min <= current_cost3_max)) { // CASES 1-4
			if ((current_cost3_min <= pp_cost3_max) && (pp_cost3_max <= current_cost3_max)) {
				// CASE 1: Don't split
				//     [   ]
				// [          ]
				return false;

			} else if (pp_cost3_max < next_cost3_min) {
				// CASE 2: Split if can't extend ppa_instance
				//     [    ]
				// [      ]    [  ]
				return !(*instance)->can_extend(pp_cost3_min, pp_cost3_max, this->eps3);

			} else { // (next_cost3_min <= pp_cost3_max)
				// CASE 3+4: Split
				//      [    ]        //    [           ]
				// [      ][    ]     // [    ][]...[][    ]
				return true;
			}

		} else if ((current_cost3_max <= pp_cost3_min) && (pp_cost3_min <= next_cost3_min)) { // CASES 5-7
			if (pp_cost3_max < next_cost3_min) {
				// CASE 5: Don't split
				//        [    ]
				// [    ]       [    ]
				return false;

			} else if ((next_cost3_min <= pp_cost3_max) && (pp_cost3_max <= next_cost3_max)) {
				// CASE 6: Split if can't extend next_ppa_instance
				//        [     ]
				// [    ]     [    ]
				return !(*next_instance)->can_extend(pp_cost3_min, pp_cost3_max, this->eps3);

			} else { // (next_cost3_max < pp_cost3_max)
				// CASE 7: Split
				//        [         ]
				// [    ]   []..[][    ]
				return true;
			}
		}
	}

	// CASE 8: Don't split
	//        [    ]
	//
	return false;
}

PPAInstancePtr PPAInstanceSet::get_matching_instance(PathPairPtr pp) {
	// Should be called only on pp that should not be split
	CostType pp_top_left_cost3 = pp->get_top_left()->get_cost_until_now()[2];
	for (auto instance = this->ppa_instances.begin(); instance != this->ppa_instances.end(); ++instance) {
		if (((*instance)->cost3_min <= pp_top_left_cost3) && (pp_top_left_cost3 <= (*instance)->cost3_max)) {
			return *instance;
		}
	}

	CostType pp_bottom_right_cost3 	= pp->get_bottom_right()->get_cost_until_now()[2];
	PPAInstancePtr new_ppa_instance = std::make_shared<PPAInstance>(
		std::min(pp_top_left_cost3, pp_bottom_right_cost3),
		std::max(pp_top_left_cost3, pp_bottom_right_cost3),
		PPQueue(this->number_of_vertices),
		std::vector<CostType>(this->number_of_vertices+1, MAX_COST),
		PathPair::SolutionsSet());

	this->ppa_instances.insert(new_ppa_instance);
	return new_ppa_instance;
}

PathPair::SolutionsSet PPAInstanceSet::get_solutions() {
	PathPair::SolutionsSet solutions;
	for (auto instance = this->ppa_instances.begin(); instance != this->ppa_instances.end(); ++instance) {
		for (auto solution = (*instance)->solutions.begin(); solution != (*instance)->solutions.end(); ++solution) {
			solutions.push_back(*solution);
		}
	}
	return solutions;
}

