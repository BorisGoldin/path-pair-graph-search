#include "PathPair.h"

void PathPair::throw_if_invalid() const {
    if ((this->top_left->get_cost_until_now()[0] > this->bottom_right->get_cost_until_now()[0]) ||
        (this->top_left->get_cost_until_now()[1] < this->bottom_right->get_cost_until_now()[1]) ||
        (this->top_left->get_vertex_id() != this->bottom_right->get_vertex_id())) {
        throw std::runtime_error("Invalid path pair");
    }
}

PathPair::PathPair(const SearchNodePtr &top_left, const SearchNodePtr &bottom_right) : top_left(top_left), bottom_right(bottom_right) {
    this->throw_if_invalid();
}

Idx PathPair::get_vertex_id() const {
    return top_left->get_vertex_id();
}

SearchNodePtr PathPair::get_top_left() const {
    return this->top_left;
}

SearchNodePtr PathPair::get_bottom_right() const {
    return this->bottom_right;
}
void PathPair::update_nodes(const SearchNodePtr &top_left, const SearchNodePtr &bottom_right) {
    this->top_left = top_left;
    this->bottom_right = bottom_right;
    this->throw_if_invalid();
}

bool PathPair::update_nodes_by_merge_if_bounded(const PathPairPtr &other, const Pair<double> eps, bool use_heuristic) {
    if (this->get_vertex_id() != other->get_vertex_id()) {
        return false;
    }

    Triplet<CostType> this_top_left_cost =
        use_heuristic ? this->top_left->get_full_cost() : this->top_left->get_cost_until_now();
    Triplet<CostType> other_top_left_cost =
        use_heuristic ? other->top_left->get_full_cost() : other->top_left->get_cost_until_now();
    Triplet<CostType> this_bottom_right_cost =
        use_heuristic ? this->bottom_right->get_full_cost() : this->bottom_right->get_cost_until_now();
    Triplet<CostType> other_bottom_right_cost =
        use_heuristic ? other->bottom_right->get_full_cost() : other->bottom_right->get_cost_until_now();

    SearchNodePtr new_top_left;
    SearchNodePtr new_bottom_right;

    // Pick node with min cost1 (min cost2 if equal)
    if ((this_top_left_cost[0] < other_top_left_cost[0]) ||
        ((this_top_left_cost[0] == other_top_left_cost[0]) && (this_top_left_cost[1] < other_top_left_cost[1]))) {
        new_top_left = this->get_top_left();
    } else {
        new_top_left = other->get_top_left();
    }

    // Pick node with min cost2 (min cost1 if equal)
    if ((this_bottom_right_cost[1] < other_bottom_right_cost[1]) ||
        ((this_bottom_right_cost[1] == other_bottom_right_cost[1]) && (this_bottom_right_cost[0] < other_bottom_right_cost[0]))) {
        new_bottom_right = this->get_bottom_right();
    } else {
        new_bottom_right = other->get_bottom_right();
    }

    // If path pair wasn't changed, no need for bound check
    if ((new_top_left == this->get_bottom_right()) && (new_bottom_right == this->get_bottom_right())) {
        return true;
    }

    // Check if path pair is bounded after merge - if not the merge is illegal
    if (((new_top_left->get_cost_until_now()[0] * (1 + eps[0])) < new_bottom_right->get_cost_until_now()[0]) ||
        ((new_bottom_right->get_cost_until_now()[1] * (1 + eps[1])) < new_top_left->get_cost_until_now()[1])) {
        return false;
    }

    this->update_nodes(new_top_left, new_bottom_right);
    return true;
}

bool PathPair::is_active() const {
    return this->active_status;
}

bool PathPair::is_bounded(const Pair<double> eps) const {
    CostType cost1_min = this->top_left->get_cost_until_now()[0];
    CostType cost1_max = this->bottom_right->get_cost_until_now()[0];
    CostType cost2_min = this->bottom_right->get_cost_until_now()[1];
    CostType cost2_max = this->top_left->get_cost_until_now()[1];

    return (((cost1_min * (1 + eps[0])) >= cost1_max) &&
            ((cost2_min * (1 + eps[1])) >= cost2_max));
}

