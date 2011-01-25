#pragma once

#include <cmath>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>

using namespace std;

//Introsort
template <class T>
struct Introsort
{
	vector<T> a;
	static const int block_limit = 16;
	
	void sort(vector<T> &s)
	{
		a = s;
		int size = s.size();
		quicksort(0, size-1, 2*log2(size));
		insertionsort(0, size-1);
	}
		
	//Insertion sort (only used to sort blocks of 'block_limit' size)
	void insertionsort(int x1, int x2)
	{
		for (int x=x1; x<x2; ++x)
		{
			T temp = a[x];
			int pos = x;
			
			while (pos > x1 && a[pos-1] > temp)
			{
				a[pos] = a[pos-1];
				pos--;
			}
			a[pos] = temp;
		}
	}	

	//Modified quicksort that switches to heapsort when recursion is excessive
	void quicksort(int x1, int x2, int depth_limit)
	{
		//Small ranges are left for the final insertion sort over the whole array
		if (x2-x1 <= block_limit) return;
		
		//Switch to heapsort if recursion limit has been hit
		if (depth_limit == 0)
		{
			heapsort(x1,x2);
			return;
		}
		int x = partition(x1,x2);
		quicksort(x1, x-1, depth_limit - 1);
		quicksort(x, x2, depth_limit - 1);
	}
	
	//Partition a subsection of the array into two blocks around a specified pivot. Returns the start index of the right block
	int partition(int x1, int x2)
	{
		int pivot = choose(x1, x1 + ((x2-x1)/2), x2);
		
		while (true)
		{
			while (a[x1] <  pivot) x1++; //Guarantees that left block does not contain any pivot values
			while (a[x2] >= pivot) x2--;

			if (x1 >= x2) return x1;
			swap(x1,x2);

			x1++;
			x2--;			
		}
	}
	
	//From three indexes, choose the one which maps to the middle value
	int choose(int x1, int x2, int x3)
	{
		if (a[x1] <= a[x2])
		{
			if (a[x2] <= a[x3]) return a[x2]; //1,2,3
			if (a[x1] <= a[x3]) return a[x3]; //1,3,2
			return a[x1]; //3,1,2
		}
		if (a[x1] <= a[x3]) return a[x1]; //2,1,3
		if (a[x3] <= a[x2]) return a[x2]; //3,2,1
		return a[x3]; //2,3,1
	}
	
	//Heapsort
	void heapsort(int x1, int x2)
	{
		int n = x2 - x1 + 1;
		
		for (int i=n/2; i>0; --i)
		{
			sink(i,n,x1);
		}
		for (int i=n; i>1; --i)
		{
			swap(x1, x1+i-1);
			sink(1, i-1, x1);
		}
    }

	//Sink down the stack
	void sink(int i, int size, int x1)
	{
		int value = a[x1 + i - 1];
		int child;

		while (i <= size/2)
		{
			child = 2 * i;
			
			if (child < size && a[x1 + child - 1] < a[x1 + child])
			{
				child++;
			}
			if (value >= a[x1 + child - 1]) break;
			a[x1 + i - 1] = a[x1 + child - 1];
			i = child;
		}
		a[x1 + i - 1] = value;
	}

	//Swap two elements
	void swap(int x1, int x2)
	{
		T tmp = a[x1];
		a[x1] = a[x2];
		a[x2] = tmp;
	}
	
	//Find the log of a value
	int log2(int x)
	{
		return (int)(floor(log(x) / log(2)));
    }
};

template <class T>
struct QuickSort
{
	vector<T> data;
	int k;
	
	void sort(vector<T> &d)
	{
		k = 0;
		data = d;
		quicksort(0, data.size()-1);
	}

	//Inplace divide and conquer, n.log(n)
	void quicksort(int x1, int x2)
	{
		if (x1 >= x2)
		{
			int x = partition(x1, x2);
			quicksort(x1, x);
			quicksort(x+1, x2);
		}
	}

	//Partition the data array around a pivot
	int partition(int x1, int x2)
	{
		T pivot = data[choose(x1,x2)]; //choose(x1,x2)
		x1--;x2++;

		while (true)
		{
			do x1++; while (data[x1] < pivot);
			do x2--; while (data[x2] > pivot);
			if (x1 < x2) swap(x1,x2);
			else return x2;
		}
	}
	
	//From three indexes, choose the one which maps to the middle value
	int choose(int x1, int x3)
	{
		int x2 = x1 + ((x3-x1)/2);
		
		if (data[x1] <= data[x2])
		{
			if (data[x2] <= data[x3]) return x2; //1,2,3
			if (data[x1] <= data[x3]) return x3; //1,3,2
			return x1; //3,1,2
		}
		if (data[x1] <= data[x3]) return x1; //2,1,3
		if (data[x3] <= data[x2]) return x2; //3,2,1
		return x3; //2,3,1
	}
	
	//Swap two elements
	void swap(int x1, int x2)
	{
		T tmp = data[x1];
		data[x1] = data[x2];
		data[x2] = tmp;
	}
	
	//Ternary quicksort. TODO: get this to actually work
	void qsort3(int x1, int x2)
	{
		int p1 = x1;
		int p2 = x2;

		T pivot = data[choose(x1,x2)];
		x1--;x2++;

		while (true)
		{
			do x1++; while (data[x1] < pivot);
			do x2--; while (data[x2] > pivot);
			if (x1 < x2) 
			{
				swap(x1,x2);
				if (data[x1] == pivot) swap(x1,p1);
				if (data[x2] == pivot) swap(x2,p2);
			}
			else break;
		}
		
		//Swap the pivots at the edges back to the middle
		for (int k = x1; k < p1; ++k) { swap(data,k,p2); p2--; }
		for (int k = x2; k > p2; --k) { swap(data,p1,k); p1++; }

		//Recursion
		qsort(data, x1, p2);
		qsort(data, p1, x2);
	}	
};

template <class T>
struct HeapSort
{
	vector<T> data;
	
	//Heapsort
	void sort(vector<T> &d)
	{
		data = d;
		
		int size = data.size();
		
		for (int i=size/2; i>0; --i)
		{
			sink(i,size);
		}
		for (int i=size; i>1; --i)
		{
			swap(0, i-1);
			sink(1, i-1);
		}
		d = data;
    }

	//Sink down the stack
	void sink(int i, int size)
	{
		int value = data[i-1];

		while (i <= size/2)
		{
			int child = 2*i;
			
			if (child < size && data[child-1] < data[child])
			{
				child++;
			}
			if (value >= data[child-1]) break;
			data[i-1] = data[child-1];
			i = child;
		}
		data[i-1] = value;
	}

	//Swap two elements
	void swap(int x1, int x2)
	{
		T tmp = data[x1];
		data[x1] = data[x2];
		data[x2] = tmp;
	}	
};

template <class T>
struct MergeSort
{
	vector<T> data;
	vector<T> temp;
	
	void sort(vector<T> &d)
	{
		data = d;
		int size = data.size();
		temp.resize(size);
		
		for (int b=1; b<size; b*=2)
		{
			for (int i=0; i<size; i+=2*b)
			{
				merge(i,i+b,b);
			}
			data = temp;
		}
		d = data;
	}
	
	void merge(int x1, int x2, int size)
	{
		//cout << "merge: " << x1 << " " << x2 << " " << size << endl;
		int max = temp.size();
		
		int end1 = x1 + size; if (end1 > max) end1 = max;
		int end2 = x2 + size; if (end2 > max) end2 = max;
		
		int in = x1;
				
		while (x1 < end1 || x2 < end2)
		{
			while (x1 < end1 && (x2 >= end2 || data[x1] <= data[x2])) { temp[in] = data[x1]; x1++; in++; }
			while (x2 < end2 && (x1 >= end1 || data[x2] <= data[x1])) { temp[in] = data[x2]; x2++; in++; }
		}
	}
};

