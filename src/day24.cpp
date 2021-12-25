#include "advent2021.h"

output_t day24(input_t in) {
	std::string part1("99999999999999");
	std::string part2("11111111111111");

	std::array<int8_t, 14> B, C;
	std::vector<int> S;
	for (int i = 0; i < 14; i++) {
		parse::skip(in, 37);
		int8_t a = parse::positive<int8_t>(in);
		parse::skip(in, 7);
		if (*in.s == '-') {
			in.s++;
			B[i] = -parse::positive<int8_t>(in);
		} else {
			B[i] = parse::positive<int8_t>(in);
		}
		parse::skip(in, 80);
		C[i] = parse::positive<int8_t>(in);
		parse::skip(in, 17);

		if (a == 1) {
			S.push_back(i);
		} else {
			int j = S.back();
			S.pop_back();
			int delta = B[i] + C[j];
			if (delta < 0) {
				part1[i] = '9' + delta;
				part2[j] = '1' - delta;
			} else {
				part1[j] = '9' - delta;
				part2[i] = '1' + delta;
			}
		}
	}

	return { part1, part2 };
}
