#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include "sortalgos.h"

typedef size_t (*sort_fp)(void *base, size_t nmemb, size_t size,
	int (*compare)(const void *, const void *),
	void (*swap)(void *base, size_t ia, size_t ib, size_t size));

int cmp_int(const void * a, const void * b) {
	int ia = *(const int *)a, ib = *(const int *)b;
	return (ia > ib) - (ia < ib);
}

void int_swap(void * base, size_t ia, size_t ib, size_t size) {
	printf("[swap %d,%d]", *(int *)rsort_elem(base, size, ia), *(int *)rsort_elem(base, size, ib));
	rsort_swap(base, ia, ib, size);
}

bool check_sorted(int * arr, int N) {
	for(int i = 1; i < N; ++i) {
		if(arr[i] < arr[i-1]) return false;
	}

	return true;
}

#define NUMARGS(...)( (sizeof((int[]){__VA_ARGS__})/sizeof(int)) )
#define test(sorter, ...) _impl_test( NUMARGS(__VA_ARGS__), ((int[]){__VA_ARGS__}), sorter )

void print_arr(int * arr, int num) {
	for(int i = 0; i != num; ++i) {
		if(i > 0) printf(", ");
		printf("%d", arr[i]);
	}
}

void _impl_test(int num, int * arr, sort_fp sorter) {
	print_arr(arr, num);
	printf("\t");
	size_t swaps = sorter(arr, (size_t)num, sizeof(int), cmp_int, int_swap);
	printf("\t(%zu swaps)\n", swaps);
	assert(check_sorted(arr, num));
}

void run_test(sort_fp sorter) {
	test(sorter, 1,2);
	test(sorter, 2,1);

	test(sorter, 1,2,3);
	test(sorter, 1,3,2);
	test(sorter, 2,1,3);
	test(sorter, 2,3,1);
	test(sorter, 3,1,2);
	test(sorter, 3,2,1);

	// all permutations of 4
	test(sorter, 1,2,3,4);
	test(sorter, 1,2,4,3);
	test(sorter, 1,3,2,4);
	test(sorter, 1,3,4,2);
	test(sorter, 1,4,2,3);
	test(sorter, 1,4,3,2);

	test(sorter, 2,1,3,4);
	test(sorter, 2,1,4,3);
	test(sorter, 2,3,1,4);
	test(sorter, 2,3,4,1);
	test(sorter, 2,4,1,3);
	test(sorter, 2,4,3,1);

	test(sorter, 3,1,2,4);
	test(sorter, 3,1,4,2);
	test(sorter, 3,2,1,4);
	test(sorter, 3,2,4,1);
	test(sorter, 3,4,1,2);
	test(sorter, 3,4,2,1);

	test(sorter, 4,1,2,3);
	test(sorter, 4,1,3,2);
	test(sorter, 4,2,1,3);
	test(sorter, 4,2,3,1);
	test(sorter, 4,3,1,2);
	test(sorter, 4,3,2,1);

	// duplicates (to test stability / equality handling)
	test(sorter, 1,1);
	test(sorter, 2,2);

	test(sorter, 1,1,2);
	test(sorter, 1,2,1);
	test(sorter, 2,1,1);

	test(sorter, 1,2,2);
	test(sorter, 2,1,2);
	test(sorter, 2,2,1);

	test(sorter, 1,1,2,2);
	test(sorter, 1,2,1,2);
	test(sorter, 1,2,2,1);
	test(sorter, 2,1,1,2);
	test(sorter, 2,1,2,1);
	test(sorter, 2,2,1,1);

	// negatives / zeros (if your sorter supports them)
	test(sorter, 0,1);
	test(sorter, 1,0);

	test(sorter, -1,0,1);
	test(sorter, -1,1,0);
	test(sorter, 0,-1,1);
	test(sorter, 0,1,-1);
	test(sorter, 1,-1,0);
	test(sorter, 1,0,-1);
}

void run_edge_length_tests(sort_fp sorter) {
	// L = 0 (empty array)
	{
		int dummy;
		int * arr = &dummy;
		print_arr(arr, 0);
		printf("\t");
		size_t swaps = sorter(arr, 0, sizeof(int), cmp_int, int_swap);
		printf("\t(%zu swaps)\n", swaps);
		assert(check_sorted(arr, 0));
	}

	// L = 1
	test(sorter, 5);
	test(sorter, -3);
	test(sorter, 0);
}

