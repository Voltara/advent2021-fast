#include "advent2021.h"

namespace {

/* Room contents are packed into a 32-bit integer, 2 bits per location.
 * Although each location can be in any of 5 states (A, B, C, D, empty),
 * by not introducing gaps between amphipods, we don't need "empty".
 */
constexpr uint32_t ROOM_XOR = 0xffaa5500;
constexpr uint16_t COST[] = { 1, 10, 100, 1000 };

// [8*room + hall]
// moves to mask after a given previous move
constexpr uint32_t SafeSkips[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0x3, 0x7, 0x7, 0x7, 0x7, 0,
	0, 0, 0x3, 0x707, 0xf0f, 0xf0f, 0xf0f, 0,
	0, 0, 0x3, 0x707, 0xf0f0f, 0x1f1f1f, 0x1f1f1f, 0,
};

uint32_t read_input(const char *str) {
	auto to_glyph = [](char c) { return c - 'A'; };
	uint32_t room = 0;
	for (int i = 0; i < 4; i++, str += 2) {
		room |= to_glyph(str[31]) << ((8 * i) + 0);
		room |= to_glyph(str[45]) << ((8 * i) + 2);
	}
	return room ^ (ROOM_XOR & 0x0f0f0f0f);
}

// Insert the Part 2 occupants into the input rooms
uint32_t insert_part2(uint32_t room) {
	room = (room & 0x03030303) | ((room << 4) & 0xc0c0c0c0);
	return room ^ 0x1c2c0c3c;
}

/* Allowing multiple amphipods to occupy a location, how much energy is
 * required to solve the starting position?  Assumes every amphipod must
 * leave its room at least once, which should be true of any input.
 */
auto base_cost(uint32_t room) {
	room ^= ROOM_XOR;
	auto move_cost = [](int dist) {
		return 2 * std::max(1, std::abs(dist));
	};
	int base = 0, second_row = 0;
	for (int i = 0; i < 4; i++) {
		int glyph0 = room & 3, glyph1 = (room >> 2) & 3;
		int cost0 = COST[glyph0], cost1 = COST[glyph1];
		base += cost0 * (move_cost(i - glyph0) + 1);
		base += cost1 * move_cost(i - glyph1);
		second_row += cost1;
		room >>= 8;
	}
	int cost1 = base + second_row * 2 + 3333;
	int cost2 = base + second_row * 4 + 29115;
	return std::make_pair(cost1, cost2);
}

struct Room {
	uint32_t room;

	Room(uint32_t room = 0) : room(room) {
	}

	operator uint32_t () const {
		return room;
	}

	bool empty(int r) const {
		return !((room >> (8 * r)) & 0xff);
	}

	uint32_t get(int r) const {
		return r ^ ((room >> (8 * r)) & 3);
	}

	void pop(int r) {
		uint32_t mask1 = 0xff << (8 * r);
		uint32_t mask2 = 0x3f << (8 * r);
		room = ((room >> 2) & mask2) | (room & ~mask1);
	}

};

struct Hall {
	uint32_t hall;

	Hall(uint32_t hall = 0) : hall(hall) {
	}

	operator uint32_t () const {
		return hall;
	}

	bool empty(int h) const {
		return !(hall & (4 << (4 * h)));
	}

	void clear(int h) {
		hall &= ~(0xf << (4 * h));
	}

	void set(int h, int g) {
		hall |= (4 | g) << (4 * h);
	}

	int get(int h) const {
		return (hall >> (4 * h)) & 3;
	}

	uint32_t mask() const {
		return hall & 0x4444444;
	}
};

struct State {
	Room room;
	Hall hall;

	State() : room(), hall() {
	}

	State(uint64_t hash) : room(hash), hall(hash >> 32) {
	}

	uint64_t hash() const {
		return (uint64_t(hall) << 32) | room;
	}

	bool solved() const {
		return !(room | hall);
	}

	static int room_L(int r) { return r + 1; }

	static int room_R(int r) { return r + 2; }

	bool obstructed(int r, int h) const {
		int lo, hi;
		if (h <= room_L(r)) {
			lo = h + 1;
			hi = room_L(r);
		} else {
			lo = room_R(r);
			hi = h - 1;
		}
		uint32_t mask = (16 << (4 * hi)) - (1 << (4 * lo));
		return !!(hall & mask);
	}

	bool force_one() {
		// can move from hallway to room?
		for (auto b : bits(hall.mask())) {
			int h = b / 4;
			int r = hall.get(h);
			if (room.empty(r) && !obstructed(r, h)) {
				hall.clear(h);
				return true;
			}
		}

		// can move from room to room?
		for (int r = 0; r < 4; r++) {
			if (room.empty(r)) continue;
			int g = room.get(r);
			if (g == r || !room.empty(g)) continue;
			if (!obstructed(r, (r < g) ? room_R(g) : room_L(g))) {
				room.pop(r);
				return true;
			}
		}

		return false;
	}

	bool deadlocked() const {
		// Two amphipods in the hallway are deadlocked
		// if they must cross each other
		uint32_t h43 = hall & 0x0077000;
		if (h43 == 0x0047000) return true;
		if (h43 == 0x0057000) return true;
		uint32_t h42 = hall & 0x0070700;
		if (h42 == 0x0040700) return true;
		uint32_t h32 = hall & 0x0007700;
		if (h32 == 0x0004600) return true;
		if (h32 == 0x0004700) return true;
		return false;
	}

