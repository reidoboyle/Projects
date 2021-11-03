"""https://www.geeksforgeeks.org/python-program-for-quicksort/"""
# Citation for quicksort implementation in python ^
"""https://www.geeksforgeeks.org/python-program-for-insertion-sort/"""
# Citation for insertion sort implementation in python ^
"""https://machinelearningmastery.com/curve-fitting-with-python/"""
# Citation for polynomial curve fitting model used to make lines
"""https://www.geeksforgeeks.org/advanced-quick-sort-hybrid-algorithm/"""
# Citation for the hybrid sort algorithm

import random
import matplotlib.pyplot as plt
import copy
import time
from numpy import arange
from scipy.optimize import curve_fit
# Python implementation of the above approach

def objective(x,a,b,c):
    return a * x + b* x**2 + c
def quickSort(arr, low, high):
    if len(arr) == 1:
        return arr
    if low < high:
        # pi is partitioning index, arr[p] is now
        # at right place
        pi = partition(arr, low, high)

        # Separately sort elements before
        # partition and after partition
        quickSort(arr, low, pi - 1)
        quickSort(arr, pi + 1, high)
# Function to do insertion sort
def insertionSort(arr):
    # Traverse through 1 to len(arr)
    for i in range(1, len(arr)):

        key = arr[i]

        # Move elements of arr[0..i-1], that are
        # greater than key, to one position ahead
        # of their current position
        j = i - 1
        while j >= 0 and key < arr[j]:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = key

# Function to perform the insertion sort
def insertion_sort(arr, low, n):
    for i in range(low + 1, n + 1):
        val = arr[i]
        j = i
        while j > low and arr[j - 1] > val:
            arr[j] = arr[j - 1]
            j -= 1
        arr[j] = val


# The following two functions are used
# to perform quicksort on the array.

# Partition function for quicksort
def partition(arr, low, high):
    pivot = arr[high]
    i = j = low
    for i in range(low, high):
        if arr[i] < pivot:
            arr[i], arr[j] = arr[j], arr[i]
            j += 1
    arr[j], arr[high] = arr[high], arr[j]
    return j


# Function to call the partition function
# and perform quick sort on the array
def quick_sort(arr, low, high):
    if low < high:
        pivot = partition(arr, low, high)
        quick_sort(arr, low, pivot - 1)
        quick_sort(arr, pivot + 1, high)
        return arr


# Hybrid function -> Quick + Insertion sort
def hybrid_quick_sort(arr, low, high,thresh):
    while low < high:

        # If the size of the array is less
        # than threshold apply insertion sort
        # and stop recursion
        if high - low + 1 < thresh:
            insertion_sort(arr, low, high)
            break

        else:
            pivot = partition(arr, low, high)

            # Optimised quicksort which works on
            # the smaller arrays first

            # If the left side of the pivot
            # is less than right, sort left part
            # and move to the right part of the array
            if pivot - low < high - pivot:
                hybrid_quick_sort(arr, low, pivot - 1,thresh)
                low = pivot + 1
            else:
                # If the right side of pivot is less
                # than left, sort right side and
                # move to the left side
                hybrid_quick_sort(arr, pivot + 1, high,thresh)
                high = pivot - 1
#uncomment below to find best K value to use
"""
ranges = [50,100,200,300,400,500,1000,2000,3000,4000,5000,10000,20000,30000,40000,50000]
lin = 1
K_vals = [2, 5, 7, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 35, 40, 45, 50,
          60, 70, 80]

bests = {}
for k in K_vals:
    bests[k] = 0

for i in ranges:
    #print(i)
    test_list = [random.randint(0,1000) for _ in range(0,i)]
    N = len(test_list)
    # Quicksort time computation (ms)
    quick_sort_avg = 0.0
    K_vals = [2,5,7,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,35,40,45,50,60,70,80]
    K_times = []
    for K in K_vals:
        quick_sort_avg = 0.0
        for j in range(10):
            # use same 100 random seeds
            random.seed(j)
            # generate list from that seed
            test_list = [random.randint(0, 1000) for _ in range(0, i)]
            #time it
            start_time = time.perf_counter()
            copy_list_1 = copy.deepcopy(test_list)
            hybrid_quick_sort(copy_list_1,0,N-1,K)
            quick_sort_time = float(time.perf_counter()) - float(start_time)/float(lin)
            # add time to total
            quick_sort_avg += quick_sort_time
        # compute an average
        quick_sort_avg = float(quick_sort_avg)#/float(10.0)
        K_times.append(quick_sort_avg)
    idx = 0
    min_ = 100
    for c in range(0,len(K_times)):
        if K_times[c] < min_:
            min_ = K_times[c]
            idx = c
    bests[K_vals[idx]] += 1
    print(K_vals[idx])
    #print(K_times)

fig1, ax1 = plt.subplots()
lbs = list(bests.keys())
lbs = [str(l) for l in lbs]
print(list(bests.values()))
ax1.pie(list(bests.values()),labels=lbs,autopct='%1.1f%%',startangle=90)
plt.show()
"""

