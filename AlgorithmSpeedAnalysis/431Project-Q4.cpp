#include<iostream>
#include<vector>
#include<ctime>
#include<set>

/* https://www.geeksforgeeks.org/search-insert-position-of-k-in-a-sorted-array/ */
// ^ citation for implementing binary search insertion of c++ vector

class sorted_vector {
public:
	sorted_vector() {}
	void insert(int val)
	{
		int start = 0;
		int end = vec.size()-1;
		int pos;
		//std::cout << val << " ";
		while (start <= end)
		{
			pos = (start + end) / 2;
			if (vec[pos] == val)
			{
				break;
			}
			else if (vec[pos] < val)
			{
				start = pos + 1;
			}
			else
			{
				end = pos - 1;
			}
		}
		if (start >= end)
		{
			pos = end + 1;
		}
		auto it = vec.begin() + pos;
		vec.insert(it, val);
	}

private:
	std::vector<int> vec;
};

int main()
{
	sorted_vector sorted_vec = sorted_vector();

	int n = 10;
	std::vector<double> sorted_vec_times;
	std::vector<int> N_vals;

	while (n <= 1600000)
	{
		std::cout << n << "\n";
		N_vals.push_back(n);
		std::vector<int> rands;
		// initialize sorted vector
		sorted_vector sorted_vec = sorted_vector();
		
		// initialize random values to insert
		for (int i = 0; i < n; i++)
		{
			int rand_num = rand();
			rands.push_back(rand_num);
		}
		// time the insertion
		std::clock_t start_time = std::clock();
		for (int i = 0; i < n; i++)
		{
			sorted_vec.insert(rands[i]);
		}
		std::clock_t tot_time = std::clock() - start_time;
		double vec_time = ((double)tot_time / (double)CLOCKS_PER_SEC);
		sorted_vec_times.push_back(vec_time);

		if (n >= 100000)
		{
			n = n + 100000;
		}
		else
		{
			n = n * 10;
		}
		if (n > 1600000)
		{
			std::cout << "N = " << n << "\n";
			std::cout << "Sorted Vector Times: [ " << "\n";
			for (auto time : sorted_vec_times)
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
	}
}
