#include "advent2021.h"

output_t day07(input_t in) {
	std::array<uint16_t, 2048> counts = { };
	std::array<uint32_t, 2048> sums = { };
	uint32_t squares = 0;

	for (; in.len > 0; parse::skip(in, 1)) {
		int n = parse::positive<uint16_t>(in) % 2048;
		counts[n]++;
		sums[n] += n;
		squares += n * n;
	}

	std::partial_sum(counts.begin(), counts.end(), counts.begin());
	std::partial_sum(sums.begin(), sums.end(), sums.begin());

	// The count is always 1000 anyway
	if (counts.back() % 2 != 0) abort();

	auto midpoint = std::lower_bound(
			counts.begin(), counts.end(), counts.back() / 2);
	int median = std::distance(counts.begin(), midpoint);
	int part1 = (sums.back() - 2*sums[median])
		+ median*(2*counts[median] - counts.back());

	auto try_part2 = [&](uint32_t mu) {
		int n = squares + sums.back() - 2*sums[mu] +
			mu*(2*(counts[mu] - sums.back())
					+ (mu-1)*counts.back());
		return n / 2;
	};

	/* Answer will round either up or down from the mean, but
	 * it will not necessarily round toward the closest value
	 */
	int mean = sums.back() / counts.back();
	int part2 = std::min(try_part2(mean), try_part2(mean + 1));

	return { part1, part2 };
}
