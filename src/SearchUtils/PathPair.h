#ifndef SEARCH_UTILS_NODES_PATH_PAIR_H
#define SEARCH_UTILS_NODES_PATH_PAIR_H

#include <set>
#include "SearchNode.h"

class PathPair;
using PathPairPtr = std::shared_ptr<PathPair>;

class PathPair
{
private:
    SearchNodePtr top_left      = nullptr;
    SearchNodePtr bottom_right  = nullptr;
    bool active_status          = true;

    void throw_if_invalid() const;

public:
    PathPair() = default;
    PathPair(const SearchNodePtr &top_left, const SearchNodePtr &bottom_right);

    void update_nodes(const SearchNodePtr &top_left, const SearchNodePtr &bottom_right);
    bool update_nodes_by_merge_if_bounded(const PathPairPtr &other, const Pair<double> eps, bool use_heuristic=false);

    Idx get_vertex_id() const;
    SearchNodePtr get_top_left() const;
    SearchNodePtr get_bottom_right() const;

    bool is_active() const;
    bool is_bounded(const Pair<double> eps) const;

    void deactivate();

    static PathPairPtr extend(const PathPairPtr &node, const Edge edge, Triplet<CostType> future_heuristic_cost);
    static PathPairPtr merge(const PathPairPtr &a, const PathPairPtr &b, bool use_heuristic=false);

    struct less_than_full_costs {
        bool operator()(const PathPairPtr &a, const PathPairPtr &b) const;
    };

    using SolutionsSet = std::vector<PathPairPtr>;

    static SearchNode::SolutionsSet solutions_from_path_pair_solutions(const PathPair::SolutionsSet &pp_solutions);

    friend std::ostream& operator<<(std::ostream &stream, const PathPair &pp);
};

#endif //SEARCH_UTILS_NODES_PATH_PAIR_H
