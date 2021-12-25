#include "advent2021.h"

namespace {

constexpr int points(int pos) {
	return pos + 1;
}

auto get_skip10(const auto &pos) {
	std::array<int, 2> skip = { };

	skip[0] += 4 * points((pos[0] + 0) % 10);
	skip[0] += 4 * points((pos[0] + 2) % 10);
	skip[0] += 2 * points((pos[0] + 6) % 10);

	skip[1] += 2 * points((pos[1] + 0) % 10);
	skip[1] += 2 * points((pos[1] + 3) % 10);
	skip[1] += 1 * points((pos[1] + 4) % 10);
	skip[1] += 2 * points((pos[1] + 5) % 10);
	skip[1] += 2 * points((pos[1] + 8) % 10);
	skip[1] += 1 * points((pos[1] + 9) % 10);

	return skip;
}

int day21_part1(const auto &start) {
	constexpr int FINAL_SCORE = 1000;

	// Get number of points earned by each player in 10 turns
	auto skip10 = get_skip10(start);

	// Find the winner and total number of turns played,
	// rounded down to the nearest multiple of 20
	int winner = skip10[0] < skip10[1];
	int turns = (FINAL_SCORE - 1) / skip10[winner];
	std::array<int, 2> score = {
		skip10[0] * turns,
		skip10[1] * turns
	};
	turns *= 20;

	// Play out the remaining turns
	int die = 6 + 9 * turns;
	auto pos = start;
	for (int i = 0; score[1 - i] < FINAL_SCORE; i = 1 - i) {
		pos[i] += die;
		score[i] += points(pos[i] % 10);
		die += 9;
		turns++;
	}

	return score[1 - winner] * turns * 3;
}

struct winlose_t {
	int64_t win = 0, lose = 0;
	winlose_t swap() const {
		return winlose_t{lose, win};
	}
	winlose_t operator * (int64_t m) const {
		return winlose_t{win * m, lose * m};
	}
	winlose_t & operator += (const winlose_t &o) {
		win += o.win;
		lose += o.lose;
		return *this;
	}
	int64_t max() const {
		return std::max(win, lose);
	}
};

int64_t day21_part2(const auto &start) {
	winlose_t DP[21][21][10][10] = { };

	for (int score_sum = 40; score_sum >= 0; score_sum--) {
		int s0 = std::min(score_sum, 20);
		for (int s1 = score_sum - s0; s0 >= 0 && s1 <= 20; s0--, s1++) {
			for (int p0 = 0; p0 < 10; p0++) {
				int next_p0 = (p0 < 7) ? p0 + 3 : p0 - 7;
				for (int p1 = 0; p1 < 10; p1++) {
					winlose_t WL;
					int pos = next_p0;
					for (int mul : { 1, 3, 6, 7, 6, 3, 1 }) {
						int score = s0 + points(pos);
						if (score < 21) {
							WL += DP[s1][score][p1][pos] * mul;
						} else {
							WL += winlose_t{0, 1} * mul;
						}
						pos = (pos < 9) ? pos + 1 : 0;
					}
					DP[s0][s1][p0][p1] = WL.swap();
				}
			}
		}
	}

	return DP[0][0][start[0]][start[1]].max();
}

}

output_t day21(input_t in) {
	auto points = [](int pos) {
		return pos + 1;
	};

	std::array<int, 2> start;
	for (auto &s : start) {
		parse::skip(in, 28);
		s = (parse::positive<int>(in) - 1) % 10;
		parse::skip(in, 1);
	}

	auto part1 = day21_part1(start);
	auto part2 = day21_part2(start);

	return { part1, part2 };
}
