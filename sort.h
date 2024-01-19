#pragma once

namespace Tmpl8
{
	class Game;

	template <
		typename T, 
		typename Get,
		typename Class
		>
	class Sort
	{
	public:

		/// <summary>
		/// pretty standard binary search implementation.
		/// </summary>
		/// <param name="vector"> ref to a vector we're searching in</param>
		/// <param name="get">a pointer to a function that takes an instance of the templated class and returns a numerical value</param>
		/// <param name="clss">the class that the get function belongs to. We can't call it if we don't have that function for misterious c++ reasons</param>
		/// <param name="value">the value that we're trying to find.</param>
		/// <param name="start_of_range">the index that we want to start searching at</param>
		/// <param name="end_of_range">the index that we want to stop our search at</param>
		/// <returns>the index of the value we're trying to find</returns>
		static int binary_search(vector<T>& vector, Get get, Class clss, int value, int start_of_range, int end_of_range) {
			if (start_of_range == end_of_range) {
				if ((clss->*get)(vector[start_of_range]) > value) {
					return start_of_range;
				}
				return start_of_range + 1;
			}


			if (start_of_range > end_of_range) {
				return start_of_range;
			}


			int middle = round((start_of_range + end_of_range) / 2);

			if ((clss->*get)(vector[middle]) < value) {
				return binary_search(vector, get, clss, value, middle + 1, end_of_range);
			}
			else if ((clss->*get)(vector[middle]) > value) {
				return binary_search(vector, get, clss, value, start_of_range, middle - 1);
			}

			return middle;
		}

		//lazy overload, code is the same as above. 
		//I don't feel like figuring out a clean way to solve this with templates or whatever crazy stuff c++ has waiting for me.

