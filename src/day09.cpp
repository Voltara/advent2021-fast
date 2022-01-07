#include "advent2021.h"

/* Performs a line sweep over the map, identifying contiguous intervals
 * of basin (0-8) locations.  These intervals are joined with the previous
 * row using a disjoint set data structure which tracks the size and low
 * point of each basin.
 */

namespace {

// Open interval [lo, hi)
struct interval {
	interval *parent;
	uint8_t lo, hi;
	uint8_t min, size;

	interval() {
	}

	interval(uint8_t lo, uint8_t hi, uint8_t min) :
		lo(lo), hi(hi),
		min(min), size(hi - lo)
	{
	}

	interval * find() {
		interval *root = this;
		while (root != root->parent) {
			root = root->parent = root->parent->parent;
		}
		return root;
	}

	void join(interval *other) {
		auto a = find(), b = other->find();
		if (a == b) return;
		if (a->size < b->size) std::swap(a, b);
		b->parent = a;
		a->size += b->size;
		a->min = std::min(a->min, b->min);
	}
};

// Allocation pool
struct interval_pool {
	std::vector<interval> pool;
	int size;

	interval_pool(int size) : pool(size), size(size) {
	}

	auto alloc(uint8_t lo, uint8_t hi, uint8_t min) {
		auto ret = &pool[--size];
		*ret = interval{lo, hi, min};
		ret->parent = ret;
		return ret;
	}

	auto begin() {
		return pool.begin() + size;
	}

	auto end() {
		return pool.end();
	}
};

}

output_t day09(input_t in) {
	constexpr int DIM = 100;

	if (in.len != DIM * (DIM + 1)) abort();

	interval SENTINEL{UINT8_MAX, UINT8_MAX, 0};
	interval_pool pool(DIM * DIM + 1);

	std::vector<interval *> prev = { &SENTINEL }, curr;
	for (int r = 0; r < DIM; r++, in.s++) {
		// Convert this row into basin intervals
		uint8_t lo = 0, min = '9';
		for (int c = 0; c < DIM; c++, in.s++) {
			if (*in.s == '9') {
				if (lo != c) {
					curr.push_back(pool.alloc(lo, c, min));
				}
				lo = c + 1, min = '9';
			} else {
				min = std::min(min, uint8_t(*in.s));
			}
		}
		if (lo != DIM) {
			curr.emplace_back(pool.alloc(lo, DIM, min));
		}

		// Join this row's intervals with the last row
		auto p = prev.begin();
		for (auto r : curr) {
			while ((*p)->hi <= r->lo) p++;
			while ((*p)->hi <= r->hi) r->join(*p++);
			if ((*p)->lo < r->hi) r->join(*p);
		}

		curr.push_back(&SENTINEL);
		prev.swap(curr);
		curr.clear();
	}

	int part1 = 0, part2 = 0;

	// Find all disjoint-set roots and collect their
	// sizes and minimum values
	std::vector<int> sizes;
	sizes.reserve(std::distance(pool.begin(), pool.end()));
	for (auto &r : pool) {
		if (r.find() == &r) {
			part1 += r.min - '0' + 1;
			sizes.push_back(r.size);
		}
	}

	// Take the product of the top three sizes
	std::partial_sort(
			sizes.begin(), sizes.begin() + 3, sizes.end(),
			std::greater<int>{});
	part2 = sizes[0] * sizes[1] * sizes[2];

	return { part1, part2 };
}
