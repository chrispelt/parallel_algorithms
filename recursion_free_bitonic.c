#include <stdio.h>    
#include <time.h>    
#include <stdlib.h>
#include <stdbool.h>

void swap(int *a, int *b) 
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
} 
  
void print(int arr[], int n){
    for(int i = 0; i < n; i++)
      printf("%d ", arr[i]);
      printf("\n\n");
}

void bitonic(int arr[], int n, bool asc){
    if (asc) {
	    for (int k = 2; k <= n; k = 2*k){
            for (int j = k/2; j > 0; j /= 2){
                for (int i = 0; i < n; i++){ 
                    int l = i^j;
                    if (l > i){
                        if (  (!(i&k) && (arr[i] > arr[l])) || (  (i&k) && (arr[i] < arr[l])) ){
                            swap(&arr[i], &arr[l]);
                        }  
	                }
	            }
	        }
	        //print(arr,n);
	    }
	}
	else {
	    for (int k = 2; k <= n; k = 2*k){
            for (int j = k/2; j > 0; j /= 2){
                for (int i = 0; i < n; i++){ 
                    int l = i^j;
                    if (l > i){
                        if (  (!(i&k) && (arr[i] < arr[l])) || (  (i&k) && (arr[i] > arr[l])) ){
                            swap(&arr[i], &arr[l]);
                        }  
	                }
	            }
	        }
	        //print(arr,n);
	    }
	}
}
//Must be a power of 2 (for now)
const int SIZE=16;

int main(int argc, char * argv[]) {
    int my_array[SIZE];
    srand(time(NULL));
    
    for (int i = 0; i < SIZE; i++) {
        my_array[i] = rand() % 100;
    }
    print(my_array, SIZE);

    bitonic(my_array, SIZE, 1);
    print(my_array, SIZE);
}
