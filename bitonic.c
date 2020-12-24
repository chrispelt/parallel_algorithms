#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <math.h>

#include "mpi.h"

#define N 32  // must be a power of 2

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


int main(int argc, char **argv){
	
	int rank, size, sendcnt, rcvcnt;
	int src = 0;
	MPI_Status stat;	

	srand(time(0));
	int a[N];

	for(int j=0; j<N; j++){
		a[j] = rand() % 100;
	}
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

        sendcnt	= N / size;
	rcvcnt = N / size;

	int localA[sendcnt];

	MPI_Scatter(a, sendcnt, MPI_INT, localA, rcvcnt, MPI_INT, src, MPI_COMM_WORLD);

	if(!(rank % 2)){
		int localB[2*sendcnt];
		bitonicSort(localA, sendcnt, ascending);
		MPI_Recv(localB, sendcnt, MPI_INT, rank+1, 1, MPI_COMM_WORLD, &stat);
		for(int m=sendcnt; m<2*sendcnt; m++){
                        localB[m] = localA[m-sendcnt];
                }
                if(!(rank % 4)){
                     	bitonicMerge(localB, 0, 2*sendcnt, ascending);
                }else{
                	bitonicMerge(localB, 0, 2*sendcnt, descending);
                }
                
                for(int i=0; i<2*sendcnt; i++){
			printf("%d ", localB[i]);
		}
		printf("\n");
		
		if(rank!=0){	
			MPI_Send(localB, 2*sendcnt, MPI_INT, src, 1, MPI_COMM_WORLD);
		}
	}else{
		bitonicSort(localA, sendcnt, descending);
		MPI_Send(localA, sendcnt, MPI_INT, rank-1, 1, MPI_COMM_WORLD);
	}
		
	//bitonicSort(localA, sendcnt, ascending);

//	if(rank == 0){
//		for (int k=1; k<size; k++){
//			MPI_Recv(a, sendcnt, MPI_INT, k, 1, MPI_COMM_WORLD, &stat);
//			bitonicMerge(a, 0, N, ascending);
//		}
//	}else{
//		MPI_Send(localA, sendcnt, MPI_INT, src, 1, MPI_COMM_WORLD);	
//	}
	
//	MPI_Gather(localA, sendcnt, MPI_INT, a, rcvcnt, MPI_INT, src, MPI_COMM_WORLD);
	

	if(rank == 0){
		MPI_Recv(a, 2*sendcnt, MPI_INT, 2, 1, MPI_COMM_WORLD, &stat);
//		MPI_Recv(a, 2*sendcnt, MPI_INT, 3, 1, MPI_COMM_WORLD, &stat);
//		bitonicMerge(a, 0, N/2, ascending);
//		bitonicMerge(a, N/2, N/2, descending);
//		bitonicMerge(a, 0, N, ascending);
		for(int i=0; i<N; i++){
			printf("%d ", a[i]);
		}
		printf("\n");
	}

	MPI_Finalize();

	return 0;

}
