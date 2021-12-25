#include "advent2021.h"

namespace {

struct bitstream {
	std::vector<uint64_t> B;
	int idx = 0, cur = 64, len = 0;

	void push_hex(uint64_t nibble) {
		if (len % 64 == 0) B.push_back(0);
		B.back() |= nibble << (60 - len % 64);
		len += 4;
	}

	uint64_t take(int n) {
		uint64_t value;
		if (n <= cur) {
			cur -= n;
			value = B[idx] >> cur;
		} else {
			value = B[idx++] << (n - cur);
			cur = 64 - n + cur;
			value |= B[idx] >> cur;
		}
		len -= n;
		return value & ((1LL << n) - 1);
	}

	uint64_t take_varint() {
		uint64_t frag = take(5), value = 0;
		while (frag & 0x10) {
			value |= frag ^ 0x10;
			value <<= 4;
			frag = take(5);
		}
		return value | frag;
	}

	auto take_version_type() {
		int n = take(6);
		return std::make_pair((n >> 3) & 7, n & 7);
	}
};

std::pair<int64_t, int64_t> solve(bitstream &B) {
	auto [ version, type ] = B.take_version_type();

	if (type == 4) {
		return { version, B.take_varint() };
	}

	int packets = INT32_MAX, end = 0;

	if (B.take(1) == 0) {
		int length = B.take(15);
		end = B.len - length;
	} else {
		packets = B.take(11);
	}

	auto [ sum, value ] = solve(B);
	sum += version;

	while (--packets && B.len > end) {
		auto [ child_sum, child_value ] = solve(B);
		sum += child_sum;
		switch (type) {
		    case 0: value += child_value;                 break;
		    case 1: value *= child_value;                 break;
		    case 2: value = std::min(value, child_value); break;
		    case 3: value = std::max(value, child_value); break;
		    case 5: value = (value >  child_value);       break;
		    case 6: value = (value <  child_value);       break;
		    case 7: value = (value == child_value);       break;
		}
	}

	return { sum, value };
}

}

output_t day16(input_t in) {
	bitstream B;
	in.len--;
	for (int i = 0; i < in.len; i++) {
		in.s[i] -= '0' + ((in.s[i] >> 4) ^ 3);
	}
	for (int i = 0; i < in.len; i++) {
		B.push_hex(in.s[i]);
	}

	auto [ part1, part2 ] = solve(B);

	return { part1, part2 };
}
