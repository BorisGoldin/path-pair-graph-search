#ifndef UTILS_DEFINITIONS_H
#define UTILS_DEFINITIONS_H

#include <map>
#include <vector>
#include <array>
#include <list>
#include <iostream>
#include <limits>
#include <functional>

#define BORIS_LOG 1

#ifdef BORIS_LOG
#include <sys/time.h>
#endif //BORIS_LOG

// Default settings
#ifndef DEBUG
#define DEBUG 2
#endif

const size_t MAX_COST = std::numeric_limits<size_t>::max();

template<typename T>
using Pair      = std::array<T, 2>;

template<typename T>
std::ostream& operator<<(std::ostream &stream, const Pair<T> pair) {
    stream << "[" << pair[0] << ", " << pair[1] << "]";
    return stream;
}


template<typename T>
using Triplet   = std::array<T, 3>;

template<typename T>
std::ostream& operator<<(std::ostream &stream, const Triplet<T> triplet) {
    stream << "[" << triplet[0] << ", " << triplet[1] << ", " << triplet[2] << "]";
    return stream;
}


using Heuristic = std::function<Triplet<size_t>(size_t)>;


// Structs and classes
struct Edge {
    size_t                 source;
    size_t                 target;
    Triplet<size_t>   cost;

    Edge(size_t source, size_t target, Triplet<size_t> cost) : source(source), target(target), cost(cost) {}
    Edge inverse() {
        return Edge(this->target, this->source, this->cost);
    }
};
std::ostream& operator<<(std::ostream &stream, const Edge &edge);


class AdjacencyMatrix {
private:
    std::vector<std::vector<Edge>> matrix;
    size_t                         graph_size;

public:
    AdjacencyMatrix() = default;
    AdjacencyMatrix(size_t graph_size, std::vector<Edge> &edges, bool inverse=false);
    void add(Edge edge);
    size_t size(void) const;
    const std::vector<Edge>& operator[](size_t vertex_id) const;

    friend std::ostream& operator<<(std::ostream &stream, const AdjacencyMatrix &adj_matrix);
};


struct MapNode;
struct PathPair;
using MapNodePtr = std::shared_ptr<MapNode>;
using PathPairPtr = std::shared_ptr<PathPair>;
using SolutionSet = std::vector<MapNodePtr>;
using PPSolutionSet = std::vector<PathPairPtr>;

struct MapNode {
    size_t          id;
    Triplet<size_t> g;
    Triplet<size_t> h;
    MapNodePtr      parent;

    MapNode(size_t id, Triplet<size_t> g, Triplet<size_t> h, MapNodePtr parent=nullptr)
        : id(id), g(g), h(h), parent(parent) {};

    Triplet<size_t> f(void) const ;

    struct more_than_specific_heurisitic_cost {
        size_t cost_idx;

        more_than_specific_heurisitic_cost(size_t cost_idx) : cost_idx(cost_idx) {};
        bool operator()(const MapNodePtr &a, const MapNodePtr &b) const;
    };

    struct more_than_full_cost {
        bool operator()(const MapNodePtr &a, const MapNodePtr &b) const;
    };

    friend std::ostream& operator<<(std::ostream &stream, const MapNode &node);
};

struct PathPair {
    size_t         id;
    MapNodePtr     top_left;
    MapNodePtr     bottom_right;
    MapNodePtr     parent;
    bool           is_active=true;

    PathPair(const MapNodePtr &top_left, const MapNodePtr &bottom_right)
        : id(top_left->id), top_left(top_left), bottom_right(bottom_right), parent(top_left->parent) {};

    bool update_nodes_by_merge_if_bounded(const PathPairPtr &other, const Pair<double> eps, bool use_heuristic);

    struct more_than_full_cost {
        bool operator()(const PathPairPtr &a, const PathPairPtr &b) const;
    };

    friend std::ostream& operator<<(std::ostream &stream, const PathPair &pp);
};

#endif //UTILS_DEFINITIONS_H
