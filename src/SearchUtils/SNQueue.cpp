#include "SNQueue.h"

SNQueue::SNQueue(size_t number_of_vertices) 
    : queue(SNMinHeap(), SNOpenMap(number_of_vertices+1, std::list<SearchNodePtr>())) {}

bool SNQueue::is_empty() {
    return this->queue.first.empty();
}

SearchNodePtr SNQueue::pop() {
    // Pop from min heap
    SearchNodePtr node = *(this->queue.first.begin());
    this->queue.first.erase(this->queue.first.begin());

    // Remove from open map
    std::list<SearchNodePtr>& relevant_search_nodes = this->queue.second[node->get_vertex_id()];
    for (auto iter = relevant_search_nodes.begin(); iter != relevant_search_nodes.end(); ++iter) {
        if (node == *iter) {
            relevant_search_nodes.erase(iter);
            break;
        }
    }
    
    return node;
}

void SNQueue::insert(SearchNodePtr& node) {
    this->queue.first.insert(node);
    this->queue.second[node->get_vertex_id()].push_back(node);
}