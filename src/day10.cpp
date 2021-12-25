#include "advent2021.h"

output_t day10(input_t in) {
	auto error_score = [](char c) {
		switch (c) {
		    case ')': return 3;
		    case ']': return 57;
		    case '}': return 1197;
		    case '>': return 25137;
		    default:  return 0;
		}
	};

	auto autocomplete_score = [](auto &stack) {
		int64_t score = 0;
		while (stack.size() > 1) {
			int c = stack.back();
			score = score * 5 + 1 + (0x10980 >> (c >> 3)) % 4;
			stack.pop_back();
		}
		return score;
	};

	std::vector<char> stack = { 0 };
	std::vector<int64_t> score;

	int64_t part1 = 0;
	for (auto p = in.s, end = p + in.len; p != end; p++) {
		if (*p == '\n') {
			score.push_back(autocomplete_score(stack));
		} else if ((0x19 >> (*p & 7)) & 1) {
			stack.push_back(*p);
		} else if (stack.back() + 2 == *p + (*p == ')')) {
			stack.pop_back();
		} else {
			part1 += error_score(*p);
			p = strchr(p, '\n');
			stack.resize(1);
		}
	}

	auto midpoint = score.size() / 2;
	std::nth_element(score.begin(), score.begin() + midpoint, score.end());
	int64_t part2 = score[midpoint];

	return { part1, part2 };
}
