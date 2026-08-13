#ifndef RSORT_H
#define RSORT_H

#include <stddef.h>

/*
 * Generalized, qsort-style versions of the RSort2 / RSort2_ver2 algorithms
 * from rsort.c. (RSort1 is intentionally omitted here: its recursion blows
 * up exponentially with nmemb, so it isn't usable at general sizes.)
 *
 * The algorithms themselves are unchanged: same control flow, same
 * comparisons, same swap points. They are only generalized to operate on
 * arbitrary-sized elements via a base pointer + element size + comparator,
 * instead of a hardcoded `int` array — mirroring qsort's signature:
 *
 *     void qsort(void *base, size_t nmemb, size_t size,
 *                int (*compare)(const void *, const void *));
 *
 * Unlike qsort, each function here also takes a `swap` callback (called as
 * swap(a, b, size) to exchange two elements) and returns the number of
 * element swaps it performed (matching what the original int-array
 * versions returned). Pass rsort_swap for the default byte-wise swap.
 */

static inline void *rsort_elem(void *base, size_t size, size_t idx) {
	return (unsigned char *)base + idx * size;
}

static inline void rsort_swap(void *a, void *b, size_t size) {
	unsigned char *pa = (unsigned char *)a;
	unsigned char *pb = (unsigned char *)b;
	while (size--) {
		unsigned char tmp = *pa;
		*pa = *pb;
		*pb = tmp;
		++pa;
		++pb;
	}
}

static inline size_t rsort2(void *base, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *),
		void (*swap)(void *a, void *b, size_t size)) {
	size_t swaps = 0;
	size_t p, s, x, n;

	//??? we might can replace all occurences of x with n
	n = nmemb;

	the_beginning:
	s = 0;

	state0:
	if(n < 2) return swaps;
	for(p = s + 1; p != n; ++p) {
		if(compare(rsort_elem(base, size, p), rsort_elem(base, size, s)) > 0) {
			// ??? if (p == x) ...
			x = p + 1;
			//if(x == n) { n--; break; } // needs some work
			goto state1;
		}
		//...
		//s++;
		//p = s;
	}

	// ??? arr[s] is the biggest element. swap with last element
	x = p; // ???!!!
	p = s; // ???!!!
	s++;
	n--;
	swap(rsort_elem(base, size, p), rsort_elem(base, size, n), size);
	swaps++;
	goto the_beginning;

	state1:
	for(; x != n; ++x) {
		if(compare(rsort_elem(base, size, p), rsort_elem(base, size, x)) < 0) {
			p = x;
		}
	}

	do_the_swapping:
	if(p != x - 1) { swap(rsort_elem(base, size, p), rsort_elem(base, size, x - 1), size); swaps++; }
	n--;
	// ??? if(p != s && arr[p] < arr[s]) swap(p, s) ???
	if(s == n) { goto the_beginning; }
	goto state0;
}

static inline size_t rsort2_ver2(void *base, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *),
		void (*swap)(void *a, void *b, size_t size)) {
	size_t swaps = 0;
	size_t p, s, x, n;

	//??? we might can replace all occurences of x with n
	n = nmemb;

	the_beginning:
	s = 0;

	state0:
	if(n < 2) return swaps;
	for(p = s + 1; p != n; ++p) {
		if(compare(rsort_elem(base, size, p), rsort_elem(base, size, s)) >= 0) {
			// ??? if (p == x) ...
			x = p + 1;
			//if(x == n) { n--; break; } // needs some work
			goto state1;
		}
	}

	// ??? arr[s] is the biggest element. swap with last element
	x = p; // ???!!!
	p = s; // ???!!!
	s++;
	n--;
	swap(rsort_elem(base, size, p), rsort_elem(base, size, n), size);
	swaps++;
	goto the_beginning;

	state1:
	for(; x != n; ++x) {
		if(compare(rsort_elem(base, size, p), rsort_elem(base, size, x)) <= 0) {
			p = x;
		}
	}

	do_the_swapping:
	if(p != x - 1) { swap(rsort_elem(base, size, p), rsort_elem(base, size, x - 1), size); swaps++; }
	n--;
	// ??? if(p != s && arr[p] < arr[s]) swap(p, s) ???
	if(s == n) { goto the_beginning; }
	goto state0;
}

#endif /* RSORT_H */
