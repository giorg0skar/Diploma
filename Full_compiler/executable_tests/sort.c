#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    int j,temp;
    for(j = low; j <= high-1; j++) {
        if (arr[j] <= pivot) {
            i++;
            // temp = arr[i];
            // arr[i] = arr[j];
            // arr[j] = temp;
            swap(&arr[i], &arr[j]);
        }
    }
    // arr[i+1] = temp;
    // arr[i+1] = arr[high];
    // arr[high] = temp;
    swap(&arr[i+1], &arr[high]);
    return i+1;
}

void quicksort(int arr[], int low, int high) {
    if (low < high)
    {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partition(arr, low, high);

        quicksort(arr, low, pi - 1);  // Before pi
        quicksort(arr, pi + 1, high); // After pi
    }
}

int main() {
    int N,i;
    scanf("%d", &N);
    int *a = (int *) malloc(N*sizeof(int));
    for(i = 0; i < N; i++) {
        int c;
        scanf("%d", &c);
        a[i] = c;
    }
    quicksort(a, 0, N-1);
    printf("%d", a[0]);
    for(i=1; i<N; i++) printf(", %d", a[i]);
    printf("\n");
    return 0;
}