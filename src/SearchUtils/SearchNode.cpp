#include <algorithm>
#include <memory>
#include "SearchNode.h"

SearchNode::SearchNode(const Idx vertex_id,
                       const SearchNodePtr &parent,
                       Pair<CostType> cost_until_now,
                       Triplet<CostType> future_heuristic_cost)
                        : vertex_id(vertex_id),
                          parent(parent),
                          cost_until_now({cost_until_now[0], cost_until_now[1], 0}),
                          future_heuristic_cost(future_heuristic_cost) {}

SearchNode::SearchNode(const Idx vertex_id,
                       const SearchNodePtr &parent,
                       Triplet<CostType> cost_until_now,
                       Triplet<CostType> future_heuristic_cost)
                        : vertex_id(vertex_id),
                          parent(parent),
                          cost_until_now(cost_until_now),
                          future_heuristic_cost(future_heuristic_cost) {}

SearchNode::SearchNode(const SearchNodePtr &other) {
	this->deep_copy(other);
}

void SearchNode::deep_copy(const SearchNodePtr &other) {
    this->vertex_id = other->get_vertex_id();
    this->parent = other->get_parent();
    this->cost_until_now = other->get_cost_until_now();
    this->future_heuristic_cost = other->get_future_heuristic_cost();
}

Idx SearchNode::get_vertex_id() const {
	return this->vertex_id;
}

SearchNodePtr SearchNode::get_parent() const {
    return this->parent;
}

Triplet<CostType> SearchNode::get_cost_until_now() const {
	return this->cost_until_now;
}

Triplet<CostType> SearchNode::get_future_heuristic_cost() const {
	return this->future_heuristic_cost;
}

Triplet<CostType> SearchNode::get_full_cost() const {
	return {this->cost_until_now[0] + this->future_heuristic_cost[0],
            this->cost_until_now[1] + this->future_heuristic_cost[1],
            this->cost_until_now[2] + this->future_heuristic_cost[2]};
}

bool SearchNode::is_dominated_by(const SearchNodePtr &other, Triplet<double> eps, bool use_heuristic) const {
    Triplet<CostType> this_cost = use_heuristic ? this->get_full_cost() : this->get_cost_until_now();
    Triplet<CostType> other_cost = use_heuristic ? other->get_full_cost() : other->get_cost_until_now();

    return ((other_cost[0] <= ((1 + eps[0]) * this_cost[0])) &&
            (other_cost[1] <= ((1 + eps[1]) * this_cost[1])) &&
            (other_cost[2] <= ((1 + eps[2]) * this_cost[2])));
}

SearchNodePtr SearchNode::extend(const SearchNodePtr &node, const Edge edge, Triplet<CostType> future_heuristic_cost) {
    if ((node->get_vertex_id() != edge.source) ||
        ((node->get_parent() != nullptr) && (node->get_parent()->get_vertex_id() == edge.target))) {
        return nullptr;
    }

    Triplet<CostType> new_cost_until_now = node->get_cost_until_now();

    new_cost_until_now[0] += edge.cost[0];
    new_cost_until_now[1] += edge.cost[1];
    new_cost_until_now[2] += edge.cost[2];

    return std::make_shared<SearchNode>(edge.target, node, new_cost_until_now, future_heuristic_cost);
}

bool SearchNode::less_than_full_costs::operator()(const SearchNodePtr &a, const SearchNodePtr &b) const {
    Triplet<CostType> a_cost = a->get_full_cost();
    Triplet<CostType> b_cost = b->get_full_cost();

    if (a_cost[0] != b_cost[0]) {
        return a_cost[0] < b_cost[0];
    } else if (a_cost[1] != b_cost[1]) {
        return a_cost[1] < b_cost[1];
    } else {
        return a_cost[2] < b_cost[2];
    }
}

bool SearchNode::less_than_specific_cost_until_now::operator()(const SearchNodePtr &a, const SearchNodePtr &b) const {
    CostType a_cost = a->get_cost_until_now()[cost_idx];
    CostType b_cost = b->get_cost_until_now()[cost_idx];

    if (a_cost != b_cost) {
        return a_cost < b_cost;
    } else {
        return a->get_vertex_id() < b->get_vertex_id();
    }
}

std::ostream& operator<<(std::ostream &stream, const SearchNode &node) {
    std::string parent_id = node.get_parent() == nullptr ? "-1" : std::to_string(node.get_parent()->get_vertex_id());
    stream
        << "{"
        <<      "\"vertex_id\": " << node.get_vertex_id() << ", "
        <<      "\"parent\": " << parent_id << ", "
        <<      "\"cost_until_now\": " << node.get_cost_until_now() << ", "
        <<      "\"full_cost\": " << node.get_full_cost()
        << "}";
    return stream;
}
