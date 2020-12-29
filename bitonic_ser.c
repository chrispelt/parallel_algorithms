#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <math.h>

#include "mpi.h"

#define N 4096  // must be a power of 2

#define ascending 1
#define descending 0


/* This function compares two elements of the sequence and
 * if they follows the given direction they swapped
 */
void compSwap(int arr[N], int i, int j, int dir){
	
	int tmp;

	if(dir == (arr[i] > arr[j])){
		tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	}


}


/* This function sorts a bitonic sequence in given order,
 * defined by dir. low is the starting index and cnt is
 * the number of elements
 */
void bitonicMerge(int arr[N], int low, int cnt, int dir){

	if(cnt>1){
	
		int mid = cnt/2;
		for(int i=low; i<low+mid; i++){
		
			compSwap(arr, i, i+mid, dir);
	
		}

		bitonicMerge(arr, low, mid, dir);
		bitonicMerge(arr, low+mid, mid, dir);

	}

}


/* This function produces a bitonic sequence recursively,
 * sorting its two halves in opposite direction, and then
 * call bitonicMerge to sort this sequence.
 */
void doBitonic(int arr[N], int low, int cnt, int dir){

	if(cnt>1){
		
		int mid = cnt/2;

		// sort first half in ascending order
		doBitonic(arr, low, mid, ascending);

		// sort second half in descending order
		doBitonic(arr, low+mid, mid, descending);

		// merge the whole sequence in given direction
		bitonicMerge(arr, low, cnt, dir);

	}

}


/* This is a wrapper function for bitonic sort
 */
void bitonicSort(int arr[N], int cnt, int dir){
	doBitonic(arr, 0, cnt, dir);
}


int main(){
	clock_t timer_start;
	clock_t timer_end;
	double exec_time = 0.0;
	srand(time(0));
	int a[N];

	for(int j=0; j<N; j++){
		a[j] = rand() % 100;
	}

	//int a[] = {10, 2, 65, 9, 8, 5, 4, 32};
	
	timer_start = clock();
	bitonicSort(a, N, ascending);
	timer_end = clock();
	
	exec_time += (double)(timer_end - timer_start) / CLOCKS_PER_SEC;

/*
	for(int i=0; i<N; i++){
		printf("%d ", a[i]);
	}
	printf("\n");
*/
	printf("Execution time: %f sec\n", exec_time);

	return 0;

}
