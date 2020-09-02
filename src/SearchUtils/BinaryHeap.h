#if 0

#ifndef SEARCH_UTILS_BINARY_HEAP_H
#define SEARCH_UTILS_BINARY_HEAP_H

#include "../Utils/Definitions.h"
#include "SearchNode.h"

// template <class SN_Ptr>
using SN_Ptr = SearchNodePtr;
class BinaryHeap {
private:
    size_t size;
    std::vector<SN_Ptr> elements;

    void percolate_up(size_t hole, SN_Ptr tmp) {
        if (this->size != 0) {
            for (; (hole > 1) && (is_less_than(tmp, this->elements[hole/2])); hole /= 2) {
                this->elements[hole] = this->elements[hole/2];
                this->elements[hole]->heap_index = hole;
            }
            this->elements[hole] = tmp;
            this->elements[hole]->heap_index = hole;
        }
    }

    void percolate_down(size_t hole, SN_Ptr tmp) {
        size_t child;
        if (this->size != 0) {
            for (; 2*hole <= this->size; hole = child) {
	            child = 2*hole;

	            if ((child != this->size) && (is_less_than(this->elements[child+1], this->elements[child]))) {
	                child += 1;
	            }

	            if (is_less_than(this->elements[child], tmp)) {
	                this->elements[hole] = this->elements[child];
	                this->elements[hole]->heap_index = hole;
	            } else {
	                break;
	            }
        	}
            this->elements[hole] = tmp;
            this->elements[hole]->heap_index = hole;
        }
    }

    void percolate_up_down(size_t hole, SN_Ptr tmp) {
        if (this->size != 0) {
            if ((hole > 1) && (is_less_than(tmp, this->elements[hole/2]))) {
                percolate_up(hole, tmp);
            } else {
                percolate_down(hole, tmp);
            }
        }
    }

    bool is_less_than(SN_Ptr a, SN_Ptr b) {
            #define BASE 1000000
            Triplet<CostType> a_cost = a->get_full_cost();
            Triplet<CostType> b_cost = b->get_full_cost();
            CostType a_key = a_cost[0] * BASE + a_cost[1];
            CostType b_key = b_cost[0] * BASE + b_cost[1];
        //     newk1 =  newg1 + h1;
        //     newk2 =  newg2 + h2;


        //     newkey = newk1 * BASE + newk2;

        // return SearchNode::less_than_full_costs()(a,b);
    	return (a_key < b_key);
    }

public:
	BinaryHeap(size_t max_elements=10000000) : size(0), elements(max_elements, nullptr) {}

    void delete_heap(SN_Ptr this_cell) {
        if (this_cell->heap_index != 0) {
            percolate_up_down(this_cell->heap_index, this->elements[this->size--]);
            this_cell->heap_index = 0;
        }
    }

    SN_Ptr top() {
        if (this->size == 0)
            return NULL;
        return this->elements[1];
    }

    SN_Ptr pop() {
        SN_Ptr this_cell;
        if (this->size == 0)
            return NULL;
        this_cell = this->elements[1];
        this_cell->heap_index = 0;
        percolate_down(1, this->elements[this->size]);
        this->size -= 1;
        return this_cell;
    }

    size_t heap_size() {
        return this->size;
    }

    bool is_empty() {
        return (this->size == 0);
    }

    SN_Ptr get_by_pos(size_t i) {
        return this->elements[i];
    }

    void insert(SN_Ptr this_cell) {
        if (this_cell->heap_index == 0) {
        	this->size += 1;
            percolate_up(this->size, this_cell);
        } else {
            percolate_up_down(this_cell->heap_index, this->elements[this_cell->heap_index]);
        }
    }

};

#endif //UTILS_BINARY_HEAP_H

