#include "PPQueue.h"

PPQueue::PPQueue(size_t number_of_vertices)
    : queue(PPMinHeap(), PPOpenMap(number_of_vertices+1, std::list<PathPairPtr>())) {}

bool PPQueue::is_empty() {
    return this->queue.first.empty();
}

PathPairPtr PPQueue::peek() {
    return *(this->queue.first.begin());
}

PathPairPtr PPQueue::pop() {
    // Pop from min heap
    PathPairPtr pp = *(this->queue.first.begin());
    this->queue.first.erase(this->queue.first.begin());

    // Remove from open map
    std::list<PathPairPtr>& relevant_pps = this->queue.second[pp->get_vertex_id()];
    for (auto iter = relevant_pps.begin(); iter != relevant_pps.end(); ++iter) {
        if (pp == *iter) {
            relevant_pps.erase(iter);
            break;
        }
    }

    return pp;
}

void PPQueue::insert(PathPairPtr &pp) {
    this->queue.first.insert(pp);
    this->queue.second[pp->get_vertex_id()].push_back(pp);
}

std::list<PathPairPtr>& PPQueue::get_open_pps(Idx vertex_id) {
	return this->queue.second[vertex_id];
}
