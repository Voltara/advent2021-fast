#ifndef _ADVENT2021_OCR_H
#define _ADVENT2021_OCR_H

constexpr int OCR_WIDTH = 5;

template<typename T, typename F>
static char ocr(T *p, F test) {
	if (test(p, 0, 3)) {
		if (test(p, 5, 3)) {
			if (test(p, 0, 2)) {
				return test(p, 1, 0) ? 'E' : 'Z';
			} else {
				return test(p, 1, 2) ? 'K' : 'H';
			}
		} else {
			if (test(p, 0, 2)) {
				return test(p, 0, 0) ? 'F' : 'J';
			} else {
				return 'U';
			}
		}
	} else {
		if (test(p, 5, 3)) {
			if (test(p, 0, 0)) {
				return test(p, 0, 1) ? 'R' : 'L';
			} else {
				return test(p, 2, 3) ? 'A' : 'G';
			}
		} else {
			if (test(p, 5, 1)) {
				return test(p, 0, 0) ? 'B' : 'C';
			} else {
				return test(p, 0, 1) ? 'P' : 'Y';
			}
		}
	}
	return '?';
}

#endif
