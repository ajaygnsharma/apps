/*
 * main.c
 *
 *  Created on: Jul 25, 2017
 *      Author: ajay.sharma
 */
#include <stdio.h>

// Returns maximum repeating element in arr[0..n-1].
// The array elements are in range from 0 to k-1
int maxRepeating(int* arr, int n, int k)
{
    int i;
    // Iterate though input array, for every element
    // arr[i], increment arr[arr[i]%k] by k
    for (i = 0; i< n; i++){
        arr[arr[i]%k] += k;
    }

    // Find index of the maximum repeating element
    int max = arr[0], result = 0;
    for (i = 1; i < n; i++)
    {
        printf("arr[%d]=%d\n",i,arr[i]);
        if (arr[i] > max)
        {
            max = arr[i];
            result = i;
        }
    }

    /* Uncomment this code to get the original array back
       for (int i = 0; i< n; i++)
          arr[i] = arr[i]%k; */

    // Return index of the maximum element
    return result;
}

// Driver program to test above function
int main()
{
    int arr[] = {2, 3, 3, 5, 3, 4, 1, 7};
    int n = sizeof(arr)/sizeof(arr[0]);
    int k = 8;

    printf("The maximum repeating number is %d", maxRepeating(arr, n, k));

    int arr2[] = {2, 2, 34, 100, 101, 102, 2, 2};
    printf("The maximum repeating number is %d", maxRepeating(arr2, n, k));


    return 0;
}