//Specialized block sorting algorithm for strings
struct BlockSortOld
{
	stack<int> ranges;
	vector<int> blocks;
	string sequence;
	int length;
	static const int LETTERS = 256;
	vector<int> counts;
	vector<int> starts;
	vector<bool> flags;
		
	void sort(string &s, vector<int> &blocks)
	{
		this->sequence = s;
		this->blocks = blocks;
		this->length = s.size();
		
		this->blocks.resize(this->length);
		//this->ranges.clear();
		
		this->counts.clear();
		this->counts.resize(LETTERS,0);
		
		this->starts.clear();
		this->starts.resize(LETTERS,0);
		
		this->flags.clear();
		this->flags.resize(this->length,false);
				
		for (int i=0; i<length; ++i)
		{
			this->blocks[i] = i;
		}

		range(0, this->length-1, 0);
		
		while (!this->ranges.empty())
		{
			int offset = ranges.top(); ranges.pop();
			int count = ranges.top(); ranges.pop();
			int start = ranges.top(); ranges.pop();
			range(start, start + count - 1, offset);
		}		
		blocks = this->blocks;
	}
		
	//Quickly determine if all the characters are the same
	bool same(int x1, int x2, int offset)
	{
		char c;
		
		for (int i=x1; i<=x2; ++i)
		{
			int pos = blocks[i] + offset;
			if (pos >= length) pos -= length;
			if (i==x1) c = sequence[pos];
			if (sequence[pos] != c) return false;
		}
		return true;
	}

	//Sort all the blocks in a range by reference to an internal offset character
	void range(int x1, int x2, int offset)
	{
		if (x1 >= x2) return;
				
		//Take a shortcut if possible
		while(same(x1,x2,offset))
		{
			if (++offset >= length) return;
		}		

		//Reset counts
		for (int i=0; i<LETTERS; ++i)
		{
			counts[i] = 0;
		}
	
		//Count characters in the range
		for (int i=x1; i<=x2; ++i)
		{
			int pos = blocks[i] + offset;
			if (pos >= length) pos -= length;
			char c = sequence[pos];
			counts[(unsigned char)c]++;
			flags[i] = false;
		}

		//Set the starts for each character block
		starts[0] = x1;
	
		for (int i=1; i<LETTERS; ++i)
		{
			starts[i] = starts[i-1] + counts[i-1];
			
			//Record subrange if necessary
			if (counts[i] > 0)
			{
				ranges.push(starts[i]);
				ranges.push(counts[i]);
				ranges.push(offset+1);
			}
		}
		
		//Reorder the elements in this range (in-place sort)
		for (int i=x1; i<=x2; ++i)
		{
			int index = i;
			int block = blocks[index];

			//Trace the block
			while (true)
			{
				//Skip if the block has already been processed
				if (flags[index] || index < i) break;
				
				//Flag this visit
				flags[index] = true;
				
				//Determine the destination where this block will go
				int pos = block + offset;
				if (pos >= length) pos -= length;
				char c = sequence[pos];
				int dest = starts[(unsigned char)c];

				//Increment the visit count
				starts[(unsigned char)c]++;

				//Skip if already in the right place
				if (blocks[dest] == block) break;

				//Do the switch
				int temp = blocks[dest];
				blocks[dest] = block;
				block = temp;
				index = dest;
			}
		}
	}
};

