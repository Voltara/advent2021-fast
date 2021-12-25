#include "advent2021.h"

namespace {

struct pt {
	std::array<int, 3> C;

	pt() : C() {
	}

	pt(int x, int y, int z) {
		C = { x, y, z };
	}

	pt min(const pt &o) const {
		return pt{
			std::min(C[0], o.C[0]),
			std::min(C[1], o.C[1]),
			std::min(C[2], o.C[2]),
		};
	}

	bool operator < (const pt &o) const {
		return C < o.C;
	}

	uint64_t hash() const {
		uint64_t hash = 0;
		for (auto n : C) {
			hash = (hash << 21) ^ n;
		}
		return hash;
	}
};

struct Scanner {
	std::vector<pt> P;
	pt offset;
	pt min;

	void add(const pt &p) {
		min = min.min(p);
		P.push_back(p);
	}
};

}

output_t day19(input_t in) {
	std::vector<Scanner> scanners = { };

	while (in.len) {
		scanners.emplace_back();
		auto &current_scanner = scanners.back();
		parse::skip(in, 18);
		do {
			int x = parse::integer<int>(in);
			int y = parse::integer<int>(in);
			int z = parse::integer<int>(in);
			current_scanner.add(pt{x, y, z});
			parse::skip(in, 1);
		} while (in.len && *in.s != '\n');
		if (!in.len) break;
		parse::skip(in, 1);
	}

	// TODO cleanup
	auto align = [](const Scanner &a, Scanner &b, int aa) {
		std::vector<uint8_t> collision(4096 * 6);
		for (auto pa : a.P) {
			for (auto pb : b.P) {
				int base = 0;
				for (int n : {
					2048 + (pb.C[0] - b.min.C[0]) - (pa.C[aa] - a.min.C[aa]), (pb.C[0] - b.min.C[0]) + (pa.C[aa] - a.min.C[aa]),
					2048 + (pb.C[1] - b.min.C[1]) - (pa.C[aa] - a.min.C[aa]), (pb.C[1] - b.min.C[1]) + (pa.C[aa] - a.min.C[aa]),
					2048 + (pb.C[2] - b.min.C[2]) - (pa.C[aa] - a.min.C[aa]), (pb.C[2] - b.min.C[2]) + (pa.C[aa] - a.min.C[aa]) })
				{
					int idx = base + n;
					if (++collision[idx] == 12) {
						int ori = idx / 4096;
						int axis = ori / 2;
						int negate = ori % 2;

						n += b.min.C[axis];
						if (negate) {
							n += a.min.C[aa];
						} else {
							n -= a.min.C[aa] + 2048;
						}

						b.offset.C[aa] = negate ? -n : n;

						if (axis != aa) {
							std::swap(b.min.C[aa], b.min.C[axis]);
							for (auto &p : b.P) {
								std::swap(p.C[aa], p.C[axis]);
							}
						}
						if (negate) {
							b.min.C[aa] = n - b.min.C[aa] - 2047;
							for (auto &p : b.P) {
								p.C[aa] = n - p.C[aa];
							}
						} else {
							b.min.C[aa] = b.min.C[aa] - n;
							for (auto &p : b.P) {
								p.C[aa] = p.C[aa] - n;
							}
						}
						return true;
					}
					base += 4096;
				}
			}
		}
		return false;
	};

	uint64_t need = (1LL << scanners.size()) - 2;
	std::vector<int> todo = { 0 };
	while (!todo.empty()) {
		int i = todo.back();
		todo.pop_back();
		for (auto j : bits(need)) {
			if (align(scanners[i], scanners[j], 0)) {
				align(scanners[i], scanners[j], 1);
				align(scanners[i], scanners[j], 2);
				need ^= 1LL << j;
				todo.push_back(j);
			}
		}
	}

	std::unordered_set<uint64_t> P1;
	for (auto &s : scanners) {
		for (auto p : s.P) {
			P1.insert(p.hash());
		}
	}
	int part1 = P1.size();

	int part2 = 0;
	for (auto &a : scanners) {
		for (auto &b : scanners) {
			int dist = 0;
			dist += std::abs(a.offset.C[0] - b.offset.C[0]);
			dist += std::abs(a.offset.C[1] - b.offset.C[1]);
			dist += std::abs(a.offset.C[2] - b.offset.C[2]);
			part2 = std::max(part2, dist);
		}
	}

	return { part1, part2 };
}