	bool crowded() const {
		// A hallway is too crowded if there is not enough
		// space to empty out any of the rooms
		int h0 = 0, h1 = 0;
		uint32_t H = (hall >> 2) | 0x10000000;
		bool satisfied = false;
		for (int i = 0; i < 8; i++) {
			if (H & 1) {
				if (h0 < i) {
					int r0 = std::max(0, h0 - 2);
					int r1 = std::min(3, i - 2);
					int space = i - h0;
					int mask = 3 << (2 * space);
					for (int r = r0; r <= r1; r++) {
						uint32_t rr = room >> (8 * r);
						rr &= 0xff;
						if (!(rr & mask)) {
							satisfied = true;
						}
					}
				}
				h0 = i + 1;
			}
			H >>= 4;
		}
		return !satisfied;
	}

	auto neighbors(uint32_t skip = 0) const {
		std::vector<std::tuple<int, State, uint32_t>> N;

		// TODO see if a good ranges implementation can speed things up

		// Amphipod in the hallway between rooms might
		// split the board into independent parts,
		// so we should impose a move ordering
		int skip_rooms = 0;
		for (uint32_t i = 0, h = 2; i < 3; i++, h++) {
			if (!hall.empty(h)) {
				int mask = 0b1110 << i;
				skip_rooms |= (i < hall.get(h)) ? ~mask : mask;
			}
		}

		for (int r = 0; r < 4; r++) {
			if (skip_rooms & (1 << r)) continue;

			if (room.empty(r)) continue;

			int g = room.get(r);
			int lo, hi;
			if (r < g) {
				lo = room_R(r);
				hi = room_L(g);
			} else if (g < r) {
				lo = room_R(g);
				hi = room_L(r);
			} else {
				lo = room_L(r);
				hi = room_R(r);
			}

			for (int h = 0; h < 7; h++) {
				// Don't stop along a direct path
				if (r != g && h >= lo && h <= hi) continue;

				// Impose ordering on parallel moves
				int skip_idx = 8 * r + h;
				if ((skip >> skip_idx) & 1) {
					continue;
				}

				if (!hall.empty(h) || obstructed(r, h)) continue;
				int cost = (h < lo) ? lo - h : (hi < h) ? h - hi : 0;
				cost *= 2;
				cost -= !(!cost | (r == g)) + (!h | (h == 6));
				cost *= 2;
				State n = *this;
				n.room.pop(r);
				n.hall.set(h, g);
				if (n.deadlocked()) continue;
				uint32_t skips = SafeSkips[skip_idx];
				while (n.force_one()) {
					skips = 0;
				}
				if (n.crowded()) continue;
				N.emplace_back(cost * COST[g], n, skips);
			}
		}

		return N;
	}
};

template<int Size, typename TValue>
struct Hash {
	std::vector<std::pair<uint64_t, TValue>> table;
	Hash() : table(Size) {
	}
	int find(uint64_t key) const {
		int idx = key % Size;
		while (table[idx].first && table[idx].first != ~key) {
			idx++;
			idx &= -(idx < Size);
		}
		return idx;
	}
	void insert(uint64_t key, TValue value) {
		auto idx = find(key);
		table[idx] = { ~key, value };
	}
	void set(int idx, TValue value) {
		table[idx].second = value;
	}
	TValue & get(int idx) {
		return table[idx].second;
	}
	bool exists(int idx) const {
		return table[idx].first != 0;
	}
};

int solve(State start) {
	Hash<14983, std::pair<uint16_t, uint32_t>> Cost;
	Cost.insert(start.hash(), { });

	using TQueue = std::pair<int, uint64_t>;
	std::priority_queue<TQueue, std::vector<TQueue>, std::greater<TQueue>> Q;
	Q.emplace(0, start.hash());

	while (!Q.empty()) {
		auto [ queue_cost, cur_hash ] = Q.top();
		Q.pop();

		auto [ cur_cost, cur_skips ] = Cost.get(Cost.find(cur_hash));
		if (queue_cost != cur_cost) {
			continue;
		}

		State cur(cur_hash);
		if (cur.solved()) {
			break;
		}

		auto neighbors = cur.neighbors(cur_skips);
		for (auto [ delta, state, skips ] : neighbors) {
			uint64_t hash = state.hash();
			int new_cost = cur_cost + delta;

			auto new_idx = Cost.find(hash);
			auto & [ prev_cost, prev_skips ] = Cost.get(new_idx);

			if (!Cost.exists(new_idx)) {
				Cost.insert(hash, { new_cost, skips });
				Q.emplace(new_cost, hash);
			} else if (new_cost == prev_cost) {
				prev_skips &= skips;
			} else if (new_cost < prev_cost) {
				prev_cost = new_cost;
				prev_skips = skips;
				Q.emplace(new_cost, hash);
			}
		}
	}

	return Cost.get(Cost.find(0)).first;
}

}

output_t day23(input_t in) {
	if (in.len != 66) abort();

	uint32_t room_p1 = read_input(in.s);
	uint32_t room_p2 = insert_part2(room_p1);
	auto [ base_cost1, base_cost2 ] = base_cost(room_p1);

	int part1 = base_cost1 + solve(room_p1);
	int part2 = base_cost2 + solve(room_p2);

	return { part1, part2 };
}