/**
 * Start with an initial 2-byte radix sort
 * Use a cache to record blocks that have been terminally placed
 * Process each sub block using std::sort and a custom comparator that looks for cache hits
 * Use sub-block sorts to reduce complexity of main block sorts
 *
struct BZSort
{
	static const int LETTERS = 256;
	
	struct Radix
	{
		unsigned char byte1;
		unsigned char byte2;
		bool sorted;
		unsigned int start;
		unsigned int count;
	};
		
	string sequence;
	int length;
	vector<int> blocks;
	vector<int> cache;
	vector<vector<Radix> > table;
	
	//Comparator object for comparing sequence blocks
	struct Comparator
	{
		//Internal pointers required because internal comparator cannot access class scope direcly
		vector<int> * cache;
		string * sequence;
		int length;
		int offset;

		bool operator() (int x1, int x2)
		{
			cout << ".";
			x1 += offset; if (x1 >= length) x1 -= length;
			x2 += offset; if (x2 >= length) x2 -= length;
			
			for (int i=offset; i<length; ++i)
			{
				if ((*sequence)[x1] != (*sequence)[x2])
				{
					return (*sequence)[x1] < (*sequence)[x2];
				}
				if ((*cache)[x1] >= 0 && (*cache)[x2] >= 0)
				{
					return (*cache)[x1] < (*cache)[x2];
				}
				x1++; if (x1 == length) x1 = 0;
				x2++; if (x2 == length) x2 = 0;
			}
			return false;
		}
	} comparator;
	
	//Comparator for sorting ranges by size (smallest to largest)
	static bool compare_radix(const Radix &a, const Radix &b)
	{
		return a.count < b.count;
	}
	
	void sort(string &s, vector<int> &b)
	{
		blocks   = b;
		sequence = s;
		length   = s.size();
		
		cout << "length is: " << length << endl;
		
		blocks.clear();
		blocks.resize(length);
		
		cache.clear();
		cache.resize(length,-1);
		
		comparator.sequence = &sequence;
		comparator.cache = &cache;
		comparator.length = length;
		
		//Initialize the blocks
		for (int i=0; i<length; ++i)
		{
			blocks[i] = i;
		}
		
		//Build the radix table and reorder the blocks
		cout << "Building table" << endl;
		build_radix_table();
		cout << "Reordering blocks" << endl;
		reorder_blocks();
		cout << "Sorting" << endl;
		
		//Sort outer radix ranges (smallest to largest)
		vector<Radix> order;
		order.resize(LETTERS);
		
		for (int i=0; i<LETTERS; ++i)
		{
			order[i].byte1 = i;
			order[i].count = 0;
			
			for (int j=0; j<LETTERS; ++j)
			{
				order[i].count += table[i][j].count;
			}
		}
		std::sort(order.begin(), order.end(), compare_radix);

		//Sort each of the outer radix ranges
		for (int i=0; i<order.size(); ++i)
		{
			//Skip if the outer radix range is empty
			if (order[i].count == 0) continue;
			
			//Sort all the [byte1,n] segments
			cout << "sort outer radix:" << (char)order[i].byte1 << endl;
			sort_byte1(order[i].byte1);
			
			//Update all the [n,byte1] segments
			update(order[i].byte1);
		}
		b = this->blocks;
	}
	
	//Get the radix value from a specific location
	void get_radix(int x1, int &byte1, int &byte2)
	{
		int x2 = x1 + 1; 
		if (x2 == length) x2 = 0;

		byte1 = (unsigned char)sequence[x1];
		byte2 = (unsigned char)sequence[x2];
	}
			
	//Generate the 2-byte radix table
	void build_radix_table()
	{
		//Initialize the radix table
		table.clear();
		table.resize(LETTERS);
		
		for (int i=0; i<LETTERS; ++i)
		{
			table[i].resize(LETTERS);
			
			for (int j=0; j<LETTERS; ++j)
			{
				table[i][j].byte1 = i;
				table[i][j].byte2 = j;
				table[i][j].sorted = false;
				table[i][j].start = 0;
				table[i][j].count = 0;
			}
		}

		//Count each radix
		for (int i=0; i<length; ++i)
		{
			int b1;
			int b2;
			get_radix(i,b1,b2);
			table[b1][b2].count++;
		}
		
		//Set the offsets
		unsigned int total = 0;
		
		for (int i=0; i<LETTERS; ++i)
		{
			for (int j=0; j<LETTERS; ++j)
			{
				table[i][j].start = total;
				total += table[i][j].count;
			}
		}
		// //Print out the radix table
		// for (int i=0; i<LETTERS; ++i)
		// {
		// 	for (int j=0; j<LETTERS; ++j)
		// 	{
		// 		Radix r = table[i][j];
		// 		if (r.count == 0) continue;
		// 		//cout << (char)r.byte1 << (char)r.byte2 << " " << r.start << " " << r.count << endl;
		// 	}
		// }
	}
	
	//Reorder blocks (in-place) from the radix table
	void reorder_blocks()
	{
		vector<bool> placed;
		placed.resize(length,false);
		
		// //Print the blocks
		// for (int i=0; i<blocks.size(); i++)
		// {
		// 	cout << blocks[i] << " ";
		// }
		// cout << endl;
		
		//Reorder elements (in-place)
		for (int i=0; i<length; ++i)
		{
			int index = i;
			int block = blocks[index];

			//Trace the block
			while (true)
			{
				//Skip if the block has already been processed
				if (placed[index] || index < i) break;
				
				//Flag this placement
				placed[index] = true;
				
				//Determine the destination where this block will go
				int b1,b2; get_radix(block,b1,b2);
				int dest = table[b1][b2].start;

				//Increment the visit count
				table[b1][b2].start++;

				//Skip if already in the right place
				if (blocks[dest] == block) break;

				//Do the switch
				int temp = blocks[dest];
				blocks[dest] = block;
				block = temp;
				index = dest;
			}
		}
		
		// //Print the blocks
		// for (int i=0; i<blocks.size(); i++)
		// {
		// 	cout << blocks[i] << " ";
		// }
		// cout << endl;
		
		
		//Reset the radix starts
		for (int i=0; i<LETTERS; ++i)
		{
			for (int j=0; j<LETTERS; ++j)
			{
				table[i][j].start -= table[i][j].count;
			}
		}
		
		// //Print out the radix table
		// for (int i=0; i<LETTERS; ++i)
		// {
		// 	for (int j=0; j<LETTERS; ++j)
		// 	{
		// 		Radix r = table[i][j];
		// 		if (r.count == 0) continue;
		// 		cout << (char)r.byte1 << (char)r.byte2 << " " << r.start << " " << r.count << endl;
		// 	}
		// }		
	}
	
// 0 aaabaa aaaaab 4
// 1 aabaaa aaaaba 5
// 2 abaaaa aaabaa 0
// 3 baaaaa aabaaa 1
// 4 aaaaab abaaaa 2
// 5 aaaaba baaaaa 3
	
	//Sort an outer radix group
	void sort_byte1(int byte1)
	{
		//Reorder the inner radix ranges (smallest to largest)
		vector<Radix> order = table[byte1];
		std::sort(order.begin(), order.end(), compare_radix);
		
		for (int i=0; i<LETTERS; ++i)
		{
			Radix inner = order[i];
			
			//No need to sort it if already done
			if (inner.sorted)
			{
				continue;
			}
			
			//Flag this entry
			table[inner.byte1][inner.byte2].sorted = true;
			
			//No need to do anything if the range is empty
			if (inner.count == 0)
			{
				continue;
			}
			
			//Sort the inner bucket
			int x1 = inner.start;
			int x2 = x1 + inner.count - 1;

			cout << "sorting inner radix [" << (char)inner.byte1 << "," << (char)inner.byte2 << "," << inner.count << "]..." << flush;
			sort_byte2(x1,x2);
			cout << "done" << endl;
		}
	}
	
	//Sort an inner radix group
	void sort_byte2(int x1, int x2)
	{
		if (x1 > x2) return;
		
		//Sort if there is more than 1 element
		if (x1 < x2)
		{
			vector<int>::iterator itr1 = blocks.begin();
			vector<int>::iterator itr2 = blocks.begin();

			itr1 += x1;
			itr2 += x2 + 1;

			std::sort(itr1,itr2,comparator);
		}
		
		//Print the blocks
		// for (int i=0; i<length; ++i)
		// {
		// 	cout << blocks[i] << " ";
		// }
		// cout << endl;
		
		//Proper order is now known, set the cache
		for (int i=x1; i<=x2; ++i)
		{
			int block = blocks[i];
			cache[block] = i;
			//cout << "Setting cache: " << block << " -> " << i << endl;
		}
	}
	
	//When an outer radix has been sorted, all inner radix value == outer radix can be updated
	void update(int byte1)
	{
		//Get the range represented by the outer radix
		int x1 = table[byte1][0].start;
		int x2 = table[byte1][LETTERS-1].start + table[byte1][LETTERS-1].count - 1;
		
		vector<int> counters;
		counters.clear();
		counters.resize(LETTERS,0);
		
		//For each block in the range
		for (int x=x1; x<=x2; ++x)
		{
			//Get the preceding block
			int block = blocks[x] - 1;
			if (block < 0) block = length - 1;
			
			//Get the character mapping to that block
			unsigned char c = sequence[block];
			
			//Skip if already sorted
			if (table[c][byte1].sorted)
			{
				continue;
			}
			
			//Get the current position offset in the range covered by radix [c,byte1]
			int pos = table[c][byte1].start + counters[c];
			
			//Set the value of the block
			blocks[pos] = block;
			
			//Store it in the cache
			cache[block] = pos;
			
			//Update the offset counter
			counters[c]++;
		}
		
		//Flag all the [n,byte1] radix values as sorted
		for (int i=0; i<LETTERS; ++i)
		{
			table[i][byte1].sorted = true;
		}
	}
};

*/
/**
 * Start with an initial 2-byte radix sort
 * Use a cache to record blocks that have been terminally placed
 * Process each sub block using std::sort and a custom comparator that looks for cache hits
 * Use sub-block sorts to reduce complexity of main block sorts
 *
struct CachedBlockSort
{
	static const int LETTERS = 256;

	struct Radix
	{
		unsigned char byte1;
		unsigned char byte2;
		bool sorted;
		unsigned int start;
		unsigned int count;
	};

	string sequence;
	int length;
	vector<int> blocks;
	vector<int> cache;
	vector<vector<Radix> > table;

	//Comparator object for comparing sequence blocks
	struct Comparator
	{
		//Internal pointers required because internal comparator cannot access class scope direcly
		vector<int> * cache;
		vector<int> * blocks;
		string * sequence;
		int length;
		int offset;

		bool operator() (int x1, int x2)
		{
			x1 = (*blocks)[x1] + offset; if (x1 >= length) x1 -= length;
			x2 = (*blocks)[x2] + offset; if (x2 >= length) x2 -= length;

			if ((*sequence)[x1] != (*sequence)[x2])
			{
				return (*sequence)[x1] < (*sequence)[x2];
			}
			else if ((*cache)[x1] != -1)
			{
				if ((*cache)[x2] == -1) return true;
				return (*cache)[x1] < (*cache)[x2];
			}
			else
			{
				return false;
			}
		}
	} comparator;

	//Comparator for sorting ranges by size (smallest to largest)
	static bool compare_radix(const Radix &a, const Radix &b)
	{
		return a.count < b.count;
	}

	//Get the radix value from a specific location
	void get_radix(int x1, int &byte1, int &byte2)
	{
		int x2 = x1 + 1; 
		if (x2 == length) x2 = 0;

		byte1 = (unsigned char)sequence[x1];
		byte2 = (unsigned char)sequence[x2];
	}

	//Generate the 2-byte radix table
	void build_radix_table()
	{
		//Initialize the radix table
		table.clear();
		table.resize(LETTERS);

		for (int i=0; i<LETTERS; ++i)
		{
			table[i].resize(LETTERS);

			for (int j=0; j<LETTERS; ++j)
			{
				table[i][j].byte1 = i;
				table[i][j].byte2 = j;
				table[i][j].sorted = false;
				table[i][j].start = 0;
				table[i][j].count = 0;
			}
		}

		//Count each radix
		for (int i=0; i<length; ++i)
		{
			int b1;
			int b2;
			get_radix(i,b1,b2);
			table[b1][b2].count++;
		}

		//Set the offsets
		unsigned int total = 0;

		for (int i=0; i<LETTERS; ++i)
		{
			for (int j=0; j<LETTERS; ++j)
			{
				table[i][j].start = total;
				total += table[i][j].count;
			}
		}
	}

	//Reorder blocks (in-place) from the radix table
	void reorder_blocks()
	{
		vector<bool> placed;
		placed.resize(length,false);

		//Reorder elements (in-place)
		for (int i=0; i<length; ++i)
		{
			int index = i;
			int block = blocks[index];

			//Trace the block
			while (true)
			{
				//Skip if the block has already been processed
				if (placed[index] || index < i) break;

				//Flag this placement
				placed[index] = true;

				//Determine the destination where this block will go
				int b1,b2; get_radix(block,b1,b2);
				int dest = table[b1][b2].start;

				//Increment the visit count
				table[b1][b2].start++;

				//Skip if already in the right place
				if (blocks[dest] == block) break;

				//Do the switch
				int temp = blocks[dest];
				blocks[dest] = block;
				block = temp;
				index = dest;
			}
		}

		//Reset the radix starts
		for (int i=0; i<LETTERS; ++i)
		{
			for (int j=0; j<LETTERS; ++j)
			{
				table[i][j].start -= table[i][j].count;
			}
		}
	}

	//Sort an outer radix group
	void sort_byte1(int byte1)
	{
		//Sort radix ranges (inner) by range size
		vector<Radix> order = table[byte1];
		std::sort(order.begin(), order.end(), compare_radix);

		//Sort each of the inner radix range
		for (int i=0; i<LETTERS; ++i)
		{
			sort_radix_inner(byte1, order[i].byte2);
		}
	}

	//Sort a specific radix group [outer,inner]
	void sort_radix_inner(int byte1, int byte2)
	{
		//Do nothing if already sorted
		if (table[byte1][byte2].sorted) return;
		
		//Flag this radix group
		table[byte1][byte2].sorted = true;
		
		//Do nothing if the radix range is empty
		if (table[byte1][byte2].count == 0) return;
		
		//Determine the range covered by this radix group
		int x1 = table[byte1][byte2].start;
		int x2 = x1 + table[byte1][byte2].count - 1;
		
		//Push the range onto the radix stack (starting at offset 2 because chars 0 and 1 are already known)
		stack_radix.push(2);
		stack_radix.push(x2);
		stack_radix.push(x1);

		//Process the radix stack until it is empty
		while (!stack_radix.empty())
		{
			process_stack_radix();
		}
		
		//Process the merge stack until it is empty
		while (!stack_merge.empty())
		{
			process_stack_merge();
		}

		//Now that the correct ordering is known, update the cache
		for (int i=x1; i<=x2; ++i)
		{
			int block = blocks[i];
			cache[block] = i;
		}
	}

	//Compare two blocks from the specified offset onwards
	bool compare_blocks(int x1, int x2, int offset)
	{
		x1 = blocks[x1] + offset; if (x1 >= length) x1 -= length;
		x2 = blocks[x2] + offset; if (x2 >= length) x2 -= length;

		for (int i=offset; i<length; ++i)
		{
			if (sequence[x1] != sequence[x2])
			{
				return sequence[x1] < sequence[x2];
			}
			if (cache[x1] >= 0 && cache[x2] >= 0)
			{
				return cache[x1] < cache)[x2];
			}
			x1++; if (x1 == length) x1 = 0;
			x2++; if (x2 == length) x2 = 0;
		}
		return false;
	}
	
	//Advance an offset until the character between two blocks is different
	int advance(int x1, int x2, int offset)
	{
		x1 = blocks[x1] + offset; if (x1 >= length) x1 -= length;
		x2 = blocks[x2] + offset; if (x2 >= length) x2 -= length;

		for (int i=0; i<length; ++i)
		{
			if (sequence[x1] != sequence[x2]) break;
			
			x1++; if (x1 == length) x1 = 0;
			x2++; if (x2 == length) x2 = 0;
			offset++;
		}
		return offset;
	}
	
	//Swap two blocks
	void swap_blocks(int x1, int x2)
	{
		if (x1 == x2) return;
		int temp = blocks[x1];
		blocks[x2] = blocks[x1];
		blocks[x1] = temp;
	}
	
	//Process the top element on the radix stack
	void process_stack_radix()
	{
		//Pop the radix range
		int offset = stack_radix.top(); stack_radix.pop();
		int x1 = stack_radix.top(); stack_radix.pop();
		int x2 = stack_radix.top(); stack_radix.pop();

		//Do nothing if illegal
		if (x1 > x2) return;
		
		//No need to do anything if there is only 1 element
		if (x1 == x2) return;
		
		//If there are only 2 elements, do a direct comparison and swap if necessary
		if (x2 - x1 == 1)
		{
			if (!compare_blocks(x1,x2,offset))
			{
				swap_blocks(x1,x2);
			}
			return;
		}
		
		//Sort at the offset to produce blocks sorted by letter and by cache (within each letter group)
		vector<int>::iterator itr1 = blocks.begin();
		vector<int>::iterator itr2 = blocks.begin();

		itr1 += x1;
		itr2 += x2 + 1;
		
		comparator.offset = offset;

		std::sort(itr1,itr2,comparator);

		//Determine the size of the cached(A) and uncached(B) sections in each character group
		int pos = blocks[x1] + offset;
		if (pos >= length) pos -= length;
		char c = sequence[pos];

		int sizeA = 0;
		int sizeB = 0;
		int start = x1;
		
		for (int i=x1; i<=x2; ++i)
		{
			//Get the adjusted position (by offset)
			pos = blocks[x1] + offset; if (pos >= length) pos -= length;
			
			//If the character has not changed and we're not at the end of the loop
			if (sequence[pos] == c && i != x2)
			{
				//Increase sizeA or sizeB depending on whether or not the block is cached
				(cache[pos] == -1) ? sizeB++ : sizeA++;
				continue;
			}
			
			//Process the last block
			c = sequence[pos];

			//Only need to do anything if some of the range is not cached (fully cached range is already sorted)
			if (sizeB > 0)
			{
				//If B is larger then 1 then push it onto the range stack with an increased offset
				if (sizeB > 1)
				{
					stack_range.push(offset);
					stack_range.push(start+sizeA+sizeB);
					stack_range.push(start+sizeA);
				}
				
				//If there were any cached elements then push onto the merge stack
				if (sizeA > 0)
				{
					stack_merge.push(offset);
					stack_merge.push(sizeB);
					stack_merge.push(sizeA);
					stack_merge.push(start);
				}
				
				//Reset the size counters
				sizeA = 0;
				sizeB = 0;
			}
		}
	}
	
	//Merge two ranges from the merge stack. Both ranges are sorted
	void process_stack_merge()
	{
		//Pop range information off the merge stack
		int offset = stack_merge.top(); stack_merge.pop();
		int sizeB = stack_merge.top(); stack_merge.pop();
		int sizeA = stack_merge.top(); stack_merge.pop();
		int start = stack_merge.top(); stack_merge.pop();
		
		int posA = start;
		int posB = posA + sizeA;

		//Build a copy queue
		queue temp;
		
		//Merge the two ranges into the queue
		for (int i=start; i<=end; ++i)
		{
			//Advance the offset until the difference between A and B can be resolved
			offset = advance(posA, posB, offset);
			
			if (compare(posA, posB, offset))
			{
				temp.push(blocks[posA]);
				posA++;
			}
			else
			{
				temp.push(blocks[posB]);
				posB++;
			}
		}
		
		//Copy the queued items back into the original vector
		for (int i=start; i<=end; ++i)
		{
			blocks[x1+i] = temp.top(); temp.pop();
		}
	}
	
	//When an outer radix has been sorted, all inner radix value == outer radix can be updated
	void update(int byte1)
	{
		//Get the range represented by the outer radix
		int x1 = table[byte1][0].start;
		int x2 = table[byte1][LETTERS-1].start + table[byte1][LETTERS-1].count - 1;

		vector<int> counters;
		counters.clear();
		counters.resize(LETTERS,0);

		//For each block in the range
		for (int x=x1; x<=x2; ++x)
		{
			//Get the preceding block
			int block = blocks[x] - 1;
			if (block < 0) block = length - 1;

			//Get the character mapping to that block
			unsigned char c = sequence[block];

			//Skip if already sorted
			if (table[c][byte1].sorted)
			{
				continue;
			}

			//Get the current position offset in the range covered by radix [c,byte1]
			int pos = table[c][byte1].start + counters[c];

			//Set the value of the block
			blocks[pos] = block;

			//Store it in the cache
			cache[block] = pos;

			//Update the offset counter
			counters[c]++;
		}

		//Flag all the [n,byte1] radix values as sorted
		for (int i=0; i<LETTERS; ++i)
		{
			table[i][byte1].sorted = true;
		}
	}	

	//Initialize the object
	void init(string &s, vector<int> &b)
	{
		blocks   = b;
		sequence = s;
		length   = s.size();

		blocks.clear();
		blocks.resize(length);

		cache.clear();
		cache.resize(length,-1);

		comparator.sequence = &sequence;
		comparator.cache = &cache;
		comparator.length = length;

		//Initialize the blocks
		for (int i=0; i<length; ++i)
		{
			blocks[i] = i;
		}
	}
	
	//Main sorting routine
	void sort(string &s, vector<int> &b)
	{
		//Initialize the object
		init();
		
		//Build the master radix table
		build_table();
	
		//Reorder elements into radix groups
		reorder();
	
		//Determine the running order (smallest to largest outer radix group)
		vector<Radix> order;
		order.resize(LETTERS);

		for (int i=0; i<LETTERS; ++i)
		{
			order[i].byte1 = i;
			order[i].count = 0;

			for (int j=0; j<LETTERS; ++j)
			{
				order[i].count += table[i][j].count;
			}
		}
		std::sort(order.begin(), order.end(), compare_radix);

		//For each of the outer radix groups
		for (int i=0; i<order.size(); ++i)
		{
			//Skip if the outer radix range is empty
			if (order[i].count == 0) continue;

			//Sort all the [outer,i] groups
			sort_byte1(order[i].byte1);

			//Update all the [n,byte1] segments and set the caches
			update(order[i].byte1);
		}
		b = this->blocks;
	}
};*/

