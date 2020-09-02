#include <iostream>
#include <map>
#include <set>
#include <vector>
#include "Definitions.h"

AdjacencyMatrix::AdjacencyMatrix(size_t graph_size, std::vector<Edge> &edges, bool inverse)
    : matrix((graph_size+1), std::list<Edge>()), size(graph_size) {

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
}

size_t AdjacencyMatrix::get_number_of_vertices() const {
    return this->size;
}

std::list<Edge>& AdjacencyMatrix::operator[](Idx vertex_id) {
    return this->matrix.at(vertex_id);
}

const std::list<Edge>& AdjacencyMatrix::operator[](Idx vertex_id) const {
    return this->matrix.at(vertex_id);
}

std::ostream& operator<<(std::ostream &stream, const AdjacencyMatrix &adj_matrix) {
    size_t  i = 0;

    stream << "{\n";
    for (auto vertex_iter = adj_matrix.matrix.begin(); vertex_iter != adj_matrix.matrix.end(); ++vertex_iter) {
        stream << "\t\"" << i++ << "\": [";

        std::list<Edge> edge_list = *vertex_iter;
        for (auto edge_iter = edge_list.begin(); edge_iter != edge_list.end(); ++edge_iter) {
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
