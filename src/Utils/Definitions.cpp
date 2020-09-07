#include <iostream>
#include <map>
#include <set>
#include <vector>
#include "Definitions.h"

AdjacencyMatrix::AdjacencyMatrix(size_t graph_size, std::vector<Edge> &edges, bool inverse)
    : matrix((graph_size+1), std::vector<Edge>()), graph_size(graph_size) {

    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        if (inverse) {
            this->add(iter->inverse());
        } else {
            this->add((*iter));
        }
    }
}


void AdjacencyMatrix::add(Edge edge) {
    (this->matrix[edge.source]).push_back(edge);
}

size_t AdjacencyMatrix::size() const {return this->graph_size;}


const std::vector<Edge>& AdjacencyMatrix::operator[](size_t vertex_id) const {
    return this->matrix.at(vertex_id);
}


std::ostream& operator<<(std::ostream &stream, const AdjacencyMatrix &adj_matrix) {
    size_t  i = 0;

    stream << "{\n";
    for (auto vertex_iter = adj_matrix.matrix.begin(); vertex_iter != adj_matrix.matrix.end(); ++vertex_iter) {
        stream << "\t\"" << i++ << "\": [";

        std::vector<Edge> edges = *vertex_iter;
        for (auto edge_iter = edges.begin(); edge_iter != edges.end(); ++edge_iter) {
            stream << "\"" << edge_iter->source << "->" << edge_iter->target << "\", ";
        }

        stream << "],\n";
    }
    stream << "}";
    return stream;
}


std::ostream& operator<<(std::ostream &stream, const Edge &edge) {
    stream
        << "{"
        <<  "\"edge_source\": " << edge.source << ", "
        <<  "\"edge_target\": " << edge.target << ", "
        <<  "\"edge_cost\": " << edge.cost
        << "}";

    return stream;
}


Triplet<size_t> MapNode::f(void) const {
    return Triplet<size_t>({this->g[0]+this->h[0], this->g[1]+this->h[1], this->g[2]+this->h[2]});
}


bool MapNode::more_than_specific_heurisitic_cost::operator()(const MapNodePtr &a, const MapNodePtr &b) const {
    return (a->h[cost_idx] > b->h[cost_idx]);
}


bool MapNode::more_than_full_cost::operator()(const MapNodePtr &a, const MapNodePtr &b) const {
    if (a->f()[0] != b->f()[0]) {
        return (a->f()[0] > b->f()[0]);
    } else if (a->f()[1] != b->f()[1]) {
        return (a->f()[1] > b->f()[1]);
    } else {
        return (a->f()[2] > b->f()[2]);
    }
}


std::ostream& operator<<(std::ostream &stream, const MapNode &node) {
    std::string parent_id = node.parent == nullptr ? "-1" : std::to_string(node.parent->id);
    stream
        << "{"
        <<      "\"id\": " << node.id << ", "
        <<      "\"parent\": " << parent_id << ", "
        <<      "\"cost_until_now\": " << node.g << ", "
        <<      "\"heuristic_cost\": " << node.h << ", "
        <<      "\"full_cost\": " << node.f()
        << "}";
    return stream;
}


bool PathPair::update_nodes_by_merge_if_bounded(const PathPairPtr &other, const Pair<double> eps, bool use_heuristic) {
    if (this->id != other->id) {
        return false;
    }

    Triplet<size_t> this_top_left_cost = use_heuristic ? this->top_left->f() : this->top_left->g;
    Triplet<size_t> other_top_left_cost = use_heuristic ? other->top_left->f() : other->top_left->g;
    Triplet<size_t> this_bottom_right_cost = use_heuristic ? this->bottom_right->f() : this->bottom_right->g;
    Triplet<size_t> other_bottom_right_cost = use_heuristic ? other->bottom_right->f() : other->bottom_right->g;

    MapNodePtr new_top_left;
    MapNodePtr new_bottom_right;

    // Pick node with min cost1 (min cost2 if equal)
    if ((this_top_left_cost[0] < other_top_left_cost[0]) ||
        ((this_top_left_cost[0] == other_top_left_cost[0]) && (this_top_left_cost[1] < other_top_left_cost[1]))) {
        new_top_left = this->top_left;
    } else {
        new_top_left = other->top_left;
    }

    // Pick node with min cost2 (min cost1 if equal)
    if ((this_bottom_right_cost[1] < other_bottom_right_cost[1]) ||
        ((this_bottom_right_cost[1] == other_bottom_right_cost[1]) && (this_bottom_right_cost[0] < other_bottom_right_cost[0]))) {
        new_bottom_right = this->bottom_right;
    } else {
        new_bottom_right = other->bottom_right;
    }

    // If path pair wasn't changed, no need for bound check
    if ((new_top_left == this->top_left) && (new_bottom_right == this->bottom_right)) {
        return true;
    }

    // Check if path pair is bounded after merge - if not the merge is illegal
    if ((((1+eps[0])*new_top_left->g[0]) < new_bottom_right->g[0]) ||
        (((1+eps[1])*new_bottom_right->g[1]) < new_top_left->g[1])) {
        return false;
    }

    this->top_left = new_top_left;
    this->bottom_right = new_bottom_right;
    return true;
}


bool PathPair::more_than_full_cost::operator()(const PathPairPtr &a, const PathPairPtr &b) const {
    size_t f1_a = a->top_left->g[0] + a->top_left->h[0];
    size_t f1_b = b->top_left->g[0] + b->top_left->h[0];
    size_t f2_a = a->bottom_right->g[1] + a->bottom_right->h[1];
    size_t f2_b = b->bottom_right->g[1] + b->bottom_right->h[1];

    if (f1_a != f1_b) {
        return (f1_a > f1_b);
    } else {
        return (f2_a > f2_b);
    }
}

std::ostream& operator<<(std::ostream &stream, const PathPair &pp) {
    stream << "{" << pp.top_left << ", " << pp.bottom_right << "}";
    return stream;
}