//Offset sort recursively sorts blocks of a string by block offset (uses caches)
/**
 * Start with an initial 2-byte radix sort
 * Use a cache to record blocks that have been terminally placed
 * Process each sub block using std::sort and a custom comparator that looks for cache hits
 * Use sub-block sorts to reduce complexity of main block sorts
 *
struct CachedBlockSort
{
	static const int LETTERS = 256;

	vector<int> blocks;
	vector<int> cache;
	stack<int> ranges;
	string sequence;
	int length;

	//Comparator object for comparing sequence blocks
	struct Comparator
	{
		//Internal pointers required because internal comparator cannot access class scope direcly
		vector<int> * blocks;
		vector<int> * cache;
		string * sequence;
		int length;
		int offset;

		bool operator() (int x1, int x2)
		{
			x1 += offset;
			x2 += offset;
			
			if (x1 >= length) x1 -= length;
			if (x2 >= length) x2 -= length;
			
			if ((*sequence)[x1] != (*sequence)[x2])
			{
				return (*sequence)[x1] < (*sequence)[x2];
			}
			else if ((*cache)[x1] == -1)
			{
				return false;
			}
			else if ((*cache)[x2] == -1)
			{
				return (*cache)[x1] != -1;
			}
			else 
			{
				return (*cache)[x1] < (*cache)[x2];
			}
		}
	} comparator;
	
	//Advance an offset until the character between all the offsets in a range is different OR all are cached
	int advance(int x1, int x2, int offset)
	{
		x1 += offset; if (x1 >= length) x1 -= length;
		x2 += offset; if (x2 >= length) x2 -= length;
	
		for (int i=0; i<length; ++i)
		{
			bool cached = true;
			
			int pos = blocks[x1] + offset; if (pos >= length) pos -= length;
			char c = sequence[pos];
			
			for (int j=x1; j<=x2; ++j)
			{
				pos = blocks[j] + offset; if (pos >= length) pos -= length;

				if (cached)
				{
					if (cache[pos] == -1) cached = false;
				}
				if (sequence[pos] != c) return offset;
			}
			if (cached) return offset;
			offset++;
		}
		return offset;
	}
	
	//Compare two blocks from the specified offset onwards
	bool compare_blocks(int x1, int x2, int offset)
	{
		x1 = blocks[x1] + offset; if (x1 >= length) x1 -= length;
		x2 = blocks[x2] + offset; if (x2 >= length) x2 -= length;

		for (int i=offset; i<length; ++i)
		{
			if (sequence[x1] != sequence[x2])
			{
				return sequence[x1] < sequence[x2];
			}
			if (cache[x1] >= 0 && cache[x2] >= 0)
			{
				return cache[x1] < cache[x2];
			}
			if (++x1 == length) x1 = 0;
			if (++x2 == length) x2 = 0;
		}
		return false;
	}
	
	//Swap two blocks
	void swap_blocks(int x1, int x2)
	{
		if (x1 == x2) return;
		int temp = blocks[x1];
		blocks[x2] = blocks[x1];
		blocks[x1] = temp;
	}	
	
	//Sort two blocks
	void sort_two(int x1, int x2, int offset)
	{
		if (!compare_blocks(x1,x2,offset))
		{
			swap_blocks(x1,x2);
		}
		cache[blocks[x1]] = x1;
		cache[blocks[x2]] = x2;
	}

	//Process the top element on the range stack
	void sort_range(int x1, int x2, int offset)
	{
		//Shortcut if the range is 1
		if (x2 - x1 == 0)
		{
			cache[blocks[x1]] = x1;
			return;
		}
		
		//Shortcut if the range is 2
		if (x2 - x1 == 1)
		{
			sort_two(x1,x2,offset);
			return;
		}
		
		//Sort at the offset to produce blocks sorted by letter and by cache (within each letter group)
		vector<int>::iterator itr1 = blocks.begin();
		vector<int>::iterator itr2 = blocks.begin();

		itr1 += x1;
		itr2 += x2 + 1;

		comparator.offset = offset;
		std::sort(itr1,itr2,comparator);

		//Loop through the sorted group and write each letter range to the stack
		int pos = blocks[x1] + offset;
		if (pos >= length) pos -= length;
		char c = sequence[pos];
		int start = x1;
		bool cached = true;
		
		for (int i=x1; i<=x2; ++i)
		{
			//Continue if the letter group has not changed
			pos = blocks[i] + offset;
			if (pos >= length) pos -= length;
			if (sequence[pos] == c)
			{
				if (cache[pos] == -1) cached = false;
				continue;
			}
			
			//Write the range to the stack if it needs sorting
			if (!cached && i-start > 1)
			{
				//ranges.push(advance(x1,x2,offset+1));
				ranges.push(offset+1);
				ranges.push(i-1);
				ranges.push(start);
			}
			//If all the entries were cached at the current offset, then cache the blocks
			else
			{
				for (int j=start; j<i; ++j)
				{
					cache[blocks[j]] = j;
				}
			}
			
			//Reset group variables
			cached = true;
			start = i;
			c = sequence[pos];
		}
		
		//Put the final element on the stack
		if (!cached && x2 - start > 0)
		{
			//ranges.push(advance(x1,x2,offset+1));
			ranges.push(offset+1);
			ranges.push(x2);
			ranges.push(start);
		}
		else
		{
			for (int j=start; j<=x2; ++j)
			{
				cache[blocks[j]] = j;
			}
		}
	}
	
	//Main sorting routine
	void sort(string &s, vector<int> &b)
	{
		//Initialize the object
		blocks   = b;
		sequence = s;
		length   = s.size();

		comparator.sequence = &sequence;
		comparator.cache = &cache;
		comparator.blocks = &blocks;
		comparator.length = length;

		blocks.clear();
		blocks.resize(length);

		cache.clear();
		cache.resize(length);
		
		//Initialize the blocks
		for (int i=0; i<length; ++i)
		{
			blocks[i] = i;
			cache[i] = -1;
		}
		
		//The intial range covers the entire table
		ranges.push(0);
		ranges.push(length-1);
		ranges.push(0);

		cout << "Sorting" << endl;
		
		//Sort ranges until the range stack is empty
		while (!ranges.empty())
		{
			int x1 = ranges.top(); ranges.pop();
			int x2 = ranges.top(); ranges.pop();
			int offset = ranges.top(); ranges.pop();

			sort_range(x1, x2, offset);
		}
		b = this->blocks;
	}
};

*/



