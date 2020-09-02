#ifndef SEARCH_UTILS_NODES_SEARCH_NODE_H
#define SEARCH_UTILS_NODES_SEARCH_NODE_H

#include <iostream>
#include <set>
#include "../Utils/Definitions.h"

class SearchNode;
using SearchNodePtr = std::shared_ptr<SearchNode>;

class SearchNode
{
private:
    Idx                 vertex_id;
    SearchNodePtr       parent = nullptr;
    Triplet<CostType>   cost_until_now; // cost of getting to this vertex
    Triplet<CostType>   future_heuristic_cost; // heuristic cost of getting to destination

    SearchNode(const SearchNodePtr &other);

    void deep_copy(const SearchNodePtr &other);

public:
    size_t heap_index = 0;

	SearchNode() = default;
    SearchNode(const Idx vertex_id, const SearchNodePtr &parent, Pair<CostType> cost_until_now, Triplet<CostType> future_heuristic_cost={0,0});
    SearchNode(const Idx vertex_id, const SearchNodePtr &parent, Triplet<CostType> cost_until_now, Triplet<CostType> future_heuristic_cost={0,0,0});

	Idx get_vertex_id() const;

    SearchNodePtr get_parent() const;

	Triplet<CostType> get_cost_until_now() const;

	Triplet<CostType> get_future_heuristic_cost() const;

	Triplet<CostType> get_full_cost() const;

    bool is_dominated_by(const SearchNodePtr &other, Triplet<double> eps, bool use_heuristic=false) const;

    static SearchNodePtr extend(const SearchNodePtr &node, const Edge edge, Triplet<CostType> future_heuristic_cost);

    struct less_than_full_costs {
        bool operator()(const SearchNodePtr &a, const SearchNodePtr &b) const;
    };

    struct less_than_specific_cost_until_now {
        size_t cost_idx;

        less_than_specific_cost_until_now(size_t cost_idx) : cost_idx(cost_idx) {};
        bool operator()(const SearchNodePtr &a, const SearchNodePtr &b) const;
    };

    using SolutionsSet = std::vector<SearchNodePtr>;
    friend std::ostream& operator<<(std::ostream &stream, const SearchNode &node);
};

#endif //SEARCH_UTILS_NODES_SEARCH_NODE_H