		/// <summary>
		/// pretty standard binary search implementation.
		/// </summary>
		/// <param name="vector"> ref to a vector we're searching in</param>
		/// <param name="get">a pointer to a function that takes an instance of the templated class and returns a numerical value</param>
		/// <param name="clss">the class that the get function belongs to. We can't call it if we don't have that function for misterious c++ reasons</param>
		/// <param name="value">the value that we're trying to find.</param>
		/// <param name="start_of_range">the index that we want to start searching at</param>
		/// <param name="end_of_range">the index that we want to stop our search at</param>
		/// <returns>the index of the value we're trying to find</returns>
		static int binary_search(vector<T>& vector, Get get, Class clss, float value, int start_of_range, int end_of_range) {
			if (start_of_range == end_of_range) {
				if ((clss->*get)(vector[start_of_range]) > value) {
					return start_of_range;
				}
				return start_of_range + 1;
			}


			if (start_of_range > end_of_range) {
				return start_of_range;
			}


			int middle = round((start_of_range + end_of_range) / 2);

			if ((clss->*get)(vector[middle]) < value) {
				return binary_search(vector, get, clss, value, middle + 1, end_of_range);
			}
			else if ((clss->*get)(vector[middle]) > value) {
				return binary_search(vector, get, clss, value, start_of_range, middle - 1);
			}

			return middle;
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
		/// <param name="result"></param>
		/// <param name="get"></param>
		/// <param name="clss"></param>
		static void merge(vector<T>& a, vector<T>& b, vector<T>& result, Get get, Class clss) {
			result.reserve(a.size() + b.size());

			//if either of the vectors is empty we can just return the other
			if (a.size() == 0) {
				result = b;
				return;
			}

			if (b.size() == 0) {
				result = a;
				return;
			}

			//if the last item in either sorted list is less than the first in the other we can just combine them in order;
			if ((clss->*get)(a.back()) < b[0]) {
				result = a;
				result.insert(result.end(), begin(b), end(b));
				return;
			}

			if ((clss->*get)(b.back()) < a[0]) {
				result = b;
				result.insert(result.end(), begin(a), end(a));
				return;
			}


			//otherwise we have to compare the items in the list one by one to see which one to add first;
			int index_a = 0;
			int index_b = 0;

			while (index_a < a.size() && index_b < b.size()) {
				if ((clss->*get)(a[index_a]) < (clss->*get)(b[index_b])) {
					result.emplace_back(a[index_a]);
					index_a++;
				}
				else {
					result.emplace_back(b[index_b]);
					index_b++;
				}
			}

			//when one of the lists is empty we need to add the remainder of the other
			while (index_a < a.size()) {
				result.emplace_back(a[index_a]);
				index_a++;
			}

			while (index_b < b.size()) {
				result.emplace_back(b[index_b]);
				index_b++;
			}
		}

		static void merge(vector<T>& a, vector<T>& b, Get get, Class clss) {
			b.reserve(a.size() + b.size());

			//if either of the vectors is empty we can just return the other
			if (a.size() == 0) {
				return;
			}

			if (b.size() == 0) {
				b = a;
				return;
			}

			//if the last item in either sorted list is less than the first in the other we can just combine them in order;
			if ((clss->*get)(a.back()) < (clss->*get)(b[0])) {
				b.insert(b.begin(), begin(a), end(a));
				return;
			}

			if ((clss->*get)(b.back()) < (clss->*get)(a[0])) {
				b.insert(b.end(), begin(a), end(a));
				return;
			}


			//otherwise we have to compare the items in the list one by one to see which one to add first;
			int index_a = 0;
			int index_b = 0;

			while (index_a < a.size() && index_b < b.size()) {
				if ((clss->*get)(a[index_a]) < (clss->*get)(b[index_b])) {
					b.insert(b.begin() + index_b, a[index_a]);
					index_a++;
				}
				else {
					index_b++;
				}
			}

			//when vector b is done but vec a is not we need to add the remainder of a to b
			while (index_a < a.size()) {
				b.emplace_back(a[index_a]);
				index_a++;
			}
		}
		
		/// <summary>
		/// An implementation of the insertion sort algorithm. This implementation sorts in place. 
		/// Insertion sort is slow on large data sets but relatively efficient on small, highly ordered data sets.
		/// Worst case peformance is O(n^2) comparisons and swaps. Best case peformance is O(n) comparisons and O(1) swaps. 
		/// </summary>
		/// <param name="vec_ref_original">A pointer to the unsorted data.</param>
		/// <param name="compare">A function pointer to, or function object of, a function that returns a value that can be compared. 
		/// This should compare the two instances.</param>
		/// <param name="clss">A pointer to the instance of the class that this function belongs to</param>
		static void insertion_sort(vector<T>& vector, Get get, Class clss) {

			//no need to sort vector of size 1 or 0
			if (vector.size() < 2) {
				return;
			}

			//textbook implementation of a insertion sort
			int i = 1;
			while (i < vector.size()) {
				int j = i;      //   v this syntax down here is in order to call the function at a function pointer that's a member of a class. Why is c++ like this?????
				while (j > 0 && (clss->*get)(vector[j - 1]) > (clss->*get)(vector[j])) {
					swap(vector[j - 1], vector[j]);
					j--;
				}
				i++;
			}
			return;
		}

		/// <summary>
		/// An implementation of the insertion sort algorithm. This implementation sorts in place. A variant that utilizes 
		/// Insertion sort is slow on large data sets but relatively efficient on small, highly ordered data sets.
		/// Worst case peformance is O(n^2) comparisons and swaps. Best case peformance is O(n) comparisons and O(1) swaps. 
		/// </summary>
		/// <param name="vec_ref_original">A pointer to the unsorted data.</param>
		/// <param name="compare">A function pointer to, or function object of, a function that returns a value that can be compared. 
		/// This should compare the two instances.</param>
		/// <param name="clss">A pointer to the instance of the class that this function belongs to</param>
		static void binary_insertion_sort(vector<T>& vector, Get get, Class clss) {

			//no need to sort vector of size 1 or 0
			if (vector.size() < 2) {
				return;
			}

			//textbook implementation of a insertion sort
			int i = 1;
			while (i < vector.size()) {
				int desired_index = binary_search(vector, get, clss, (clss->*get)(vector[i]), 0, i - 1);
				T val = move(vector[i]);
				vector.erase(vector.begin() + i);
				vector.insert(vector.begin() + desired_index, val);

				i++;
			}
			return;
		}

	
		

		static void simplified_timsort(vector<T>& input, vector<T>& result, Get get, Class clss) {
			//for since insertion sort algorithms work well on small arrays we won't bother with this algo if it the input is small
			//cutoff point is arbitrary here. I did not get to test this.
			if (input.size() < 64) {
				result = input;
				binary_insertion_sort(result, get, clss);
				return;
			}
			
			result.reserve(input.size());

			//we're just shoving this in a vec but actual timsort uses a stack here
			//vector<vector<T>> runs;

			vector<T> new_run;
			new_run.emplace_back(input[0]);

			int i = 1;
			while (true) {
				//if we reach the end of the array we can wrap up.
				if (i == (int) input.size() - 1) {
					//prepare for the next run
					merge(new_run, result, get, clss);
					new_run.clear();
					new_run.emplace_back(input[i]);
					merge(new_run, result, get, clss);
					break;
				}

				//try to start a new run
				

				if ((clss->*get)(input[i]) >= (clss->*get)(new_run[new_run.size() - 1])) { //ascending run
					new_run.emplace_back(input[i]);
					i++;
					while ((clss->*get)(input[i]) >= (clss->*get)(new_run[new_run.size() -1]) && i != (int)input.size() - 1) {
						new_run.emplace_back(input[i]);
						i++;
					}

					// the run has ended
					merge(new_run, result, get, clss);
					new_run.clear();

					if (i < input.size() - 1) {
						new_run.emplace_back(input[i]);
						i++;
					}
					
				}
				else if ((clss->*get)(input[i]) < (clss->*get)(new_run[new_run.size() - 1])) { //descending run
					new_run.emplace_back(input[i]);
					i++;
					while ((clss->*get)(input[i]) < (clss->*get)(new_run[new_run.size() - 1]) && i != (int)input.size() - 1) {
						new_run.emplace_back(input[i]);
						i++;
					}

					// the run has ended
					reverse(new_run.begin(), new_run.end()); //make the run ascending;
					merge(new_run, result, get, clss);
					new_run.clear();

					if (i < input.size() - 1) {
						new_run.emplace_back(input[i]);
						i++;
					}
				}
			}

		}

	};

}