/**
 * Divide and conquer merge sort applied to the Burrows-Wheeler Transformation. 
 * A 'block' consists of the entire original string starting from a specific 
 * index. The string is wrapped when the end is encountered.
 *
 * Groups of blocks are merged in log(n) operations (where n is the length of the
 * original string). In the end there is one group in which all blocks are sorted.
 * Sorted blocks are stored as a linked list with an additional piece of information
 * which is the point of first difference between a block and its predecessor.
 *
 * To conserve memory the linked list of blocks also doubles as a linked list of 
 * groups.
 */
struct BlockSort
{
	struct block
	{
		unsigned int index; //index of this block
		//unsigned int gnext; //index of the next group
		//unsigned int gprev; //index of the prev group
		unsigned int next;  //closest next block
		unsigned int prev;  //closest prev block
		unsigned int diff;  //first difference between this block and prev block
	};

	const char * sequence;
	unsigned int length;
	unsigned int BLANK;
	vector<bool> flags;
	vector<block> blocks;
	vector<unsigned int> heads;

	block * gfirst;
	
	//Debugging
	unsigned int splices;
	unsigned int nulls;
	unsigned int appends;
	unsigned int aborts;
	unsigned int resolves;
	unsigned int cascades;
	unsigned int cascades2;
	unsigned int orphans;
	unsigned int quicks;
	unsigned int groups;
	unsigned int bcount;
	unsigned int ccount;
		
