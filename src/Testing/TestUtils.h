#ifndef TESTING_TEST_UTILS_H
#define TESTING_TEST_UTILS_H

#include "../Utils/Definitions.h"
#include "../SearchNodes/SearchNode.h"

void validate_eps_domination(SearchNode::SolutionsSet& opt_solutions,
                             SearchNode::SolutionsSet& apx_solutions,
                             Triplet<EpsType> eps);

#endif