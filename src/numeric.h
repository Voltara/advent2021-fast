#ifndef _ADVENT2021_NUMERIC
#define _ADVENT2021_NUMERIC

template<typename T>
static T fastmod(T n, T mod) {
	n -= mod & -(n >= mod);
	return n;
}

#endif
