#include "advent2021.h"

output_t day15(input_t in) {
	constexpr int DIM = 512, INPUT_DIM = 100, INPUT_BIG = 500;

	if (in.len != (INPUT_DIM + 1) * INPUT_DIM) abort();

	std::vector<int8_t> lookup(DIM * DIM);

	for (int r = 1; r <= INPUT_DIM; r++) {
		auto row = &lookup[DIM*r + 1];
		for (int c = 0; c < INPUT_DIM; c++) {
			row[c] = in.s[c] - '0';
		}
		in.s += INPUT_DIM + 1;
		row += DIM;
	}

	auto search = [&lookup](int r, int c) {
		int goal = DIM * r + c;

		std::array<std::vector<int>, 17> Q{};
		Q[0].emplace_back(DIM*1 + 1);

		for (int qi = 0; ; qi++) {
			Q[16].clear();
			Q[qi % 16].swap(Q[16]);

			for (auto p : Q[16]) {
				if (lookup[p] < 1) continue;
				if (p == goal) return qi;
				lookup[p] |= 0x80;
				for (int delta : { -1, 1, -DIM, DIM }) {
					auto n = p + delta;
					if (lookup[n] < 1) continue;
					Q[(qi + lookup[n]) % 16].push_back(n);
				}
			}
		}
	};

	int part1 = search(INPUT_DIM, INPUT_DIM);

	for (int r = 1; r <= INPUT_DIM; r++) {
		for (int x = 1; x <= INPUT_DIM; x++) {
			lookup[DIM*r + x] &= 0x7f;
		}
	}

	for (int r = 1; r <= INPUT_DIM; r++) {
		auto cur = &lookup[DIM*r + INPUT_DIM + 1], prev = cur - INPUT_DIM;
		for (int c = INPUT_DIM + 1; c <= INPUT_BIG; c++) {
			auto risk = *prev++;
			*cur++ = (risk & -(risk < 9)) + 1;
		}
	}
	for (int r = INPUT_DIM + 1; r <= INPUT_BIG; r++) {
		auto cur = &lookup[DIM*r + 1], prev = cur - DIM * INPUT_DIM;
		for (int c = 1; c <= INPUT_BIG; c++) {
			auto risk = *prev++;
			*cur++ = (risk & -(risk < 9)) + 1;
		}
	}
	for (int r = 0; r < DIM; r++) {
		lookup[DIM*r + 0] = 0;
		lookup[DIM*r + INPUT_BIG + 1] = 0;
	}

	int part2 = search(INPUT_BIG, INPUT_BIG);

	return { part1, part2 };
}
