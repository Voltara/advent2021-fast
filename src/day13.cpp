#include "advent2021.h"

output_t day13(input_t in) {
	constexpr int FOLD_X1 = 655, MAX_Y = 895;
	constexpr int FOLD_X2 = 40, FOLD_Y2 = 6;

	auto fold = [](int w, int n) {
		int div = n / (w + 1), mod = n % (w + 1);
		return (div & 1) ? (w - 1) - mod : mod;
	};

	std::bitset<MAX_Y * FOLD_X1> P1;
	std::bitset<FOLD_X2 * FOLD_Y2> P2;
	while (*in.s > '\n') {
		auto x = parse::positive<int16_t>(in);
		auto y = parse::positive<int16_t>(in);
		parse::skip(in, 1);
		if (y > MAX_Y) abort();
		P1.set(fold(FOLD_X1, x) + y * FOLD_X1);
		P2.set(fold(FOLD_X2, x) + fold(FOLD_Y2, y) * FOLD_X2);
	}

	int part1 = P1.count();

	std::string part2(8, ' ');
	for (int idx = 0, x0 = 0; x0 < FOLD_X2; idx++, x0 += OCR_WIDTH) {
		part2[idx] = ocr(&P2,
			[x0](auto *p, int y, int x) {
				return p->test(FOLD_X2 * y + x0 + x);
			});
	}

	return { part1, part2 };
}