void run_more_duplicate_tests(sort_fp sorter) {
	// all-equal arrays
	test(sorter, 1,1,1);
	test(sorter, 2,2,2);
	test(sorter, 1,1,1,1);
	test(sorter, 2,2,2,2);

	// three-of-a-kind plus one odd element out, all positions of the odd one
	test(sorter, 2,1,1,1);
	test(sorter, 1,2,1,1);
	test(sorter, 1,1,2,1);
	test(sorter, 1,1,1,2);

	test(sorter, 1,2,2,2);
	test(sorter, 2,1,2,2);
	test(sorter, 2,2,1,2);
	test(sorter, 2,2,2,1);

	// three distinct values, each duplicated
	test(sorter, 1,2,3,1,2,3);
	test(sorter, 3,2,1,3,2,1);
	test(sorter, 1,1,2,2,3,3);
	test(sorter, 3,3,2,2,1,1);
}

// Fixed structural patterns (already-sorted, reverse-sorted, all-equal) at a
// size beyond what's practical to enumerate as permutations.
void run_pattern_tests(sort_fp sorter, const char * name, int N) {
	int * arr = malloc(sizeof(int) * N);

	for(int i = 0; i < N; ++i) arr[i] = i;
	size_t swaps = sorter(arr, (size_t)N, sizeof(int), cmp_int, int_swap);
	printf("%s already-sorted(N=%d):\t(%zu swaps)\n", name, N, swaps);
	assert(check_sorted(arr, N));

	for(int i = 0; i < N; ++i) arr[i] = N - i;
	swaps = sorter(arr, (size_t)N, sizeof(int), cmp_int, int_swap);
	printf("%s reverse-sorted(N=%d):\t(%zu swaps)\n", name, N, swaps);
	assert(check_sorted(arr, N));

	for(int i = 0; i < N; ++i) arr[i] = 7;
	swaps = sorter(arr, (size_t)N, sizeof(int), cmp_int, int_swap);
	printf("%s all-equal(N=%d):\t(%zu swaps)\n", name, N, swaps);
	assert(check_sorted(arr, N));

	free(arr);
}

// Randomized stress test. `int_swap()` prints on every call, which would
// flood stdout across many trials, so stdout is redirected to /dev/null for
// the duration of the trial loop (restored before reporting pass/fail)
// rather than touching int_swap()/rsort_swap() itself.
void run_stress_test(sort_fp sorter, const char * name, unsigned seed, int trials, int max_len, int value_range) {
	srand(seed);
	int * arr = malloc(sizeof(int) * max_len);
	int * arr_orig = malloc(sizeof(int) * max_len);

	fflush(stdout);
	int saved_fd = dup(STDOUT_FILENO);
	int devnull = open("/dev/null", O_WRONLY);
	dup2(devnull, STDOUT_FILENO);
	close(devnull);

	int fail_trial = -1, fail_len = 0;
	for(int t = 0; t < trials; ++t) {
		int len = rand() % (max_len + 1);
		for(int i = 0; i < len; ++i) {
			arr[i] = (rand() % (2 * value_range + 1)) - value_range;
			arr_orig[i] = arr[i];
		}
		sorter(arr, (size_t)len, sizeof(int), cmp_int, int_swap);
		if(!check_sorted(arr, len)) {
			fail_trial = t;
			fail_len = len;
			break;
		}
	}

	fflush(stdout);
	dup2(saved_fd, STDOUT_FILENO);
	close(saved_fd);

	if(fail_trial >= 0) {
		printf("STRESS FAIL (%s) trial %d len %d, input: ", name, fail_trial, fail_len);
		print_arr(arr_orig, fail_len);
		printf("\n");
		fflush(stdout);
		free(arr);
		free(arr_orig);
		assert(0 && "stress test produced an unsorted array");
		return;
	}

	free(arr);
	free(arr_orig);
	printf("%s stress test passed: %d trials, len<=%d, values in [-%d,%d]\n",
		name, trials, max_len, value_range, value_range);
}

void run_all_tests(sort_fp sorter, const char * name) {
	run_test(sorter);
	run_edge_length_tests(sorter);
	run_more_duplicate_tests(sorter);
	run_pattern_tests(sorter, name, 50);
	run_stress_test(sorter, name, 42, 2000, 200, 5);          // small value range: forces lots of duplicates
	run_stress_test(sorter, name, 1337, 500, 300, 1000000);   // large value range: mostly-unique values, bigger N
}

