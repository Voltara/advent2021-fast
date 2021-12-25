#include "advent2021.h"

output_t day04(input_t in) {
	constexpr int INPUT_LEN = 7890;
	constexpr int NUM_BOARDS = 100;
	constexpr int NUM_NUMBERS = 100;
	constexpr int BOARDS_LEN = 76 * NUM_BOARDS;

	if (in.len != INPUT_LEN) abort();

	struct Entry {
		int8_t b, pos;
		int16_t next;
	};
	std::vector<Entry> lookup(NUM_NUMBERS);
	lookup.reserve(NUM_NUMBERS + NUM_BOARDS * 25);

	std::vector<int> board_sum(NUM_BOARDS);

	// Build an map from each number to the boards/positions where they appear
	char *p = in.s + (INPUT_LEN - BOARDS_LEN);
	char *end_numbers = p;

	for (int board = 0; board < NUM_BOARDS; board++) {
		p++;
		int sum = 0;
		for (int i = 0; i < 25; i++, p += 3) {
			int n = 10 * (p[0] & 0xf) + (p[1] & 0xf);
			if (n >= NUM_NUMBERS) abort();
			if (lookup[n].next) {
				lookup.push_back(lookup[n]);
				lookup[n].next = lookup.size() - 1;
			} else {
				lookup[n].next = -1;
			}
			lookup[n].b = board;
			lookup[n].pos = i;
			sum += n;
		}
		board_sum[board] = sum;
	}

	// Did 'pos' just finish a row or column?
	auto is_solved = [](int b, int pos) {
		int column = pos % 5;
		int unfinished_row = 0x00001f & (~b >> (pos - column));
		int unfinished_col = 0x108421 & (~b >> column);
		return !unfinished_row | !unfinished_col;
	};

	int part1 = 0, part2 = 0;

	std::vector<int> boards(NUM_BOARDS);
	p = in.s;
	for (int unsolved = NUM_BOARDS; unsolved && p < end_numbers; ) {
		int n = (*p++ - '0');
		if (*p != ',') n = 10 * n + (*p++ - '0');
		p++;
		if (n < 0 || n >= 100) abort();

		if (!lookup[n].next) continue;

		for (int idx = n; idx != -1; idx = lookup[idx].next) {
			int b = lookup[idx].b;
			int pos = lookup[idx].pos;

			if (!board_sum[b]) continue;

			boards[b] |= 1 << pos;
			board_sum[b] -= n;

			if (is_solved(boards[b], pos)) {
				unsolved--;
				int score = n * board_sum[b];
				board_sum[b] = 0;

				if (!part1) {
					part1 = score;
				}

				if (!unsolved) {
					part2 = score;
					break;
				}
			}
		}
	}

	return { part1, part2 };
}
