#include "advent2021.h"

namespace {

struct snumber_t {
	std::array<uint8_t, 16> v;
	uint8_t carry_left = 0;
	uint8_t carry_right = 0;

	snumber_t() {
	}

	snumber_t(input_t &in) : v() {
		int step = 16, idx = 0;
		for (; *in.s != '\n'; in.s++, in.len--) {
			switch (*in.s) {
			    case '[': step >>= 1; break;
			    case ']': step <<= 1; break;
			    case ',': break;
			    default:
				v[idx] = *in.s - '0' + 1;
				idx += step;
			}
		}
	}

	void pre_explode() {
		uint8_t *left1 = &carry_left, *left2 = NULL;
		for (int i = 0; i < 16; i++) {
			if (!v[i]) continue;
			v[i] += std::exchange(carry_right, 0);
			if (i & 1) {
				*left2 += std::exchange(v[i - 1], 1) - 1;
				carry_right = std::exchange(v[i], 0) - 1;
			} else {
				left2 = std::exchange(left1, &v[i]);
			}
		}
		for (int i = 1; i < 8; i++) v[i] = v[i * 2];
		for (int i = 0; i < 8; i++) v[i + 8] = v[i];
	}

	int magnitude() const {
		std::array<uint16_t, 16> M;
		for (int i = 0; i < 16; i++) {
			M[i] = v[i] - (v[i] != 0);
		}
		for (int s = 1; s < 16; s <<= 1) {
			for (int i = s; i < 16; i += s * 2) {
				if (v[i]) M[i - s] += 2 * (M[i - s] + M[i]);
			}
		}
		return M[0];
	}

	snumber_t append(const snumber_t &o) const {
		snumber_t s;
		s.v = v;
		int i;
		for (i = 8; i < 16; i++) {
			s.v[i] = o.v[i];
		}
		if (o.carry_left) {
			for (i = 7; !s.v[i]; i--) { }
			s.v[i] += o.carry_left;
		}
		return s;
	}

	snumber_t operator + (const snumber_t &o) const {
		snumber_t s = append(o);

		for (bool done = false; !done; ) {
			done = true;
			for (int i = 0; i < 16; i++) {
				if (s.v[i] <= 10) continue;
				done = false;

				int a = (s.v[i] + 1) / 2, b = (s.v[i] / 2) + 1;

				int bit = ((16 + i) & -(16 + i));
				for (bit >>= 1; bit && s.v[i | bit]; bit >>= 1) { }

				if (bit) {
					s.v[i] = a;
					s.v[i | bit] = b;
				} else {
					int idx;
					for (idx = i - 1; idx >= 0 && !s.v[idx]; idx--) { }
					if (idx >= 0) s.v[idx] += a - 1;
					if (i < 15) s.v[i + 1] += b - 1;
					s.v[i] = 1;
				}
				break;
			}
		}

		return s;
	}
};

}

output_t day18(input_t in) {
	std::vector<snumber_t> V;
	for (; in.len > 0; in.s++, in.len--) {
		V.emplace_back(in);
	}

	auto sum = V[0];
	for (int i = 1; i < V.size(); i++) {
		sum.pre_explode();
		auto addend = V[i];
		addend.v[0] += sum.carry_right;
		addend.pre_explode();
		sum = sum + addend;
	}
	int part1 = sum.magnitude();

	int part2 = 0;
	for (int i = 0; i < V.size(); i++) {
		auto a = V[i];
		a.pre_explode();
		for (int j = 0; j < V.size(); j++) {
			if (i == j) continue;
			auto b = V[j];
			b.v[0] += a.carry_right;
			b.pre_explode();
			part2 = std::max(part2, (a + b).magnitude());
		}
	}

	return { part1, part2 };
}
