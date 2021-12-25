#include "advent2021.h"

output_t day08(input_t in) {
	auto parse_slug = [](char *&p) {
		int slug = 0;
		auto begin = p;
		for (; *p >= 'a'; p++) {
			slug |= 1 << (*p - 'a');
		}
		return std::make_pair(slug, p - begin);
	};

	auto is_easy_length = [](int len) {
		return (0x9c >> len) & 1;
	};

	auto by_length = [](int len) {
		return (0x8fb47100 >> (len * 4)) & 0xf;
	};

	auto by_magic = [](int magic) {
		return (0x09600325 >> (magic * 4)) & 0xf;
	};

	std::array<int8_t, 128> digits;
	std::array<uint8_t, 10> slugs;

	int part1 = 0, part2 = 0;

	for (char *p = in.s, *end = p + in.len; p < end; ) {
		// First-pass scan over the digits
		uint8_t slugs_and = 0xff, slugs_xor = 0;
		for (int i = 0; i < 10; i++, p++) {
			auto [ slug, len ] = parse_slug(p);
			slugs[i] = slug;
			digits[slug] = by_length(len);
			if (is_easy_length(len)) {
				slugs_and &= slug;
				slugs_xor ^= slug;
			}
		}
		p += 2;

		// Finish mapping the digits
		for (int i = 0; i < 10; i++) {
			int slug = slugs[i], magic = digits[slug];
			if (magic > 10) {
				magic &= 4;
				magic |= !(~slug & slugs_and) << 1;
				magic |= !(~slug & slugs_xor);
				digits[slug] = by_magic(magic);
			}
		}

		// Solve part1 + part2
		int number = 0;
		for (int i = 0; i < 4; i++, p++) {
			auto [ slug, len ] = parse_slug(p);
			part1 += is_easy_length(len);
			number = 10 * number + digits[slug];
		}
		part2 += number;
	}

	return { part1, part2 };
}
