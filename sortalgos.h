#ifndef SORTALGOS_H
#define SORTALGOS_H

#include <stddef.h>
#include <stdlib.h>

#include "rsort.h"

/*
 * Generic reference sorting algorithms (quicksort, insertion sort), used as
 * baselines to compare against the RSort algorithms in rsort.h. Same calling
 * convention as rsort2 / rsort2_ver2:
 *
 *     size_t sort(void *base, size_t nmemb, size_t size,
 *                 int (*compare)(const void *, const void *),
 *                 void (*swap)(void *base, size_t ia, size_t ib, size_t size));
 *
 * Named `quicksort`, not `qsort`, to avoid colliding with the <stdlib.h>
 * declaration of the standard library's qsort() (which has a different
 * signature — no swap callback, void return).
 *
 * Reuses rsort_elem() from rsort.h for element-pointer arithmetic.
 */

static inline size_t insertion_sort(void *base, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *),
		void (*swap)(void *base, size_t ia, size_t ib, size_t size)) {
	size_t swaps = 0;

	for(size_t i = 1; i < nmemb; ++i) {
		size_t j = i;
		while(j > 0 && compare(rsort_elem(base, size, j), rsort_elem(base, size, j - 1)) < 0) {
			swap(base, j, j - 1, size);
			++swaps;
			--j;
		}
	}

	return swaps;
}

static inline size_t quicksort_partition(void *base, size_t size,
		int (*compare)(const void *, const void *),
		void (*swap)(void *base, size_t ia, size_t ib, size_t size),
		size_t lo, size_t hi, size_t *swaps) {
	// Randomized pivot selection guards against the O(n^2) worst case that
	// a fixed (e.g. last-element) pivot hits on sorted/reverse-sorted input.
	size_t pivot_idx = lo + (size_t)rand() % (hi - lo + 1);
	if(pivot_idx != hi) {
		swap(base, pivot_idx, hi, size);
		++(*swaps);
	}

	void * pivot = rsort_elem(base, size, hi);
	size_t i = lo;
	for(size_t j = lo; j < hi; ++j) {
		if(compare(rsort_elem(base, size, j), pivot) < 0) {
			if(i != j) {
				swap(base, i, j, size);
				++(*swaps);
			}
			++i;
		}
	}

	if(i != hi) {
		swap(base, i, hi, size);
		++(*swaps);
	}

	return i;
}

static inline void quicksort_range(void *base, size_t size,
		int (*compare)(const void *, const void *),
		void (*swap)(void *base, size_t ia, size_t ib, size_t size),
		size_t lo, size_t hi, size_t *swaps) {
	if(lo >= hi) return;

	size_t p = quicksort_partition(base, size, compare, swap, lo, hi, swaps);

	if(p > lo) quicksort_range(base, size, compare, swap, lo, p - 1, swaps);
	if(p < hi) quicksort_range(base, size, compare, swap, p + 1, hi, swaps);
}

static inline size_t quicksort(void *base, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *),
		void (*swap)(void *base, size_t ia, size_t ib, size_t size)) {
	size_t swaps = 0;
	if(nmemb > 1) quicksort_range(base, size, compare, swap, 0, nmemb - 1, &swaps);
	return swaps;
}

#endif /* SORTALGOS_H */