	bool debug;
	
	//Get a pointer to a block
	block * _b(unsigned int index)
	{
		//assert(index <= length);
		++bcount;
		return (index == length) ? NULL : &(blocks[index]);
	}
	
	char _c(unsigned int index)
	{
		//assert(index < length);
		++ccount;
		return sequence[index];
	}
	
	//Print out information about a block
	void dump(block * q)
	{
		if (q == NULL)
		{
			cout << "Unknown block: " << q->index << endl;
		}
		else
		{
			cout << blockseq(q->index) << ' ' << q->prev << " <- " << q->index << " -> " << q->next << " (" << q->diff << ") ";

			if (q->prev != BLANK)
			{
				if (_b(q->prev)->next != q->index) cout << '*';
			}
			cout << endl;
		}
	}
	
	void matrix()
	{
		// cout << "--------------------" << endl;
		// block * p = gfirst;
		// 
		// for (block * p = gfirst; p != NULL; p = _b(p->gnext))
		// {
		// 	for (block * q = p; q != NULL; q = _b(q->next))
		// 	{
		// 		dump(q);
		// 	}
		// 	cout << endl;
		// }		
		// cout << "--------------------" << endl;
	}
	
	//Get the sequence for a particular block
	string blockseq(unsigned int index)
	{
		string s = "";
		
		for (unsigned int i=0; i<length; ++i)
		{
			if (index == length) index = 0;
			s += sequence[index];
			index++;
		}
		return s;
	}	

	//Initialize the object
	void init(string &s)
	{
		debug = false;
		length = s.size();
		BLANK = length;
		sequence = s.c_str();
				
		blocks.resize(length);
		flags.resize(length);
		heads.resize(length);
		
		for (int i=0; i<length; ++i)
		{
			blocks[i].index = i;
			blocks[i].next  = BLANK;
			blocks[i].prev  = BLANK;
			blocks[i].diff  = BLANK;
			
			//blocks[i].gnext = i+1;
			//blocks[i].gprev = i-1;

			flags[i] = false;
			
			heads[i] = i;
		}
		// blocks[0].gprev = BLANK;
		// blocks[length-1].gnext = BLANK;
		// 
		// gfirst = _b(0);
	}
	
	// //Remove a group
	// void remove(block * b)
	// {
	// 	//assert(b != NULL);
	// 	//assert(b->gprev != BLANK || b->gnext != BLANK);
	// 	
	// 	if (b->gprev != BLANK) _b(b->gprev)->gnext = b->gnext;
	// 	if (b->gnext != BLANK) _b(b->gnext)->gprev = b->gprev;
	// 	
	// 	if (b == gfirst) gfirst = _b(b->gnext);
	// 	
	// 	b->gprev = BLANK;
	// 	b->gnext = BLANK;
	// 	
	// 	//if (debug) cout << " - remove " << b->index << endl;
	// 	////if (debug) matrix();
	// }
	
	// //Replace a group with another one
	// void replace(block * old, block * fresh)
	// {
	// 	if (old == fresh) return;
	// 	
	// 	//assert(old != NULL && fresh != NULL);
	// 	//assert(old != fresh);
	// 	//assert(fresh != gfirst);
	// 	//assert(old->gprev != BLANK || old->gnext != BLANK);
	// 	
	// 	if (fresh->gprev != BLANK || fresh->gnext != BLANK) remove(fresh);
	// 
	// 	fresh->gnext = old->gnext;
	// 	fresh->gprev = old->gprev;
	// 	
	// 	if (old->gprev != BLANK) _b(old->gprev)->gnext = fresh->index;
	// 	if (old->gnext != BLANK) _b(old->gnext)->gprev = fresh->index;
	// 	
	// 	if (old == gfirst) gfirst = fresh;
	// 	
	// 	old->gnext = BLANK;
	// 	old->gprev = BLANK;
	// 
	// 	//if (debug) cout << " - replace " << old->index << " with " << fresh->index << endl;
	// 	////if (debug) matrix();		
	// }
	
	//Set the winner between two blocks. True if {a} wins.
	bool resolve(block * a, block * b)
	{
		//Blocks must point to the same parent
		//assert(a != b);
		//assert(a->prev == b->prev);
		
		//Auto win to the bigger up pointer
		if (a->diff != b->diff)
		{
			//assert(a->diff != BLANK);
			//assert(b->diff != BLANK);
		
			if (a->diff > b->diff)
			{
				b->prev = a->index;
				return true;
			}
			a->prev = b->index;
			return false;				
		}
		
		//Find the point of first difference
		unsigned int diff = (a->diff == BLANK) ? 0 : a->diff;

		unsigned int p1 = a->index + diff;
		unsigned int p2 = b->index + diff;
		
		if (p1 >= length) p1 -= length;
		if (p2 >= length) p2 -= length;

		//Not faster to try to take a shortcut looking for a<-->b
		while (diff < length)
		{
			++resolves;

			if (_c(p1) != _c(p2))
			{
				if (_c(p1) < _c(p2))
				{
					if (diff - b->diff > 2) flags[b->index] = true;
					b->diff = diff;
					b->prev = a->index;
					return true;
				}
				if (diff - a->diff > 2) flags[a->index] = true;
				a->diff = diff;
				a->prev = b->index;
				return false;
			}
			
			//Advance to the next character
			if (++p1 == length) p1 = 0;
			if (++p2 == length) p2 = 0;
			++diff;
		}

		//Should not be possible to get here
		//FIXME: deal with this by keeping a pseudo element (end of string)
		cout << "Whole string is a repeat" << endl;
		//assert(false);
	}
	
