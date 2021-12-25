#include "advent2021.h"

output_t day02(input_t in) {
	enum { CMD_DOWN, CMD_UP, CMD_FORWARD, CMD_UNKNOWN };

	constexpr int8_t SKIP[] = {
		5, // "down "
		3, // "up "
		8, // "forward "
		0
	};

	int x = 0, aim = 0, depth = 0;

	while (in.len > 0) {
		int command = *in.s & 3;
		parse::skip(in, SKIP[command] + 2);
		int n = *(in.s - 2) - '0';

		/* Branchless conditionals.  They don't really help here,
		 * but it's a simple example in case you encounter it
		 * later in this solution set.
		 *
		 * The comparison evaluates to 0 or 1, and negating that
		 * gives 00000... or 11111... in binary, which we can
		 * use to mask the value being added or subtracted.
		 */
		aim   +=  n        & -(command == CMD_DOWN   );
		aim   -=  n        & -(command == CMD_UP     );
		x     +=  n        & -(command == CMD_FORWARD);
		depth += (n * aim) & -(command == CMD_FORWARD);
	}

	int part1 = x * aim;
	int part2 = x * depth;

	return { part1, part2 };
}
