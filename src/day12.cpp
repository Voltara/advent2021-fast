#include "advent2021.h"

namespace {

constexpr uint64_t set1(uint64_t n) {
	return uint64_t(-1) / 15 * n;
}

}

output_t day12(input_t in) {
	std::unordered_map<uint32_t, int> name;

	constexpr int END = 8, START = 9;

	name[0x656E64] = END;
	name[0x74617274] = START;

	std::array<uint64_t, START + 1> edge{};
	std::vector<uint64_t> big;

	uint32_t word = 0;
	int n_small = 0;
	int id0 = 0;
	bool is_big0 = false;
	for (auto end = in.s + in.len; in.s < end; in.s++) {
		if (*in.s >= 'A') {
			word = (word << 8) | *in.s;
		} else {
			bool is_big = !(word & 0x20);
			int next_id = is_big ? 0x80 + big.size() : n_small;

			auto [ it, ok ] = name.emplace(word, next_id);
			int id = it->second;

			if (ok) {
				if (is_big) {
					big.push_back(0);
				} else {
					n_small++;
				}
			}

			if (*in.s == '-') {
				id0 = id;
				is_big0 = is_big;
			} else if (is_big0) {
				big[id0 ^ 0x80] |= 1LL << (4 * id);
			} else if (is_big) {
				big[id  ^ 0x80] |= 1LL << (4 * id0);
			} else {
				edge[id]  += 1LL << (4 * id0);
				edge[id0] += 1LL << (4 * id);
			}

			word = 0;
		}
	}

	for (auto b : big) {
		for (auto s : bits(b)) {
			edge[s / 4] += b;
		}
	}

	// remove edges leading to start
	for (auto &e : edge) {
		e &= 0xfffffffff;
	}

	int part1 = 0, part2 = 0;

	// TODO cleanup
	{
		// Part 1+2 prototype
		// key: (visited << 3) | cur
		// visited & (1 << n_small) if any small cave twice visited
		// note: visited excludes start,end
		std::vector<int> P2(4096);

		// Expand the start node
		for (auto b : bits((edge[START] + set1(7)) & set1(8))) {
			int i = b / 4;
			b &= -4;
			int weight = (edge[START] >> b) & 7;
			if (i == END) {
				part1 += weight;
			} else {
				P2[(8 << i) | i] += weight;
			}
		}
		std::vector<int> cur, next;
		for (int b = 1 << (n_small - 1); b > 0; b >>= 1) {
			next.push_back(b);
		}

		cur.swap(next);
		while (!cur.empty()) {
			for (auto n : cur) {
				for (int b = 1 << n_small; b > n; b >>= 1) {
					next.push_back(n | b);
				}
				int base = n << 3;
				for (auto s : bits(n & 0xff)) {
					int cur_weight = P2[base | s];
					if (!cur_weight) continue;
					for (auto b : bits((edge[s] + set1(7)) & set1(8))) {
						int i = b / 4;
						b &= -4;
						int weight = (edge[s] >> b) & 7;
						if (i == END) {
							if (~n & (1 << n_small)) {
								part1 += weight * cur_weight;
							} else {
								part2 += weight * cur_weight;
							}
						} else {
							int new_idx = base | (8 << i) | i;
							if (n & (1 << i)) {
								if (n & (1 << n_small)) continue;
								new_idx |= (8 << n_small);
							}
							P2[new_idx] += weight * cur_weight;
						}
					}
				}
			}
			cur.swap(next);
			next.clear();
		}
	}

	part2 += part1;

	return { part1, part2 };
}