	//When the diff pointer of a newly placed read has been advanced then cascade upstream and downstream
	//Cascade identifies the bounds of the repeated segment and updates all character pairs
	void cascade(block * z)
	{
		//No need to do anything if not flagged
		if (!flags[z->index]) return;
		
		//Unflag the block
		flags[z->index] = false;
		
		//Shouldn't be possible for a flagged block to point nowhere
		//assert(_b(z->prev) != NULL);
		//assert(z->diff != BLANK);

		unsigned int p1 = z->prev;
		unsigned int p2 = z->index;
		unsigned int diff = z->diff;
		
		//if (debug) cout << " - cascade " << p2 << " to " << p1 << endl;
		////if (debug) matrix();
		
		//Backtrack to the start of the matching segment
		while (_c(p1) == _c(p2))
		{
			//Think of diagonal reverse plots
			// - if the backtrace is on the same line then abort (handle it later)
			// - if the backtrace is higher, then update all, assert impossible to be better
			// - corrugation is dealt with later
			if (p2 != z->index)
			{
				//Abort if it's not an improvement
				if (_b(p2)->diff == diff)
				{
					if (_b(p2)->prev != p1)
					{
						break;
					}
				}
			}

			if (p1 == 0) p1 = length;
			if (p2 == 0) p2 = length;
			
			--p1; --p2; ++diff; ++cascades2;
		}

		//Update all pairs through to the end of the matching segment
		while (diff != 0)
		{
			if (++p1 == length) p1 = 0;
			if (++p2 == length) p2 = 0;

			--diff; ++cascades;
			
			//if (diff != 0) assert(_c(p1) == _c(p2));
			//if (diff == 0) assert(_c(p1) != _c(p2));

			//if (debug) cout << "   - " << p2 << "->" << p1 << endl;
			
			//Sanity check
			if (p2 == z->index)
			{
				//assert(diff == z->diff);
				continue;
			}
			
			//Sanity check
			// if (flags[p2])
			// {
			// 	//assert(_b(p2)->prev == z->index);
			// }

			//Update if blank or worse
			if (_b(p2)->diff == BLANK || _b(p2)->diff < diff)
			{
				//If the block happens to be part of an active resolve, then unflag it (if set)
				//There is no need to cascade it because THIS is it's cascade
				if (_b(p2)->prev == z->index)
				{
					flags[p2] = false;
				}
				
				//Update the loser in the pair
				_b(p2)->diff = diff;
				_b(p2)->prev = p1;

				//if (debug) cout << "    - update " << p2 << " to " << p1 << endl;
				////if (debug) matrix();

				continue;
			}
			
			//If the diff is > then a previous cascade SHOULD have resolved it (abort)
			if (_b(p2)->diff >= diff)
			{
				//Don't abort if the diff has recently been set during a resolve()
				if (flags[p2])
				{
					continue;
				}
				
				//Sanity check. Superior diff cannot occur upstream, only downstream
				//if (_b(p2)->diff > diff) //assert(diff < z->diff);
				
				// //Sanity check. Ensure superior relationship holds true at all downstream positions
				// while (diff != 0)
				// {
				// 	//assert(_b(p2)->diff >= diff);
				// 	if (++p1 == length) p1 = 0;
				// 	if (++p2 == length) p2 = 0;
				// 
				// 	--diff;
				// }
				break;
			}
			
			//If the diff is == then abort
			if (_b(p2)->diff == diff)
			{
				//Don't abort if the diff has recently been set during a resolve()
				if (flags[p2])
				{
					continue;
					//flags[p2] = false;
				}
				break;
			}
		}
	}
	
	//Append a block {b} to the tail of a merged list {z} and return the displaced read (if there is one)
	void append(block * b, block * z)
	{
		//if (debug) cout << " - append " << b->index << " to " << z->index << " (" << b->diff << ")" << endl;
		
		if (z->next != b->index)
		{
			z->next = b->index;
			
			////assert(b->prev == z->index);
			if (b->prev == z->index)
			{
				cascade(b);
			}
		}
		//assert(!flags[b->index]);
		////if (debug) matrix();
		++appends;
	}
	
	//Merge group B into group A (A is defined by what B points to)
	unsigned int merge(block * group, unsigned int bin)
	{
		//assert(group != NULL);
		//assert(group->gprev != BLANK || group->gnext != BLANK);
				
		block * b = group;       //assert(b != NULL); //head element from group B
		block * z = _b(b->prev); //assert(z != NULL); //tail element from AB
		block * a = _b(z->next); //head element from group A

		//assert(a != b);
		
		//if (debug) cout << "Merge " << group->index << " into " << z->index << endl;
		
		//assert(adjacent(z->index, b->index, bin));
		
		//Merge group B into group A
		while (a != NULL && b != NULL)
		{
			////if (debug) cout << "Resolving " << a->index << " and " << b->index << endl;
			
			//Splice if {b} has a crossed pointer
			if (b->prev != z->index)
			{
				if (adjacent(b->prev, b->index, bin))
				{
					//if (debug) cout << "Splice " << b->index << " to " << b->prev << endl;
					append(a,z);
					z = _b(b->prev);
					a = _b(z->next);
					//assert(a != b);
					++splices;
					continue;
				}
				else
				{
					append(a,z);
					//replace(group,b);
					return b->index;
				}
			}

			//Splice if {a} has a crossed pointer
			if (a->prev != z->index)
			{
				if (adjacent(a->prev, a->index, bin))
				{
					//if (debug) cout << "Splice " << a->index << " to " << a->prev << endl;
					append(b,z);
					z = _b(a->prev);
					b = a;
					a = _b(z->next);
					//assert(a != b);
					++splices;
					continue;
				}
				else
				{
					append(b,z);
					//replace(group,a);
					return a->index;
				}
			}
			
			//assert(a->prev == z->index);
			//assert(b->prev == z->index);

			//Find the better of the two reads
			if (resolve(a,b))
			{
				b->prev = a->index;
				append(a,z);
				z = a;
				a = _b(a->next);
			}
			else
			{
				a->prev = b->index;
				append(b,z);
				z = b;
				b = _b(b->next);
			}
		}
		
		//To get here, one (but not both) of the groups must be empty
		//assert(a == NULL || b == NULL);
		//assert(a != NULL || b != NULL);
		
		//Append the non-null group
		//if (debug) cout << "Empty " << endl;
		append(a == NULL ? b : a, z);
		//remove(group);
		++nulls;
		return BLANK;
	}

	//Determine if two reads are in adjacent bin groups
	inline bool adjacent(unsigned int a, unsigned int b, unsigned int bin)
	{
		//assert(a != b);
		//assert(a != BLANK);
		//assert(b != BLANK);
		
		a /= bin;
		b /= bin;

		a /= 2;
		b /= 2;
		
		if (a != b) aborts++;
		
		return a == b;
	}
	
