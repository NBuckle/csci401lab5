#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100

int array[SIZE];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;




// Separate main array into two halves
void merge(int arr[], int start, int mid, int end) {
    int i, j, k;
    int n1 = mid - start + 1;
    int n2 = end - mid;

    int first_Array[n1], second_array[n2];  

    for (i = 0; i < n1; i++)
        first_Array[i] = arr[start + i];
    for (j = 0; j < n2; j++)
        second_array[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = start;

    while (i < n1 && j < n2) {
        if (first_Array[i] <= second_array[j]) {
            arr[k] = first_Array[i];
            i++;
        } else {
            arr[k] = second_array[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = first_Array[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = second_array[j];
        j++;
        k++;
    }
}

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.
void* mergeSort(void* args) {
  StartEndIndexes* sei = (StartEndIndexes*)args; // sei to shorten start end indexes
  int start = sei->start;
  int end = sei->end;

  if (start < end) {
      int mid = start + (end - start) / 2;
      pthread_t leftThread, rightThread; // the two threads for the left and right halves

      StartEndIndexes leftArgs, rightArgs;
      leftArgs.start = start;
      leftArgs.end = mid;       //set boundaries for them
      rightArgs.start = mid + 1;
      rightArgs.end = end;

      pthread_create(&leftThread, NULL, mergeSort, (void*)&leftArgs);
      pthread_create(&rightThread, NULL, mergeSort, (void*)&rightArgs);

      // join after the threads finish
      pthread_join(leftThread, NULL);
      pthread_join(rightThread, NULL);

      merge(array, start, mid, end);
  }

  return NULL;
}

int main() {
    srand(time(0));
    StartEndIndexes sei;
    sei.start = 0;
    sei.end = SIZE - 1;
    
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
    
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    
    // 3. Create a 2 threads for merge sort.
    pthread_t thread;
    pthread_create(&thread, NULL, mergeSort, (void*)&sei);

    // 4. Wait for mergesort to finish.
    pthread_join(thread, NULL);

    // 5. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);
}
