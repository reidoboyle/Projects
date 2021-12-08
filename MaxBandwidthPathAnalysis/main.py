import random
from Graph2 import G2
from Graph1 import G1
from MBP_no_heap import Dijkstras
from MBP_with_heap import Dijkstras_with_H
from MaxHeap import MaxHeap
from Kruskals import Kruskals_Main
import time

"""
Running the main file creates the graphs and runs the 3 algorithms
and outputs the results in lists at the end. To run the analysis and 
see the graphs from the report, copy the lists from the output into the lists
in analysis.py and run analysis.py to see the graphs. 
"""

source = 10
destination = 456

kruskal_g1_times = []
kruskal_g2_times = []
djk_nh_g1_times = []
djk_nh_g2_times = []
djk_g1_times = []
djk_g2_times = []
s_d = []
for i in range(5):
    random.seed(random.randint(1,200))
    print('Generating graphs G1 and G2')
    g1 = G1()
    g2 = G2()
    for i in range(5):
        source = random.randint(0,4999)
        destination = random.randint(0,4999)
        s_d.append([source,destination])
        print(source,destination)

        while destination == source:
            destination = random.randint(0,4999)

        print('Running Kruskals')
        k_start_time = time.perf_counter()
        Kruskals_Main(g1,source,destination)
        k_time = float(time.perf_counter()) - float(k_start_time)
        kruskal_g1_times.append(k_time)
        print(k_time)
        ################################
        k_start_time = time.perf_counter()
        Kruskals_Main(g2,source,destination)
        k_time = float(time.perf_counter()) - float(k_start_time)
        kruskal_g2_times.append(k_time)
        print(k_time)

        print('Running Dijkstras with Heap')
        dh_start_time = time.perf_counter()
        Dijkstras_with_H(g1,source,destination,5000)
        dh_time = float(time.perf_counter()) - float(dh_start_time)
        djk_g1_times.append(dh_time)
        print(dh_time)
        #################################
        dh_start_time = time.perf_counter()
        Dijkstras_with_H(g2, source, destination, 5000)
        dh_time = float(time.perf_counter()) - float(dh_start_time)
        djk_g2_times.append(dh_time)
        print(dh_time)

        print('Running Dijkstras without Heap')
        d_start_time = time.perf_counter()
        Dijkstras(g1,source,destination,5000)
        d_time = float(time.perf_counter()) - float(d_start_time)
        djk_nh_g1_times.append(d_time)
        print(d_time)
        #################################
        d_start_time = time.perf_counter()
        Dijkstras(g2, source, destination, 5000)
        d_time = float(time.perf_counter()) - float(d_start_time)
        djk_nh_g2_times.append(d_time)
        print(d_time)


print(kruskal_g1_times)
print(kruskal_g2_times)
print(djk_nh_g1_times)
print(djk_nh_g2_times)
print(djk_g1_times)
print(djk_g2_times)
print(s_d)