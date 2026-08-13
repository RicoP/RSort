#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

int RSort1(int * arr, int L);
int RSort2(int * arr, int L);
int RSort2_ver2(int * arr, int L);

typedef int (*sort_fp)(int * arr, int N);

void swap(int * arr, int ia, int ib) {
	printf("[swap %d,%d]", arr[ia], arr[ib]);
	int tmp = arr[ia];
	arr[ia] = arr[ib];
	arr[ib] = tmp;
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
	//printf("\t-->\t");
	printf("\t");
	int swaps = sorter(arr, num);
	//print_arr(arr, num);
	printf("\t(%d swaps)\n", swaps);
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

int main() {
	printf("Sort 1\n");
	run_test(RSort1);

	printf("------------------------\n");
	printf("Sort 2\n");
	run_test(RSort2);
	printf("------------------------\n");
	printf("Sort 2 ver 2\n");
	run_test(RSort2_ver2);
	printf("\nOK!\n");
}

/*
void RSort(int * arr, int L) {
	if(L <= 1) return;
	print_arr(arr, L);
	printf(" -> ");

	for(int i = 1; i != L; ++i) {
		if(arr[i] > arr[0]) {
			RSort(arr + i, L - i);
			if(arr[i] > arr[0]) {
				return;
			}
			swap(arr, 0, i);
			i = 0;
			continue;
		}
		swap(arr, 0, i);
	}
	
	RSort(arr + 1, L - 2);
}
*/





// ALGO 2!
#if 0
if(N < 2) return;

int p,s,x,n;
//??? we might can replace all occurences of x with n
n = N;

the_beginning:
s = 0;

state0:
if(n < 2) return;
for(p = s + 1; p != n; ++p) {
	if(arr[p] > arr[s]) {
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
swap(p, n);
goto the_beginning;

state1:
for(; x != n; ++x) {
	if(arr[p] < arr[x]) {
		p = x;
	}
}

do_the_swapping:
if(p != x-1) swap(p, x-1);
n--;
// ??? if(p != s && arr[p] < arr[s]) swap(p, s) ???
if(s == n) { goto the_beginning; }
goto state0;
#endif


int RSort2(int * arr, int N) {
	int swaps = 0;
	int p,s,x,n;

	//??? we might can replace all occurences of x with n
	n = N;

	the_beginning:
	s = 0;

	state0:
	if(n < 2) return swaps;
	for(p = s + 1; p != n; ++p) {
		if(arr[p] > arr[s]) {
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
	swap(arr, p, n);
	swaps++;
	goto the_beginning;

	state1:
	for(; x != n; ++x) {
		if(arr[p] < arr[x]) {
			p = x;
		}
	}

	do_the_swapping:
	if(p != x-1) { swap(arr, p, x-1); swaps++; };
	n--;
	// ??? if(p != s && arr[p] < arr[s]) swap(p, s) ???
	if(s == n) { goto the_beginning; }
	goto state0;
}


int RSort2_ver2(int * arr, int N) {
	int swaps = 0;
	int p,s,x,n;

	//??? we might can replace all occurences of x with n
	n = N;

	the_beginning:
	s = 0;

	state0:
	if(n < 2) return swaps;
	for(p = s + 1; p != n; ++p) {
		if(arr[p] >= arr[s]) {
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
	swap(arr, p, n);
	swaps++;
	goto the_beginning;

	state1:
	for(; x != n; ++x) {
		if(arr[p] <= arr[x]) {
			p = x;
		}
	}

	do_the_swapping:
	if(p != x-1) { swap(arr, p, x-1); swaps++; };
	n--;
	// ??? if(p != s && arr[p] < arr[s]) swap(p, s) ???
	if(s == n) { goto the_beginning; }
	goto state0;
}












int RSort1_rec(int * arr, int * arr0, int s, int L) {
	if(L <= 1) return 0;
	int swaps = 0;
	//print_arr(arr, L);

	for(int i = 1; i != L; ++i) {
		swaps += RSort1_rec(arr + i, arr0, i, L - i);
		if(arr[i] < arr[0]) { /*printf("[swap %d,%d]", arr0[s], arr0[s + i]);*/ swap(arr, 0, i); --i; swaps++; }
	}
	return swaps;
}

int RSort1(int * arr, int L) {
	RSort1_rec(arr, arr, 0, L);
}
