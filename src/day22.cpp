#include "advent2021.h"

namespace {

constexpr int NUM_PART1 = 20, NUM_PART2 = 400;
constexpr int NUM_TOTAL = NUM_PART1 + NUM_PART2;


struct Interval {
	int lo, hi;

	Interval() : lo(), hi() {
	}

	Interval(input_t &in) {
		lo = parse::integer<int>(in);
		hi = parse::integer<int>(in) + 1;
	}

	Interval(int lo, int hi) : lo(lo), hi(hi) {
	}

	bool intersects(const Interval &o) const {
		return (lo < o.hi) && (o.lo < hi);
	}

	Interval intersect(const Interval &o) const {
		return Interval{std::max(lo, o.lo), std::min(hi, o.hi)};
	}

	auto length() const {
		return hi - lo;
	}
};

struct Cuboid {
	std::array<Interval, 3> axis;

	int64_t volume() const {
		int64_t vol = 1;
		for (auto &r : axis) {
			vol *= r.length();
		}
		return vol;
	}

	bool intersects(const Cuboid &o) const {
		for (int i = 0; i < axis.size(); i++) {
			if (!axis[i].intersects(o.axis[i])) {
				return false;
			}
		}
		return true;
	}

	Cuboid intersect(const Cuboid &o) const {
		Cuboid r;
		for (int i = 0; i < axis.size(); i++) {
			r.axis[i] = axis[i].intersect(o.axis[i]);
		}
		return r;
	}
};

struct bitset512 {
	using T = uint64_t;

	std::array<T, 8> B = { };

	void set(int n) {
		B[n >> 6] |= T(1) << (n & 63);
	}

	bitset512 operator & (const bitset512 &o) const {
		bitset512 r;
		for (int i = 0; i < B.size(); i++) {
			r.B[i] = B[i] & o.B[i];
		}
		return r;
	}

	struct iterator {
		const bitset512 &BS;
		int idx;
		bits<uint64_t> chunk;
		iterator(auto &BS, int idx, uint64_t chunk)
			: BS(BS), idx(idx), chunk(chunk)
		{
		}
		bool operator != (const iterator &o) const {
			return idx != o.idx || chunk != o.chunk;
		}
		int operator * () const {
			return *chunk + (idx * 64);
		}
		iterator& operator++ () {
			++chunk;
			while (!chunk.mask && ++idx < 7) {
				chunk = BS.B[idx];
			}
			return *this;
		}
	};
	auto begin() const {
		int idx = 0;
		while (idx < 7 && !B[idx]) idx++;
		return iterator{*this, idx, B[idx]};
	}
	auto end() const {
		return iterator{*this, 7, 0};
	}
};

struct Day22 {
	std::vector<Cuboid> cuboids;
	std::vector<bool> on;
	std::vector<bitset512> BS;

	void add(input_t &in) {
		Cuboid c;
		on.push_back(in.s[2] != 'f');
		for (int i = 0; i < 3; i++) {
			c.axis[i] = Interval{in};
		}
		cuboids.push_back(c);
	}

	void find_intersections() {
		/* Find all intersections with a previous cuboid in the list.
		 * The cuboids are convex and axis-aligned, so the 1D version
		 * of Helly's theorem applies.  Let A&B mean "A intersects B",
		 *   (A&B and A&C and B&C) => A&B&C
		 */
		BS.resize(cuboids.size());
		for (int i = 0; i < cuboids.size(); i++) {
			for (int j = 0; j < i; j++) {
				if (cuboids[i].intersects(cuboids[j])) {
					BS[j].set(i);
				}
			}
		}
	}

	int64_t intersect_volume(const Cuboid &cuboid, const auto &set) {
		int64_t vol = cuboid.volume();
		for (auto idx : set) {
			auto common = cuboid.intersect(cuboids[idx]);
			vol -= intersect_volume(common, set & BS[idx]);
		}
		return vol;
	}

	auto solve() {
		int64_t part1 = 0, part2 = 0;
		find_intersections();
		for (int i = 0; i < cuboids.size(); i++) {
			if (!on[i]) continue;
			part2 += intersect_volume(cuboids[i], BS[i]);
			if (i == NUM_PART1 - 1) part1 = part2;
		}
		return std::make_pair(part1, part2);
	}
};

}

output_t day22(input_t in) {
	Day22 day22;

	for (int i = 0; i < NUM_TOTAL; i++) {
		day22.add(in);
	}

	auto [ part1, part2 ] = day22.solve();

	return { part1, part2 };
}
