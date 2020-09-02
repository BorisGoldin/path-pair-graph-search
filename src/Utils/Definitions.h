#ifndef UTILS_DEFINITIONS_H
#define UTILS_DEFINITIONS_H

#include <map>
#include <vector>
#include <list>
#include <array>
#include <iostream>
#include <limits>

// Default settings
#ifndef DEBUG
#define DEBUG 2
#endif

// Types
using Idx       = size_t;
using CostType  = double;
using EpsType   = double;

template<typename T>
using Pair      = std::array<T, 2>;

template<typename T>
using Triplet   = std::array<T, 3>;

using Heuristic = std::function<Triplet<CostType>(Idx)>;

// Structs and classes
struct Edge
{
    Idx                 source;
    Idx                 target;
    Triplet<CostType>   cost;

    Edge inverse() {
        return Edge({.source=this->target, .target=this->source, .cost=this->cost});
    }
};
std::ostream& operator<<(std::ostream &stream, const Edge &edge);

class AdjacencyMatrix
{
private:
    std::vector<std::list<Edge>> matrix;
    size_t size;
public:
    AdjacencyMatrix() = default;
    AdjacencyMatrix(size_t graph_size, std::vector<Edge> &edges, bool inverse=false);
    void add(Edge edge);
    size_t get_number_of_vertices() const;
    std::list<Edge>& operator[](Idx vertex_id);
    const std::list<Edge>& operator[](Idx vertex_id) const;

    friend std::ostream& operator<<(std::ostream &stream, const AdjacencyMatrix &adj_matrix);
};

// Functions
template<typename T>
std::ostream& operator<<(std::ostream &stream, const Pair<T> pair) {
    stream << "[" << pair[0] << ", " << pair[1] << "]";
    return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream &stream, const Triplet<T> triplet) {
    stream << "[" << triplet[0] << ", " << triplet[1] << ", " << triplet[2] << "]";
    return stream;
}

// Globals
const size_t    AMOUNT_OF_COST_FUNCTIONS    = 3;
const CostType  MAX_COST                    = std::numeric_limits<CostType>::max();

#endif //UTILS_DEFINITIONS_H
