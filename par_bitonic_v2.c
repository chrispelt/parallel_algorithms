#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <math.h>

#include "mpi.h"

#define N 4096  // must be a power of 2

#define ascending 1
#define descending 0


void print(int *arr, int n){
    for(int i = 0; i < n; i++)
      printf("%d ", arr[i]);
      printf("\n\n");
}

int CompareAsc(const void * a, const void * b) {
    return ( * (int *)a - * (int *)b );
}

int CompareDesc(const void * a, const void * b) {
    return ( * (int *)b - * (int *)a );
}

/* This function compares two elements of the sequence and
 * if they follows the given direction they swapped
 */
void compSwap(int *arr, int i, int j, int dir){
	
	int tmp;

	if(dir == (arr[i] > arr[j])){
		tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	}
}

/* This function sorts a bitonic sequence in given order,
 * defined by dir. low is the starting index and cnt is
 * the number of elements*/
void bitonicMerge(int* arr, int low, int cnt, int dir){

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
 * call bitonicMerge to sort this sequence.*/
void doBitonic(int *arr, int low, int cnt, int dir){
	if(cnt>1){	
		int mid = cnt/2;
		doBitonic(arr, low, mid, ascending);
		doBitonic(arr, low+mid, mid, descending);

		bitonicMerge(arr, low, cnt, dir);
	}
}

// This is a wrapper function for bitonic sort
void bitonicSort(int *arr, int cnt, int dir){
	doBitonic(arr, 0, cnt, dir);
}

int main(int argc, char **argv){
	
	int rank, size, sendcnt, rcvcnt, levels;
	const int src = 0;
	double timer_start;
	double timer_end;
	MPI_Status stat;	

	srand(time(NULL));
	int a[N];

	for(int j=0; j<N; j++){
		a[j] = rand() % 100;
	}
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	sendcnt = N / size;
	rcvcnt = N / size;
	levels = (int)(log2(size));
	
	int* localA = (int*) malloc(sendcnt * sizeof(int));;
    //int localA[N];
	if (rank == src){
	    timer_start = MPI_Wtime();
	}
	
	MPI_Scatter(a, sendcnt, MPI_INT, localA, rcvcnt, MPI_INT, src, MPI_COMM_WORLD);

    if (rank%2){
        qsort(localA, sendcnt, sizeof(int), CompareDesc);
    }
    else {
        qsort(localA, sendcnt, sizeof(int), CompareAsc);
    }    


	for (int i = 0; i<levels; i++){
	    if (rank&(1<<i) && ((rank>>i)<<i)==rank){
	    //printf("up: i=%d rank=%d\n",i,rank);
	        bitonicMerge(localA, 0, sendcnt, descending);
	        //printf("PC:%d sending to %d\n",rank,rank-(1<<i));
	        MPI_Send(localA, sendcnt, MPI_INT, rank-(1<<i), 1, MPI_COMM_WORLD);
	    }
	    else if (!(rank&(((1<<(i+1))-1)))){
	    //printf("down: i=%d rank=%d\n",i,rank);
	        bitonicMerge(localA, 0, sendcnt, ascending);
	        int* localB = (int*) realloc(localA, (2*sendcnt*sizeof(int)));
	        localA = localB;
	        //print(localA,2*sendcnt);
	        if (rank+(1<<i)<size){
	            //printf("i:%d PC:%d receiving from %d\n",i,rank,rank+(1<<i));
	            MPI_Recv(localA+sendcnt, sendcnt, MPI_INT, rank+(1<<i), 1, MPI_COMM_WORLD, &stat);
                //print(localA,2*sendcnt);
            }
            
	    }
	    sendcnt *=2;
	}
	
	//free(localA);

	if(rank == 0){
	    bitonicMerge(localA, 0, sendcnt, ascending);
	    timer_end = MPI_Wtime();
		//for(int i=0; i<N; i++){
		//	printf("%d ", localA[i]);
		//}
		printf("\n");
		printf("Time taken %f\n", timer_end - timer_start);
	}

	MPI_Finalize();

	return 0;

}