typedef struct {
	sort_fp fn;
	const char * name;
} sorter_entry;

static const sorter_entry all_sorters[] = {
	{ rsort2,         "rsort2" },
	{ rsort2_ver2,    "rsort2_ver2" },
	{ quicksort,      "quicksort" },
	{ insertion_sort, "insertion_sort" },
};
#define NUM_SORTERS (sizeof(all_sorters) / sizeof(all_sorters[0]))

bool check_sorted_generic(void * base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
	for(size_t i = 1; i < nmemb; ++i) {
		if(compare(rsort_elem(base, size, i), rsort_elem(base, size, i - 1)) < 0) return false;
	}

	return true;
}

// Comparison counter, wired in transparently by run_size_comparison so every
// benchmark reports comparisons alongside swaps — needed to tell whether
// runtime is actually swap-bound or comparison-bound.
static long long g_compare_count;
static int (*g_inner_compare)(const void *, const void *);

int counting_compare(const void * a, const void * b) {
	++g_compare_count;
	return g_inner_compare(a, b);
}

// Runs every sorter over the same `master` data (element type erased to
// void*/size/compare), timing each with rsort_swap (silent) rather than
// int_swap: a printf-per-swap would both flood stdout and dominate the
// measured time, making the comparison meaningless.
void run_size_comparison(void * master, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *), const char * label) {
	void * arr = malloc(size * nmemb);

	printf("  %s (element size=%zu, N=%zu):\n", label, size, nmemb);
	for(size_t k = 0; k < NUM_SORTERS; ++k) {
		memcpy(arr, master, size * nmemb);
		g_inner_compare = compare;
		g_compare_count = 0;
		clock_t t0 = clock();
		size_t swaps = all_sorters[k].fn(arr, nmemb, size, counting_compare, rsort_swap);
		clock_t t1 = clock();
		assert(check_sorted_generic(arr, nmemb, size, compare));
		printf("    %-15s %8.4fs  (%zu swaps, %lld compares)\n",
			all_sorters[k].name, (double)(t1 - t0) / CLOCKS_PER_SEC, swaps, g_compare_count);
	}

	free(arr);
}

// Runtime comparison on large, uniform-random int arrays.
void run_runtime_comparison(void) {
	int sizes[] = { 1000, 5000, 20000 };

	printf("\nRuntime comparison (uniform random ints, CPU time):\n");
	for(size_t si = 0; si < sizeof(sizes) / sizeof(sizes[0]); ++si) {
		int N = sizes[si];

		srand(4242);
		int * master = malloc(sizeof(int) * N);
		for(int i = 0; i < N; ++i) master[i] = rand();

		run_size_comparison(master, (size_t)N, sizeof(int), cmp_int, "int");

		free(master);
	}
}

// Comparator that only looks at the first 8 bytes, vs. cmp_recordN below
// which memcmp's the whole element. Both still SWAP the whole element
// (rsort_swap always moves `size` bytes) — this pair only varies how much
// gets read for comparison, to isolate comparison cost from swap cost.
int cmp_prefix8(const void * a, const void * b) {
	return memcmp(a, b, 8);
}

