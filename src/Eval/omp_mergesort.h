#ifndef OMP_MERGESORT_H_QHRJTEP9
#define OMP_MERGESORT_H_QHRJTEP9

/*
 OpenMP recursive merge sort
 Copyright (C) 2011  Atanas Radenski
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
 Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <omp.h>
#include <vector>
#include <algorithm>

const int Small = 32;

template <typename T, typename Cmp>
void merge(T arr[], int size, T temp[], Cmp cmp);
template <typename T, typename Cmp>
void insertion_sort(T arr[], int size, Cmp cmp);
template <typename T, typename Cmp>
void mergesort_serial(T a[], int size, T temp[], Cmp cmp);
template <typename T, typename Cmp>
void mergesort_parallel_omp(T a[], int size, T temp[], int threads, Cmp cmp);

template <typename T, typename Cmp>
void mergesort(T a[], int size, T temp[], int threads, Cmp cmp) {
    // Enable nested parallelism, if available
    omp_set_nested(1);
    // Parallel mergesort
    mergesort_parallel_omp<T, Cmp>(a, size, temp, threads, cmp);
}

// OpenMP merge sort with given number of threads
template <typename T, typename Cmp>
void mergesort_parallel_omp(T a[], int size, T temp[], int threads,
                            Cmp cmp) {
    if (threads == 1) {
        //        printf("Thread %d begins serial merge sort\n",
        //        omp_get_thread_num());
        mergesort_serial<T, Cmp>(a, size, temp, cmp);
    } else if (threads > 1) {
// TODO remove num_threads?
#pragma omp parallel sections num_threads(2)
        {
//		        printf("Thread %d begins recursive section\n",
// omp_get_thread_num());
#pragma omp section
            { // printf("Thread %d begins recursive call\n",
                // omp_get_thread_num());
                mergesort_parallel_omp<T, Cmp>(a, size / 2, temp, threads / 2,
                                               cmp);
            }
#pragma omp section
            { // printf("Thread %d begins recursive call\n",
                // omp_get_thread_num());
                mergesort_parallel_omp<T, Cmp>(a + size / 2, size - size / 2,
                                               temp + size / 2,
                                               threads - threads / 2, cmp);
            }
            // The above use of temp + size/2 is an essential change from the
            // serial version
        }
        // Thread allocation is implementation dependent
        // Some threads can execute multiple sections while others are idle
        // Merge the two sorted sub-arrays through temp
        merge<T, Cmp>(a, size, temp, cmp);
    } else {
        assert(false);
        return;
    }
}

template <typename T, typename Cmp>
void mergesort_serial(T a[], int size, T temp[], Cmp cmp) {
    // Switch to insertion sort for small arrays
    if (size < Small) {
        insertion_sort<T, Cmp>(a, size, cmp);
        return;
    }
    mergesort_serial<T, Cmp>(a, size / 2, temp, cmp);
    mergesort_serial<T, Cmp>(a + size / 2, size - size / 2, temp, cmp);
    // The above call will not work properly in an OpenMP program
    // Merge the two sorted subarrays into a temp array
    merge<T, Cmp>(a, size, temp, cmp);
}

template <typename T, typename Cmp>
void merge(T arr[], int size, T temp[], Cmp cmp) {
    int i1 = 0;
    int i2 = size / 2;
    int tempi = 0;
    while (i1 < size / 2 && i2 < size) {
        if (cmp(arr[i1], arr[i2])) {
            temp[tempi] = arr[i1];
            i1++;
        } else {
            temp[tempi] = arr[i2];
            i2++;
        }
        tempi++;
    }
    while (i1 < size / 2) {
        temp[tempi] = arr[i1];
        i1++;
        tempi++;
    }
    while (i2 < size) {
        temp[tempi] = arr[i2];
        i2++;
        tempi++;
    }
    // Copy sorted temp array into main array
    std::copy(temp, temp + size, arr);
}

template <typename T, typename Cmp>
void insertion_sort(T arr[], int size, Cmp cmp) {
    int i;
    for (i = 0; i < size; i++) {
        int j;
        auto v = arr[i];
        for (j = i - 1; j >= 0; j--) {
            if (cmp(arr[j], v))
                break;
            arr[j + 1] = arr[j];
        }
        arr[j + 1] = v;
    }
}


#endif /* end of include guard: OMP_MERGESORT_H_QHRJTEP9 */
