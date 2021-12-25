#include "advent2021.h"

output_t day14(input_t in) {
	auto map = [](char ch) {
		constexpr std::array<uint8_t, 15> M = {
			0, 0, 0, 1, 2, 3, 4, 5, 6, 0, 7, 8, 9, 0, 0
		};
		return M[ch % 15];
	};

	if (in.len != 822) abort();

	std::vector<uint64_t> count(100), next(100);
	std::array<std::array<uint8_t, 2>, 100> trans{};

	uint8_t first = map(*in.s++), last;
	for (uint8_t i = 1, prev = first; i < 20; i++) {
		last = map(*in.s++);
		count[prev * 10 + last]++;
		prev = last;
	}

	in.s += 2;
	for (uint8_t i = 0; i < 100; i++) {
		uint8_t s0 = map(in.s[0]);
		uint8_t s1 = map(in.s[1]);
		uint8_t d1 = map(in.s[6]);
		trans[s0 * 10 + s1][0] = s0 * 10 + d1;
		trans[s0 * 10 + s1][1] = d1 * 10 + s1;
		in.s += 8;
	}

	auto step = [&count, &next, trans]() {
		for (int i = 0; i < 100; i++) {
			next[trans[i][0]] += count[i];
			next[trans[i][1]] += count[i];
		}
		count.swap(next);
		std::fill(next.begin(), next.end(), 0);
	};

	auto score = [&count, first, last]() {
		std::array<uint64_t, 10> sum{};
		sum[first]++;
		sum[last]++;
		for (int i = 0, s0 = 0; s0 < 10; s0++) {
			for (int s1 = 0; s1 < 10; s1++, i++) {
				sum[s0] += count[i];
				sum[s1] += count[i];
			}
		}
		auto [ lo, hi ] = std::minmax_element(sum.begin(), sum.end());
		return (*hi - *lo) / 2;
	};

	for (int i = 0; i < 10; i++) step();
	auto part1 = score();
	for (int i = 10; i < 40; i++) step();
	auto part2 = score();

	return { part1, part2 };
}
