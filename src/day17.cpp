#include "advent2021.h"

namespace {
 
// Closed interval
struct range {
	int lo, hi;

	range() : lo(), hi() {
	}

	range(int lo, int hi) : lo(lo), hi(hi) {
	}

	range(input_t &in) {
		lo = parse::integer<int>(in);
		hi = parse::integer<int>(in);
	}

	int size() const {
		return (lo <= hi) ? (hi - lo + 1) : 0;
	}

	range intersect(const range &o) const {
		return range{std::max(lo, o.lo), std::min(hi, o.hi)};
	}

	range min(const range &o) const {
		return range{std::min(lo, o.lo), std::min(hi, o.hi)};
	}
};

static int floor_div(int n, int d) {
	int q = n / d, r = n % d;
	return q - (r && ((n < 0) ^ (d < 0)));
}

static int ceil_div(int n, int d) {
	int q = n / d, r = n % d;
	return q + (r && !((n < 0) ^ (d < 0)));
}

/* Get the vy-bounds to hit (L <= y <= H)
 *   2L/n + n - 1 <= 2v <= 2H/n + n - 1
 */
static int velocity_bound(int L, int n, auto div_fn) {
	/* This can be simplified to a single division, but
	 * two divisions clarifies how hyper_count works
	 */
	return div_fn(div_fn(2*L, n) + n - 1, 2);
}

static int hyper_count(range ry, int min_velocity) {
	int count = 0;

	range vy_bound{min_velocity, -ry.lo};

	for (int k = 1; k <= -ry.hi && vy_bound.size() > 0; k++) {
		range rn{ceil_div(-ry.hi*2, k), floor_div(-ry.lo*2, k)};

		// Adjustments to account for velocity_bound rounding
		rn.lo += (rn.lo ^ ~k) & 1;
		rn.hi -= (rn.hi ^ ~k) & 1;

		if (rn.size() == 0) continue;

		range rvy{
			velocity_bound(ry.lo, rn.lo, ceil_div),
			velocity_bound(ry.hi, rn.hi, floor_div)};

		rvy = rvy.intersect(vy_bound);
		count += rvy.size();
		vy_bound.hi = std::min(vy_bound.hi, rvy.lo - 1);
	}

	return count;
}

static int day17_part2(range rx, range ry) {
	/* Get the vx-bounds to hit (L <= x <= H)
	 *   2L/n + n - 1 <= 2v <= 2H/n + n - 1
	 * Except that vx stops at zero.
	 * We should be able to detect this by taking the
	 * minimum boundary value seen so far.
	 */
	int answer = 0;
	range prev_rvx{INT32_MAX, INT32_MAX}, prev_rvy;

	int n = 1;
	for (bool done = false; !done; n++) {
		range rvx{
			velocity_bound(rx.lo, n, ceil_div),
			velocity_bound(rx.hi, n, floor_div)};
		range rvy{
			velocity_bound(ry.lo, n, ceil_div),
			velocity_bound(ry.hi, n, floor_div)};

		// Stop when the upper bound of x-velocity begins climbing
		done = rvx.hi > prev_rvx.hi;

		// Ignore boundaries that increase
		rvx = rvx.min(prev_rvx);

		// Add new pairs, subtract overlap with previous iteration
		answer += rvx.size() * rvy.size();
		answer -= rvx.intersect(prev_rvx).size() * rvy.intersect(prev_rvy).size();

		prev_rvx = rvx;
		prev_rvy = rvy;
	}

	for (int max_n = -ry.hi / n; n < max_n; n++) {
		range rvy{
			velocity_bound(ry.lo, n, ceil_div),
			velocity_bound(ry.hi, n, floor_div)};
		answer += prev_rvx.size() * (rvy.size() - rvy.intersect(prev_rvy).size());
		prev_rvy = rvy;
	}

	auto raw_hyper = hyper_count(ry, prev_rvy.hi + 1);
	answer += prev_rvx.size() * raw_hyper;

	return answer;
}

}

output_t day17(input_t in) {
	range rx{in};
	range ry{in};

	int part1 = ry.lo * (ry.lo + 1) / 2;
	int part2 = day17_part2(rx, ry);

	return { part1, part2 };
}
