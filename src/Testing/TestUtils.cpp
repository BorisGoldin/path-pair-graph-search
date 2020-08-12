#include "TestUtils.h"

// TODO move out into python code that uses json log files
void validate_eps_domination(SearchNode::SolutionsSet& opt_solutions,
                             SearchNode::SolutionsSet& apx_solutions,
                             Triplet<EpsType> eps) {
    std::cout << "Starting Validation" << std::endl;
    for (auto iter1 = opt_solutions.begin(); iter1 != opt_solutions.end(); ++iter1) {
        SearchNodePtr opt_sol = *iter1;
        for (auto iter2 = apx_solutions.begin(); iter2 != apx_solutions.end(); ++iter2) {
            if ((*iter1)->is_dominated_by(*iter2, eps, false)) {
                continue;
            }
        }
        std::cout << "ERROR - solution not dominated: " << opt_sol << std::endl;
    }
    std::cout << "Finished Validation" << std::endl;
}