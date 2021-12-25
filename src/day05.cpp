#include "advent2021.h"

namespace {

struct Line {
	int16_t x0, x1, y0, y1;

	void normalize() {
		if (y0 > y1 || (y0 == y1 && x0 > x1)) {
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
	}
};

struct Event {
	int16_t idx, y;

	Event() { }

	Event(int16_t idx, int16_t y) : idx(idx), y(y) { }
};

template<int Size, typename T>
auto histogram_sort(const std::vector<T> &input, auto key) {
	std::array<int16_t, Size> hist = { };
	for (auto &v : input) {
		hist[key(v) + 1]++;
	}
	std::partial_sum(hist.begin(), hist.end(), hist.begin());

	std::vector<T> sorted(input.size());
	for (auto &v : input) {
		sorted[hist[key(v)]++] = v;
	}

	return sorted;
}

template<int Size>
struct Bits {
	std::array<uint64_t, Size> B{};

	Bits operator | (Bits o) const {
		for (int i = 0; i < Size; i++) {
			o.B[i] |= B[i];
		}
		return o;
	}

	Bits operator & (Bits o) const {
		for (int i = 0; i < Size; i++) {
			o.B[i] &= B[i];
		}
		return o;
	}

	void shift_up() {
		uint64_t carry = 0;
		for (int i = 0; i < Size; i++) {
			B[i] = std::exchange(carry, B[i] >> 63) | (B[i] << 1);
		}
	}

	void shift_down() {
		for (int i = 1; i < Size; i++) {
			B[i - 1] = (B[i - 1] >> 1) | (B[i] << 63);
		}
		B[Size - 1] >>= 1;
	}

	void flip(int i) {
		B[i >> 6] ^= uint64_t(1) << (i & 63);
	}

	void set_range(int lo, int hi) {
		int i0 = lo >> 6, i1 = hi >> 6;
		lo &= 63, hi &= 63;

		constexpr uint64_t ones(-1);
		if (i0 == i1) {
			B[i0] |= (ones << lo) & (ones >> (63 - hi));
		} else {
			B[i0] |= ones << lo;
			for (int i = i0 + 1; i < i1; i++) {
				B[i] = ones;
			}
			B[i1] |= ones >> (63 - hi);
		}
	}

	int count() const {
		int n = 0;
		for (auto b : B) {
			n += __builtin_popcountll(b);
		}
		return n;
	}
};

template<int Size>
struct RowBitmap {
	Bits<Size> any; // at least one line
	Bits<Size> dup; // multiple lines

	auto count() const {
		return dup.count();
	}

	void merge_dup(RowBitmap &o) {
		dup = dup | o.dup | (any & o.any);
	}

	void merge(RowBitmap &o) {
		merge_dup(o);
		any = any | o.any;
	}

	void apply_horizontal_overlap(int a0, int a1, int b0, int b1) {
		a0 = std::max(a0, b0);
		a1 = std::min(a1, b1);

		any.set_range(b0, b1);
		if (a0 <= a1) {
			dup.set_range(a0, a1);
		}
	}

	void flip(int i, int count) {
		switch (count) {
		    case 1: any.flip(i); break;
		    case 2: dup.flip(i); break;
		}
	}

	void shift_up() {
		any.shift_up();
		dup.shift_up();
	}

	void shift_down() {
		any.shift_down();
		dup.shift_down();
	}
};

}

output_t day05(input_t in) {
	constexpr int DIM = 1000;

	std::vector<Line> lines;
	while (in.len > 0) {
		lines.emplace_back();
		auto &l = lines.back();
		l.x0 = parse::positive<int16_t>(in); parse::skip(in, 1);
		l.y0 = parse::positive<int16_t>(in); parse::skip(in, 4);
		l.x1 = parse::positive<int16_t>(in); parse::skip(in, 1);
		l.y1 = parse::positive<int16_t>(in); parse::skip(in, 1);
		l.normalize();
	}

	// Ensure that horizontal lines are handled in order
	lines = histogram_sort<DIM>(lines,
			[](const auto &l) { return l.x0; });

	// Create an event for start and end of each line
	std::vector<Event> events;
	for (int i = 0; i < lines.size(); i++) {
		auto & [ x0, x1, y0, y1 ] = lines[i];
		events.emplace_back(i, y0 * 2);
		events.emplace_back(i, y1 * 2 + 1);
	}

	// Sort events by increasing y-value
	events = histogram_sort<DIM * 2>(events,
			[](const Event &e) { return e.y; });
	events.emplace_back(INT16_MAX, 0);

	int part1 = 0, part2 = 0;

	// Counts of active segments on each vertical, diagonal, antidiagonal
	std::array<uint8_t, 1024> Vc{}, Dc{}, Ac{};

	// Bitmaps of the current row
	RowBitmap<16> V, D, A;

	auto ei = events.begin();
	for (int y = 0, n = 0; y < DIM; y++) {
		RowBitmap<16> H;

		// Shift the diagonals
		D.shift_up();
		A.shift_down();

		// Handle start-of-segment events
		int16_t h0 = -1, h1 = -1;
		for (; ei->y == n; ei++) {
			auto [ x0, x1, y0, y1 ] = lines[ei->idx];
			if (x0 == x1) {
				V.flip(x0, ++Vc[x0]);
			} else if (y0 == y1) {
				H.apply_horizontal_overlap(h0, h1, x0, x1);
				h0 = x0, h1 = std::max(h1, x1);
			} else if (x0 < x1) {
				D.flip(x0, ++Dc[(x0 - y0) & 1023]);
			} else {
				A.flip(x0, ++Ac[(x0 + y0) & 1023]);
			}
		}
		n++;

		// Count the crossings for the current row
		H.merge(V);
		part1 += H.count();
		H.merge(D);
		H.merge_dup(A);
		part2 += H.count();

		// Handle end-of-segment events
		for (; ei->y == n; ei++) {
			auto [ x0, x1, y0, y1 ] = lines[ei->idx];
			if (x0 == x1) {
				V.flip(x1, Vc[x1]--);
			} else if (y0 == y1) {
				// do nothing
			} else if (x0 < x1) {
				D.flip(x1, Dc[(x1 - y1) & 1023]--);
			} else {
				A.flip(x1, Ac[(x1 + y1) & 1023]--);
			}
		}
		n++;
	}

	return { part1, part2 };
}
