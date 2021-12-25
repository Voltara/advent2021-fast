#include "advent2021.h"

namespace {

constexpr int DIM = 100;

auto input_rules(const char *in) {
	// Convert input to single-bit rules, but rearrange the
	// index bits from 876543210 to 876_543_210
	std::array<uint8_t, 2048> B;
	for (int i = 0, idx = 0; i < 512; i++) {
		B[idx] = in[i] & 1;
		idx = (idx + 0x88 + 1) & 0x777;
	}
	return B;
}

auto convert_rules_to_quads(const auto &B) {
	std::array<uint8_t, 65536> Q;
	for (int i = 0; i < Q.size(); i++) {
		uint8_t quad = 0;
		quad |= B[(i >> 5) & 0x777] << 5;
		quad |= B[(i >> 4) & 0x777] << 4;
		quad |= B[(i >> 1) & 0x777] << 1;
		quad |= B[(i >> 0) & 0x777] << 0;
		Q[i] = quad;
	}
	return Q;
}

auto input_grid_to_quads(const char *in) {
	constexpr int QDIM = DIM / 2;

	std::array<uint8_t, DIM * DIM> grid{};

	auto row = &grid[0];
	for (int r = 0; r < QDIM; r++) {
		for (int c = 0; c < QDIM; c++) {
			uint8_t quad = 0;
			quad |= (in[0]   & 1) << 5;
			quad |= (in[1]   & 1) << 4;
			quad |= (in[101] & 1) << 1;
			quad |= (in[102] & 1) << 0;
			row[c] = quad;
			in += 2;
		}
		in += DIM + 2;
		row += DIM;
	}

	return grid;
}

}

output_t day20(input_t in) {
	if (in.len != 514 + (DIM + 1) * DIM) abort();

	auto Q = convert_rules_to_quads(input_rules(in.s));
	auto grid = input_grid_to_quads(in.s + 514);

	auto advance = [&grid, &Q](uint16_t xor_in, uint8_t xor_out, int dim) {
		std::array<uint8_t, DIM> prev{};
		auto row = &grid[0];
		for (int r = 0; r < dim; r++) {
			uint16_t quad = 0;
			for (int c = 0; c < dim; c++) {
				quad = (quad & 0x3333) << 2;
				quad |= (prev[c] << 8) | row[c];
				prev[c] = row[c];
				row[c] = Q[quad ^ xor_in] ^ xor_out;
			}
			row += DIM;
		}
	};

	auto count = [&grid]() {
		int n = 0;
		for (auto g : grid) {
			n += __builtin_popcount(g);
		}
		return n;
	};

	// And it even works when infinity doesn't flip-flop
	uint16_t xor_in = Q[0] ? 0xffff : 0;
	uint8_t xor_out = Q[0] ? 0x33   : 0;

	int step = 0;

	while (step < 2) {
		advance(0, xor_out, 50 + (++step));
		advance(xor_in,  0, 50 + (++step));
	}
	int part1 = count();

	while (step < 50) {
		advance(0, xor_out, 50 + (++step));
		advance(xor_in,  0, 50 + (++step));
	}
	int part2 = count();

	return { part1, part2 };
}