# Now test with K
QS_times = []
IS_times = []
hybrid_times = []
K = 12
lin = 1
i = 1
while i < 50:
    #print(i)
    #initial lists
    test_list = [random.randint(0,1000) for _ in range(0,i)]
    copy_list = copy.deepcopy(test_list)
    N = len(test_list)

    # Quicksort time computation (ms)
    quick_sort_avg = 0.0
    for j in range(100):
        # use same 100 random seeds
        random.seed(j)
        # generate list from that seed
        test_list = [random.randint(0, 1000) for _ in range(0, i)]
        #time it
        start_time = time.perf_counter()
        copy_list_1 = copy.deepcopy(test_list)
        quickSort(copy_list_1,0,N-1)
        quick_sort_time = float(time.perf_counter()) - float(start_time)/float(lin)
        # add time to total
        quick_sort_avg += quick_sort_time
    # compute an average
    quick_sort_avg = float(quick_sort_avg)/float(100.0)

    # compute total time for quicksort and add it to list of times
    #quick_sort_time = float(float(end_time) - float(start_time))
    QS_times.append(quick_sort_avg)

    # Insertion sort time computation (ms)
    insert_sort_avg = 0.0
    for k in range(100):
        # use same seeds as quicksort to generate same lists
        random.seed(k)
        # generate list from that seed
        test_list = [random.randint(0, 1000) for _ in range(0, i)]
        is_start_time = time.perf_counter()
        copy_list_2 = copy.deepcopy(test_list)
        insertionSort(copy_list_2)
        insertion_sort_time = float(time.perf_counter()) - float(is_start_time)/float(lin)
        # add to average
        insert_sort_avg += insertion_sort_time
    insert_sort_avg = float(insert_sort_avg)/float(100.0)
    IS_times.append(insert_sort_avg)

    hybrid_sort_avg = 0.0
    for j in range(100):
        # use same 100 random seeds
        random.seed(j)
        # generate list from that seed
        test_list = [random.randint(0, 1000) for _ in range(0, i)]
        # time it
        start_time = time.perf_counter()
        copy_list_1 = copy.deepcopy(test_list)
        hybrid_quick_sort(copy_list_1, 0, N - 1,K)
        hybrid_sort_time = float(time.perf_counter()) - float(start_time) / float(lin)
        # add time to total
        hybrid_sort_avg += hybrid_sort_time
    # compute an average
    hybrid_sort_time = float(hybrid_sort_avg) / float(100.0)
    hybrid_times.append(hybrid_sort_time)

    i += 1


X = [x for x in range(1,50)]

print(QS_times)
print(IS_times)
print(hybrid_times)
plt.scatter(X,QS_times,label="Quicksort Times")
plt.scatter(X,IS_times,label="Insertion Sort Times")
plt.scatter(X,hybrid_times,label="Hybrid Sort Times")

# now learn a polynomial regressor to best estimate the cross time
popt,_ = curve_fit(objective, X,IS_times)
a,b,c = popt
x_line = arange(1,50,1)
y_line = objective(x_line,a,b,c)
plt.plot(x_line,y_line,'--',color="red",label = "Learn Function for Insertion Sort Times")
# now do the same for QuickSort
popt,_ = curve_fit(objective,X,QS_times)
a,b,c = popt
x_line_2 = arange(1,50,1)
y_line_2 = objective(x_line,a,b,c)
plt.plot(x_line,y_line_2,'--',color="purple",label="Learned Function for QuickSort Times")

popt,_ = curve_fit(objective,X,hybrid_times)
a,b,c = popt
x_line_3 = arange(1,50,1)
y_line_3 = objective(x_line,a,b,c)
plt.plot(x_line,y_line_3,'--',color="pink",label="Learned Function for Hybrid Sort Times")



plt.xlabel("N (Length of list to be sorted) ")
plt.ylabel("Time in Seconds")
plt.title("Quicksort vs. Insertion Sort vs. Hybrid Sort Times")
plt.legend()
plt.show()