void PathPair::deactivate() {
    this->active_status = false;
}

PathPairPtr PathPair::extend(const PathPairPtr &pp, const Edge edge, Triplet<CostType> future_heuristic_cost) {
    if (pp->get_vertex_id() != edge.source) {
        return nullptr;
    }

    SearchNodePtr extended_top_left = SearchNode::extend(pp->get_top_left(), edge, future_heuristic_cost);
    SearchNodePtr extended_bottom_right;

    if (pp->get_top_left() == pp->get_bottom_right()) {
        extended_bottom_right = extended_top_left;
    } else {
        extended_bottom_right = SearchNode::extend(pp->get_bottom_right(), edge, future_heuristic_cost);
    }

    if ((extended_top_left == nullptr) || (extended_bottom_right == nullptr)) {
        return nullptr;
    }

    return std::make_shared<PathPair>(extended_top_left, extended_bottom_right);
}

PathPairPtr PathPair::merge(const PathPairPtr &a, const PathPairPtr &b, bool use_heuristic) {
    if (a->get_vertex_id() != b->get_vertex_id()) {
        return nullptr;
    }

    Triplet<CostType> a_top_left_cost = use_heuristic ? a->top_left->get_full_cost() : a->top_left->get_cost_until_now();
    Triplet<CostType> b_top_left_cost = use_heuristic ? b->top_left->get_full_cost() : b->top_left->get_cost_until_now();
    Triplet<CostType> a_bottom_right_cost = use_heuristic ? a->bottom_right->get_full_cost() : a->bottom_right->get_cost_until_now();
    Triplet<CostType> b_bottom_right_cost = use_heuristic ? b->bottom_right->get_full_cost() : b->bottom_right->get_cost_until_now();

    SearchNodePtr new_top_left;
    SearchNodePtr new_bottom_right;

    if ((a_top_left_cost[0] < b_top_left_cost[0]) ||
        ((a_top_left_cost[0] == b_top_left_cost[0]) && (a_top_left_cost[1] < b_top_left_cost[1]))) {
        new_top_left = a->get_top_left();
    } else {
        new_top_left = b->get_top_left();
    }

    if ((a_bottom_right_cost[1] < b_bottom_right_cost[1]) ||
        ((a_bottom_right_cost[1] == b_bottom_right_cost[1]) && (a_bottom_right_cost[0] < b_bottom_right_cost[0]))) {
        new_bottom_right = a->get_bottom_right();
    } else {
        new_bottom_right = b->get_bottom_right();
    }

    return std::make_shared<PathPair>(new_top_left, new_bottom_right);
}

bool PathPair::less_than_full_costs::operator()(const PathPairPtr &a, const PathPairPtr &b) const {
    Triplet<CostType> a_top_left_cost = a->get_top_left()->get_full_cost();
    Triplet<CostType> b_top_left_cost = b->get_top_left()->get_full_cost();
    Triplet<CostType> a_bottom_right_cost = a->get_bottom_right()->get_full_cost();
    Triplet<CostType> b_bottom_right_cost = b->get_bottom_right()->get_full_cost();

    if (a_top_left_cost[0] != b_top_left_cost[0]) {
        return a_top_left_cost[0] < b_top_left_cost[0];
    } else {
        return a_bottom_right_cost[1] < b_bottom_right_cost[1];
    }
}

SearchNode::SolutionsSet PathPair::solutions_from_path_pair_solutions(const PathPair::SolutionsSet &pp_solutions) {
    SearchNode::SolutionsSet solutions;
    for (auto iter = pp_solutions.begin(); iter != pp_solutions.end(); ++iter) {
        solutions.push_back((*iter)->get_top_left());
    }
    return solutions;
}

std::ostream& operator<<(std::ostream &stream, const PathPair &pp) {
    stream
        << "{"
            << "\"top_left\": " << pp.get_top_left() << ", "
            << "\"bottom_right\": " << pp.get_bottom_right()
        << "}";
    return stream;
}