	//Merge sort using an array instead of a list
	void sort(string &s, vector<int> &bks)
	{
		//Initialize
		init(s);
		
		cout << "Total length: " << length << endl;
		
		unsigned int end = length;
	
		//For each n^2 iteration
		for (unsigned int bin = 1; bin < length; bin *= 2)
		{
			cout << "Sorting iteration " << bin << ": " << flush;
	
			//Walk through the list of groups and merge as many as possible
			splices = 0;
			nulls = 0;
			appends = 0;
			aborts = 0;
			resolves = 0;
			cascades = 0;
			cascades2 = 0;
			orphans = 0;
			quicks = 0;
			groups = 0;
			ccount = 0;
			bcount = 0;
		
			unsigned int npos = 0;
			unsigned int hpos = 0;

			block * node = NULL;
			block * hold = NULL;
			
			for (npos = 0; npos < end; ++npos)
			{
				node = _b(heads[npos]);
				
				//assert(node != NULL);
				
				//If the node has a trace then follow it immediately
				if (node->prev != BLANK)
				{
					//if (debug) cout << "Trace " << node->index << endl;
					
					if (adjacent(node->index, node->prev, bin))
					{
						++quicks;
						heads[npos] = merge(node,bin);
					}
					else
					{
						//if (debug) cout << "Aborted merge as groups are not adjacent " << node->index << ' ' << node->prev << ' ' << bin << endl;
					}
				}
				//Put the group on hold if possible (orphan)
				else if (hold == NULL)
				{
					//if (debug) cout << "Hold " << node->index << endl;
					hold = node;
					hpos = npos;
				}
				//Find the winner of two orphans and merge
				else
				{
					//Catch a condition where cascade has given the held group a parent
					if (hold->prev != BLANK)
					{
						if (adjacent(hold->index, hold->prev, bin))
						{
							heads[hpos] = merge(hold,bin);
							++quicks;
						}
						hold = node;
						hpos = npos;
					}
					else
					{
						if (adjacent(node->index, hold->index, bin))
						{
							//if (debug) cout << "Pair " << node->index << endl;
							++orphans;
							
							if (resolve(hold,node))
							{
								heads[npos] = merge(node,bin);
							}
							else
							{
								heads[hpos] = merge(hold,bin);
							}
							hold = NULL;
						}
						else
						{
							//if (debug) cout << "Aborted merge as groups are not adjacent" << endl;
							hold = node;
							hpos = npos;
						}
					}
				}
			}
			
			//Rewrite, count and improve the master list
			//TODO: find a way to incorporate this in the above list
			unsigned int move = 0;
			unsigned int temp = 0;
			
			for (unsigned int i=0; i<end; ++i)
			{
				if (heads[i] == BLANK) continue;
				groups++;
				temp = heads[i];
				heads[i] = BLANK;
				heads[move] = temp;
				++move;
			}
			end = move;
		
			cout << " Groups: " << groups;
			cout << " B:" << (bcount/1000000);
			cout << " C:" << (ccount/1000000);
			cout << " Appends:" << (appends/1000000);
			cout << " Resolves:" << (resolves/1000000);
			cout << " Cascades:" << (cascades/1000000);
			cout << "+" << (cascades2/1000000);
			cout << " Splices:" << (splices/1000000);
			cout << " Aborts:" << (aborts/1000000);
			cout << " Orphans:" << (orphans/1000000);
			cout << " Quicks:" << (quicks/1000000);
			cout << endl;
		}
	
		bks.resize(length);

		//Build the final collection of sorted blocks
		unsigned int i = 0;
	
		for (block * p = _b(heads[0]); p != NULL; p = _b(p->next), ++i)
		{
			bks[i] = p->index;
		}
			
		//if (debug) matrix();
		cout << "Finished" << endl;
	}
};




/*
TODO:
- in place list for managing the lists
- must have a way to replace groups
- must have a way to delete groups
- list makes sense, but array uses less memory
- array is harder to allocate
- delete can be lazy, removing the groups with a condition check on the next pass
- a group would be set for deletion if the prev's next == group index (already integrated)
- problem might arise if an integrated group splices, then it would be seen as a group (instead of being deleted)
- best way would be to just set the group value to something else on delete or replace
- use delete or replace to set the group number properly. Lazy kill/copy on the next iteration

- on the sort process, walk the array
	- if the element is flagged for kill, write over it with the lext active group (when found)
	- two travelling pointers, A to the left group, B to the right
	- if a pointer hits a linked group, it immediately follows
	- pointer merge is only done when the two groups are both orphans
	
	
	
- Dynamic rewriting to move elements as they need it
- Don't do the rewrite after the traverse, do it as it proceeds
- A group with a trace will either return:
	- blank if fully integrated
	- a tail if spliced beyond the current bin group
	
- If the return result is not a blank then the result should be written to the trail position and the trail advanced.

- When comparing two orphans, there may be no write needed if there is a tail left
*/

// //Merge sort the blocks
// void sort_original(string &s, vector<int> &bks)
// {
// 	//Initialize
// 	init(s);
// 	
// 	// if (debug)
// 	// {
// 	// 	for (int i=0; i<length; ++i)
// 	// 	{
// 	// 		cout << i << " - " << blockseq(i) << endl;
// 	// 	}
// 	// }
// 	
// 	//For each n^2 iteration
// 	unsigned int bin = 1;
// 	
// 	cout << "Total length: " << length << endl;
// 	
// 	while (gfirst->gnext != BLANK)
// 	{
// 		cout << "Sorting iteration " << bin << ": " << flush;
// 		//if (debug) matrix();
// 		
// 		splices = 0;
// 		nulls = 0;
// 		appends = 0;
// 		aborts = 0;
// 		resolves = 0;
// 		cascades = 0;
// 		cascades2 = 0;
// 		orphans = 0;
// 		quicks = 0;
// 		groups = 0;
// 		ccount = 0;
// 		bcount = 0;
// 		
// 		block * node = NULL;
// 		block * next = NULL;
// 		block * hold = NULL;
// 
// 		for (node = gfirst; node != NULL; node = _b(node->gnext))
// 		{
// 			groups++;
// 		}
// 		
// 		for (node = gfirst; node != NULL; node = next)
// 		{
// 			next = _b(node->gnext);
// 			
// 			//Merge immediately if there is a trace (not an orphan)
// 			if (node->prev != BLANK)
// 			{
// 				//if (debug) cout << "Trace " << node->index << endl;
// 				
// 				if (adjacent(node->index, node->prev, bin))
// 				{
// 					++quicks;
// 					merge(node,bin);
// 				}
// 				else
// 				{
// 					//if (debug) cout << "Aborted merge as groups are not adjacent " << node->index << ' ' << node->prev << ' ' << bin << endl;
// 				}
// 			}
// 			//Put the group on hold if possible (orphan)
// 			else if (hold == NULL)
// 			{
// 				//if (debug) cout << "Hold " << node->index << endl;
// 				hold = node;
// 			}
// 			//Find the winner of two orphans and merge
// 			else
// 			{
// 				//assert(node->gnext != BLANK || node->gprev != BLANK);
// 				//assert(hold->gnext != BLANK || hold->gprev != BLANK);
// 
// 				//Catch a condition where cascade has given the held group a parent
// 				if (hold->prev != BLANK)
// 				{
// 					next = hold;
// 					hold = NULL;
// 				}
// 				else
// 				{
// 					if (adjacent(node->index, hold->index, bin))
// 					{
// 						//if (debug) cout << "Pair " << node->index << endl;
// 						++orphans;
// 						
// 						//assert(node->prev == BLANK);
// 						//assert(hold->prev == BLANK);
// 						
// 						if (resolve(hold,node))
// 						{
// 							merge(node,bin);
// 							next = hold;
// 						}
// 						else
// 						{
// 							merge(hold,bin);
// 							next = node;
// 						}
// 						hold = NULL;
// 					}
// 					else
// 					{
// 						//if (debug) cout << "Aborted merge as groups are not adjacent" << endl;
// 						hold = node;
// 					}
// 				}
// 			}
// 		}
// 		
// 		//Increase the bin size
// 		bin = (length / 2 > bin) ? 2*bin : length;
// 		
// 		cout << " Groups: " << groups;
// 		cout << " B:" << (bcount/1000000);
// 		cout << " C:" << (ccount/1000000);
// 		cout << " Appends:" << (appends/1000000);
// 		cout << " Resolves:" << (resolves/1000000);
// 		cout << " Cascades:" << (cascades/1000000);
// 		cout << "+" << (cascades2/1000000);
// 		cout << " Splices:" << (splices/1000000);
// 		cout << " Aborts:" << (aborts/1000000);
// 		cout << " Orphans:" << (orphans/1000000);
// 		cout << " Quicks:" << (quicks/1000000);
// 		cout << endl;
// 	}
// 	//assert(gfirst != NULL);
// 	////assert(gfirst->gnext == BLANK);
// 	
// 	bks.resize(length);
// 
// 	unsigned int i = 0;
// 	
// 	for (block * p = gfirst; p != NULL; p = _b(p->next), ++i)
// 	{
// 		bks[i] = p->index;
// 	}
// 	
// 	//if (debug) matrix();
// 	cout << "Finished" << endl;
// }


