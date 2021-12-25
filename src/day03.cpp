#include "advent2021.h"

/* Part 1 can also be done using 16 x 8-bit SIMD; it works
 * out nicely because the input characters can be loaded
 * directly into the register.  The "parse" is simply a
 * bitwise-and with 1.  Total runtime for a SIMD Part 1
 * implementation is about a microsecond.  However with
 * more and more ARM chips out there, we're going to
 * eschew the SIMD for portability.
 */

output_t day03(input_t in) {
	if (in.len != 13000) abort();

	std::array<uint64_t, 64> exists = { };
	std::array<uint16_t, 12> counts = { };

	for (char *p = in.s, *end = in.s + in.len; p != end; p += 13) {
		int value = 0;
		for (int i = 0; i < 12; i++) {
			counts[i] += (p[i] == '1');
			value |= (p[i] == '1') << (11 - i);
		}
		exists[value / 64] |= uint64_t(1) << (value % 64);
	}

	// Build a sorted list of values from the bit set
	std::array<uint16_t, 1000> values;
	for (int i = 0, idx = 0; i < 64; i++) {
		for (auto j : bits(exists[i])) {
			values[idx++] = 64 * i + j;
		}
	}

	// Binary search for partition point for bit position
	auto partition = [values](int begin, int end, uint16_t bit) {
		for (int size = end - begin; size; ) {
			int half = size / 2, mid = begin + half;
			if (~values[mid] & bit) {
				begin = mid + 1;
				size -= half + 1;
			} else {
				size = half;
			}
		}
		return begin;
	};

	// Lookup carbon or oxygen value, depending on argument
	auto lookup = [partition](int begin, int end, int mid, bool oxygen) {
		for (int bit = 0x400; begin + 1 != end; bit >>= 1) {
			if ((end - mid < mid - begin) ^ oxygen) {
				begin = mid;
			} else {
				end = mid;
			}
			mid = partition(begin, end, bit);
		}
		return begin;
	};

	// Part 1
	int gamma = 0;
	for (int i = 0; i < 12; i++) {
		gamma |= (counts[i] > 500) << (11 - i);
	}
	int epsilon = gamma ^ 0xfff;
	int part1 = gamma * epsilon;

	// Part 2
	int mid = partition(0, 1000, 0x800);
	int oxygen = values[lookup(0, 1000, mid, true )];
	int carbon = values[lookup(0, 1000, mid, false)];
	int part2 = oxygen * carbon;

	return { part1, part2 };
}