// Runtime comparison on a less trivial type: a struct wrapping a raw char
// buffer, ordered lexicographically (memcmp) by its bytes. One type +
// comparator pair is generated per buffer size via macro, since C has no
// generics and the sort algorithms need `sizeof(RecordN)` to actually equal
// each requested buffer size (not just a runtime byte count) for the
// benchmark to reflect real per-swap copy cost at that element size.
//
// Byte counts must be given as a single literal (not a `512 * 1024`
// expression): DEFINE_RECORD_TYPE pastes its argument onto `Record` via
// `##`, which only pastes with the first token of a multi-token argument.
#define DEFINE_RECORD_TYPE(N) \
	typedef struct { unsigned char buf[N]; } Record##N; \
	int cmp_record##N(const void * a, const void * b) { \
		return memcmp(a, b, sizeof(Record##N)); \
	} \
	void bench_record##N(size_t nmemb) { \
		Record##N * master = malloc(sizeof(Record##N) * nmemb); \
		srand(4242); \
		for(size_t i = 0; i < nmemb; ++i) { \
			for(size_t b = 0; b < sizeof(Record##N); ++b) master[i].buf[b] = (unsigned char)rand(); \
		} \
		run_size_comparison(master, nmemb, sizeof(Record##N), cmp_record##N, "struct{char buf[" #N "]} full-compare"); \
		run_size_comparison(master, nmemb, sizeof(Record##N), cmp_prefix8, "struct{char buf[" #N "]} prefix8-compare"); \
		free(master); \
	}

DEFINE_RECORD_TYPE(128)
DEFINE_RECORD_TYPE(256)
DEFINE_RECORD_TYPE(512)
DEFINE_RECORD_TYPE(8192)    // 8KB
DEFINE_RECORD_TYPE(32768)   // 32KB
DEFINE_RECORD_TYPE(65536)   // 64KB

void run_record_benchmarks(void) {
	printf("\nRuntime comparison (struct { char buf[N]; }, full-buffer vs first-8-bytes compare):\n");
	bench_record128(200);
	bench_record256(200);
	bench_record512(200);
	bench_record8192(200);
	bench_record32768(200);
	bench_record65536(200);
}

// Worst case for memcmp: every element shares the same (N-8)-byte prefix,
// differing only in its last 8 bytes. A full-buffer memcmp can no longer
// short-circuit early — it has to scan the entire shared prefix before
// reaching the bytes that actually differ — while a comparator that reads
// straight from the last 8 bytes finds the same answer immediately. Both
// comparators agree on order (the true key lives in those last 8 bytes),
// so this is still a fair "same sort, different comparison cost" test —
// unlike a shared *suffix*, which would make a cheap prefix-only comparator
// see ties and change what gets sorted.
#define DEFINE_RECORD_WORST_CASE(N) \
	int cmp_record##N##_suffix8(const void * a, const void * b) { \
		return memcmp((const unsigned char *)a + sizeof(Record##N) - 8, \
		              (const unsigned char *)b + sizeof(Record##N) - 8, 8); \
	} \
	void bench_record##N##_sharedprefix(size_t nmemb) { \
		Record##N * master = malloc(sizeof(Record##N) * nmemb); \
		unsigned char shared[sizeof(Record##N)]; \
		srand(1234); \
		for(size_t b = 0; b < sizeof(Record##N); ++b) shared[b] = (unsigned char)rand(); \
		for(size_t i = 0; i < nmemb; ++i) { \
			memcpy(master[i].buf, shared, sizeof(Record##N)); \
			for(size_t b = sizeof(Record##N) - 8; b < sizeof(Record##N); ++b) master[i].buf[b] = (unsigned char)rand(); \
		} \
		run_size_comparison(master, nmemb, sizeof(Record##N), cmp_record##N, "struct{char buf[" #N "]} SHARED-PREFIX full-compare"); \
		run_size_comparison(master, nmemb, sizeof(Record##N), cmp_record##N##_suffix8, "struct{char buf[" #N "]} SHARED-PREFIX suffix8-compare"); \
		free(master); \
	}

DEFINE_RECORD_WORST_CASE(128)
DEFINE_RECORD_WORST_CASE(256)
DEFINE_RECORD_WORST_CASE(512)
DEFINE_RECORD_WORST_CASE(8192)
DEFINE_RECORD_WORST_CASE(32768)
DEFINE_RECORD_WORST_CASE(65536)

void run_shared_prefix_benchmarks(void) {
	printf("\nRuntime comparison (struct { char buf[N]; }, SHARED PREFIX -- memcmp worst case):\n");
	bench_record128_sharedprefix(200);
	bench_record256_sharedprefix(200);
	bench_record512_sharedprefix(200);
	bench_record8192_sharedprefix(200);
	bench_record32768_sharedprefix(200);
	bench_record65536_sharedprefix(200);
}

int main() {
	// rsort1 has no generic counterpart in rsort.h: its recursion blows up
	// exponentially with N, so it was intentionally left out of the header.

	printf("Sort 2\n");
	run_all_tests(rsort2, "Sort2");

	printf("------------------------\n");
	printf("Sort 2 ver 2\n");
	run_all_tests(rsort2_ver2, "Sort2ver2");

	printf("------------------------\n");
	printf("Quicksort\n");
	run_all_tests(quicksort, "Quicksort");

	printf("------------------------\n");
	printf("Insertion sort\n");
	run_all_tests(insertion_sort, "InsertionSort");

	printf("\nOK!\n");

	run_runtime_comparison();
	run_record_benchmarks();
	run_shared_prefix_benchmarks();
}
