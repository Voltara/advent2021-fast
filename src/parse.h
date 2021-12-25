#ifndef _ADVENT2021_PARSE_H
#define _ADVENT2021_PARSE_H

namespace parse {

static void skip(input_t &in, int n) {
	if (in.len < n) abort();
	in.s += n, in.len -= n;
}

template<typename T>
static T positive(input_t &in) {
	bool have = false;
	for (T n = 0; in.len; in.s++, in.len--) {
		uint8_t d = *in.s - '0';
		if (d <= 9) {
			n = 10 * n + d;
			have = true;
		} else if (have) {
			return n;
		}
	}
	return 0;
}

template<typename T>
static T integer(input_t &in) {
	bool have = false, neg = false;
	char prev = 0;
	for (T n = 0; in.len; in.s++, in.len--) {
		uint8_t d = *in.s - '0';
		if (d <= 9) {
			n = 10 * n + d;
			neg |= !have & (prev == '-');
			have = true;
		} else if (have) {
			return neg ? -n : n;
		}
		prev = *in.s;
	}
	return 0;
}

}

#endif
