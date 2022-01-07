#include "advent2021.h"

output_t day01(input_t in) {
	int part1 = 0, part2 = 0;

	// Use a 64-bit integer as a FIFO of 16-bit integers
	uint64_t previous = INT64_MAX;
	uint16_t n;
	while ((n = parse::positive<uint16_t>(in))) {
		part1 += n > uint16_t(previous);
		part2 += n > uint16_t(previous >> 32);
		previous = (previous << 16) + n;
	}

	return { part1, part2 };
}
