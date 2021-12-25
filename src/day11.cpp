#include "advent2021.h"

namespace {

struct Dumbo {
	std::array<uint64_t, 10> D;

	Dumbo() { }

	Dumbo(uint64_t row) {
		std::fill(D.begin(), D.end(), row);
	}

	// Return row with all 10 positions set to n
	static constexpr uint64_t SET1(uint64_t n) {
		return (1LLU << 60) / 63 * n;
	}

	// Return true if all positions are zero
	bool empty() const {
		for (auto row : D) {
			if (row) return false;
		}
		return true;
	}

	// Count ones (expects only 0 or 1 in all positions)
	int count() const {
		int sum = 0;
		for (auto row : D) {
			sum += __builtin_popcountll(row);
		}
		return sum;
	}

	// Bitwise-or
	Dumbo operator | (const Dumbo &o) const {
		Dumbo N;
		for (int i = 0; i < D.size(); i++) {
			N.D[i] = D[i] | o.D[i];
		}
		return N;
	}

	// Add a constant to all positions
	Dumbo operator + (uint64_t n) const {
		Dumbo N;
		for (int i = 0; i < D.size(); i++) {
			N.D[i] = D[i] + SET1(n);
		}
		return N;
	}

	// Return array of ones in positions that are 10 or more
	Dumbo tens() const {
		Dumbo N;
		for (int i = 0; i < D.size(); i++) {
			N.D[i] = ((D[i] + SET1(22)) >> 5) & SET1(1);
		}
		return N;
	}

	// Spread out values 3-wide
	Dumbo spread() const {
		Dumbo N;
		for (int i = 0; i < D.size(); i++) {
			N.D[i] = (D[i] + (D[i] << 6) + (D[i] >> 6)) & SET1(63);
		}
		return N;
	}

	// Spread values in addend 3-tall and add to this
	Dumbo add_y3(const Dumbo &o) const {
		Dumbo N;
		N.D[0] = D[0] + o.D[0];
		for (int i = 1; i < D.size(); i++) {
			N.D[i - 1] += o.D[i];
			N.D[i] = D[i] + o.D[i] + o.D[i - 1];
		}
		return N;
	}

	// Zero out positions identified by ones in the mask
	Dumbo zero(const Dumbo &mask) const {
		Dumbo N;
		for (int i = 0; i < D.size(); i++) {
			N.D[i] = D[i] & ~(SET1(32) - mask.D[i]);
		}
		return N;
	}
};

};

output_t day11(input_t in) {
	int part1 = 0, part2 = 0;

	if (in.len != 110) abort();

	Dumbo dumbo;

	for (auto &r : dumbo.D) {
		r = 0;
		for (int j = 0; j < 10; j++) {
			r = (r << 6) | (in.s[j] - '0');
		}
		parse::skip(in, 11);
	}

	for (int i = 1; ; i++) {
		dumbo = dumbo + 1;

		auto tens = dumbo.tens();
		if (tens.empty()) continue;

		auto flashed = tens;
		for (;;) {
			dumbo = dumbo.add_y3(tens.spread()).zero(flashed);
			tens = dumbo.tens();
			if (tens.empty()) break;
			flashed = flashed | tens;
		}

		if (i <= 100) {
			part1 += flashed.count();
		}

		if (dumbo.empty()) {
			part2 = i;
			break;
		}
	}

	return { part1, part2 };
}
