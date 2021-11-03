"""https://www.geeksforgeeks.org/python-program-for-quicksort/"""
# Citation for quicksort implementation in python ^
"""https://www.geeksforgeeks.org/python-program-for-insertion-sort/"""
# Citation for insertion sort implementation in python ^
"""https://machinelearningmastery.com/curve-fitting-with-python/"""
# Citation for polynomial curve fitting model used to make lines
import random
import time
import matplotlib.pyplot as plt
import copy
from numpy import arange
from scipy.optimize import curve_fit


def objective(x,a,b,c):
    return a * x + b* x**2 + c

# Python program for implementation of Insertion Sort

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


def partition(arr, low, high):
    i = (low - 1)  # index of smaller element
    pivot = arr[high]  # pivot

    for j in range(low, high):

        # If current element is smaller than or
        # equal to pivot
        if arr[j] <= pivot:
            # increment index of smaller element
            i = i + 1
            arr[i], arr[j] = arr[j], arr[i]

    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    return (i + 1)

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
random.seed(12)

QS_times = []
IS_times = []
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

    # Compute total time for insertion sort and add it to list of times
    #insertion_sort_time = float(float(is_end_time) - float(is_start_time))
    IS_times.append(insert_sort_avg)
    # print((end_time - start_time)*1000, " milliseconds")
    #if i == 100:
    #    lin = int(10.0)
    i += 1


X = [x for x in range(1,50)]

print(QS_times)
print(IS_times)

plt.scatter(X,QS_times,label="Quicksort Times")
plt.scatter(X,IS_times,label="Insertion Sort Times")

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

min = 1
n_val = 0
for i in range(1,49):
    temp = abs(float(y_line_2[i])-float(y_line[i]))
    if( temp < min):
        min = temp
        n_val = i
print("Point at which Quicksort is better = " + str(n_val))
print(min)



plt.xlabel("N (Length of list to be sorted) ")
plt.ylabel("Time in Seconds")
plt.title("Quicksort vs. Insertion Sort Times")
plt.legend()
plt.show()

