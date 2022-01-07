#include "advent2021.h"

output_t day06(input_t in) {
	constexpr int INITIAL = 9;

	std::array<uint64_t, 257> A;
	std::fill(A.begin(), A.begin() + INITIAL, 0);

	// Set up a recurrence for the lanternfish population
	for (; in.len > 0; parse::skip(in, 2)) {
		A[(*in.s + 1) & 7]++;
	}
	A[0] = A[2] + A[3] + A[4] + A[5] + A[6];
	for (int i = 1; i < INITIAL; i++) {
		A[i] += A[i - 1];
	}

	// Iterate out to day 256
	for (int i = INITIAL; i <= 256; i++) {
		A[i] = A[i - 7] + A[i - 9];
	}

	uint64_t part1 = A[80], part2 = A[256];

	return { part1, part2 };
}
