// 431Project-Q3.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <set>
#include <unordered_set>
#include <ctime>

int main()
{
    int n = 10;
    std::vector<double> hash_times;
    std::vector<double> btree_times;
    std::vector<int> N_vals;
    while (1)
    {
        N_vals.push_back(n);
        std::multiset<int> ms;
        std::unordered_multiset<int> u_ms;

        std::vector<int> rands;

        for (int i = 0; i < n; i++)
        {
            int rand_num = rand();
            rands.push_back(rand_num);
        }

        std::clock_t start_time = std::clock();
        for (int i = 0; i < n; i++)
        {
            ms.insert(rands[i]);
        }
        std::clock_t tot_time = std::clock() - start_time;
        double btree_time = ((double)tot_time / (double) CLOCKS_PER_SEC);
        btree_times.push_back(btree_time);

        start_time = std::clock();
        for (int i = 0; i < n; i++)
        {
            u_ms.insert(rands[i]);
        }
        tot_time = std::clock() - start_time;
        double hash_time = ((double)tot_time / (double)CLOCKS_PER_SEC);
        hash_times.push_back(hash_time);

        if (hash_time > 3.0 || btree_time > 3.0)
        {
            std::cout << "N = " << n << "\n";
            std::cout << "Hash Table Time: " << hash_time << "\n";
            std::cout << "Binary Tree Time: " << btree_time << "\n";
            
            std::cout << "Hash Table Times: [";
            for (auto time : hash_times)
            {
                std::cout << time << ",";
            }
            std::cout << "]\n";
            std::cout << "Binary Tree Times: [";
            for (auto time : btree_times)
            {
                std::cout << time << ",";
            }
            std::cout << "]\n";
            std::cout << "N values: [";
            for (auto n_val : N_vals)
            {
                std::cout << n_val << ",";
            }
            std::cout << "]\n";
            break;
        }
        if (n >=100000)
        {
            n = n + 100000;
        }
        else
        {
            n = n * 10;
        }
       


    }
}


