#include <iostream>
#include <map>
#include <set>
#include <vector>
#include "Definitions.h"

AdjacencyMatrix::AdjacencyMatrix(std::vector<Edge>& edges, bool inverse) {
    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        if (inverse) {
            this->add((*iter).inverse());
        } else {
            this->add((*iter));
        }
    }
}

void AdjacencyMatrix::add(Edge edge) {
    this->matrix[edge.source].push_back(edge);

    if (matrix.find(edge.target) == matrix.end()) {
        matrix[edge.target] = std::vector<Edge>();
    }
}

size_t AdjacencyMatrix::get_number_of_vertices() const {
    return this->matrix.size();
}

std::vector<Edge>& AdjacencyMatrix::operator[](Idx vertex_id) {
    return this->matrix.at(vertex_id);
}

const std::vector<Edge>& AdjacencyMatrix::operator[](Idx vertex_id) const {
    return this->matrix.at(vertex_id);
}

std::ostream& operator<<(std::ostream& stream, const AdjacencyMatrix& adj_matrix) {
    stream << "{\n";
    for (auto vertex_iter = adj_matrix.matrix.begin(); vertex_iter != adj_matrix.matrix.end(); ++vertex_iter) {
        stream << "\t\"" << vertex_iter->first << "\": [";

        std::vector<Edge> edge_vector = vertex_iter->second;
        for (auto edge_iter = edge_vector.begin(); edge_iter != edge_vector.end(); ++edge_iter) {
            stream << "\"" << edge_iter->source << "->" << edge_iter->target << "\", ";
        }

        stream << "],\n";
    } 
    stream << "}";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
    stream
        << "{"
        <<  "\"edge_source\": " << edge.source << ", "
        <<  "\"edge_target\": " << edge.target << ", "
        <<  "\"edge_cost\": " << edge.cost
        << "}";

    return stream;
}
