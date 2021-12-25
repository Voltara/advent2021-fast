#include "advent2021.h"

constexpr int WIDTH = 139, HEIGHT = 137;

namespace {

struct Cucumber {
	// Wasting a uint64_t to give 256-bit SIMD a chance
	std::array<std::array<uint64_t, 4>, HEIGHT> C;

	bool operator != (const Cucumber &o) const {
		return C != o.C;
	}

	Cucumber operator | (const Cucumber &o) const {
		Cucumber result;
		for (int r = 0; r < HEIGHT; r++) {
			for (int i = 0; i < 4; i++) {
				result.C[r][i] = C[r][i] | o.C[r][i];
			}
		}
		return result;
	}

	Cucumber operator & (const Cucumber &o) const {
		Cucumber result;
		for (int r = 0; r < HEIGHT; r++) {
			for (int i = 0; i < 4; i++) {
				result.C[r][i] = C[r][i] & o.C[r][i];
			}
		}
		return result;
	}

	Cucumber operator ^ (const Cucumber &o) const {
		Cucumber result;
		for (int r = 0; r < HEIGHT; r++) {
			for (int i = 0; i < 4; i++) {
				result.C[r][i] = C[r][i] ^ o.C[r][i];
			}
		}
		return result;
	}

	Cucumber shift_up() const {
		Cucumber result;
		result.C[HEIGHT - 1] = C[0];
		for (int r = 1; r < HEIGHT; r++) {
			result.C[r - 1] = C[r];
		}
		return result;
	}

	Cucumber shift_down() const {
		Cucumber result;
		result.C[0] = C[HEIGHT - 1];
		for (int r = 1; r < HEIGHT; r++) {
			result.C[r] = C[r - 1];
		}
		return result;
	}

	Cucumber shift_left() const {
		Cucumber result;
		for (int r = 0; r < HEIGHT; r++) {
			result.C[r][0] = (C[r][0] >> 1) | (C[r][1] << 63);
			result.C[r][1] = (C[r][1] >> 1) | (C[r][2] << 63);
			result.C[r][2] = (C[r][2] >> 1) | (C[r][0] << 10);
			result.C[r][2] &= 0x7ff;
			result.C[r][3] = 0;
		}
		return result;
	}

	Cucumber shift_right() const {
		Cucumber result;
		for (int r = 0; r < HEIGHT; r++) {
			result.C[r][0] = (C[r][0] << 1) | (C[r][2] >> 10);
			result.C[r][1] = (C[r][1] << 1) | (C[r][0] >> 63);
			result.C[r][2] = (C[r][2] << 1) | (C[r][1] >> 63);
			result.C[r][2] &= 0x7ff;
			result.C[r][3] = 0;
		}
		return result;
	}
};

Cucumber advance_right(const Cucumber &D, const Cucumber &R) {
	Cucumber result = R.shift_right();
	Cucumber blocked = result & (D | R);
	return (result ^ blocked) | blocked.shift_left();
}

Cucumber advance_down(const Cucumber &D, const Cucumber &R) {
	Cucumber result = D.shift_down();
	Cucumber blocked = result & (D | R);
	return (result ^ blocked) | blocked.shift_up();
}

}

output_t day25(input_t in) {
	if (in.len != (WIDTH + 1) * HEIGHT) abort();

	auto to_mask = [](int count, const char *p) {
		uint64_t D_mask = 0, R_mask = 0;
		for (int i = 0; i < count; i++) {
			D_mask |= uint64_t(p[i] == 'v') << i;
			R_mask |= uint64_t(p[i] == '>') << i;
		}
		return std::make_pair(D_mask, R_mask);
	};

	Cucumber D, R;
	for (int r = 0; r < HEIGHT; r++) {
		auto &D_row = D.C[r], &R_row = R.C[r];
		std::tie(D_row[0], R_row[0]) = to_mask(64, in.s);
		std::tie(D_row[1], R_row[1]) = to_mask(64, in.s + 64);
		std::tie(D_row[2], R_row[2]) = to_mask(11, in.s + 128);
		D_row[3] = R_row[3] = 0;
		in.s += WIDTH + 1;
	}

	int part1 = 0;

	for (bool done = false; !done; part1++) {
		done = true;
		Cucumber next_R = advance_right(D, R);
		done = done && !(next_R != R);
		R = next_R;
		Cucumber next_D = advance_down(D, R);
		done = done && !(next_D != D);
		D = next_D;
	}

	return { part1, 0 };
}
