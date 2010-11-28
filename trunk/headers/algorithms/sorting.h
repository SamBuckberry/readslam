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


// /**
//  * Basically a merge-sort that uses tail end recursion
//  * 'Up' pointers are used to mark diagonal threads and accelerate the merge step
//  */
// struct CrossThreadSort
// {
// 	string sequence;
// 	int length;
// 	queue<int> copy;
// 	vector<int> blocks;
// 	vector<int> ups;
// 	vector<int> winners;
// 	vector<int> anchors;
// 	
// 	int calls;
// 	int advances;
// 	int iteration;
// 	int shortcuts;
// 	
// 	bool verbose;
// 	
// 	//Initialize the object
// 	void init(string &s, vector<int> &b)
// 	{
// 		verbose = false;
// 		
// 		blocks   = b;
// 		sequence = s;
// 		length   = s.size();
// 
// 		blocks.clear();
// 		blocks.resize(length);
// 		
// 		ups.clear();
// 		ups.resize(length);
// 		
// 		winners.clear();
// 		winners.resize(length);
// 		
// 		anchors.clear();
// 		anchors.resize(length);
// 		
// 		calls = 0;
// 		advances = 0;
// 		
// 		//Initialize the blocks
// 		for (int i=0; i<length; ++i)
// 		{
// 			blocks[i] = i;
// 			ups[i] = -1;
// 			winners[i] = -1;
// 			anchors[i] = -1;
// 		}
// 	}	
// 	
// 	//Print out the stacks to the screen
// 	void dump(int x1, int x2, int end)
// 	{
// 		int len = (length > 80) ? 80 : length;
// 		
// 		for (int i=x1; i<x2; ++i)
// 		{
// 			cout << blocks[i] << " : " << ups[blocks[i]] << " : ";
// 			for (int j=0; j<len; ++j)
// 			{
// 				int p = blocks[i] + j; if (p >= length) p -= length;
// 				cout << sequence[p];
// 			}
// 			cout << endl;
// 		}
// 		cout << "-------------------" << endl;
// 		for (int i=x2; i<=end; ++i)
// 		{
// 			cout << blocks[i] << " : " << ups[blocks[i]] << " : ";
// 			for (int j=0; j<len; ++j)
// 			{
// 				int p = blocks[i] + j; if (p >= length) p -= length;
// 				cout << sequence[p];
// 			}
// 			cout << endl;
// 		}
// 	}
// 	
// 	//Compare two blocks (specified by string position) at a specific offset for ordering
// 	int compare(int x1, int x2, int offset)
// 	{
// 		x1 += offset; if (x1 >= length) x1 -= length;
// 		x2 += offset; if (x2 >= length) x2 -= length;
// 		
// 		if      (sequence[x1] < sequence[x2]) return -1;
// 		else if (sequence[x1] > sequence[x2]) return  1;
// 		else                                  return  0;
// 	}
// 	
// 	//Find the first point of difference (starting from an offset) between two blocks (specified by block start positions)
// 	int differ(int x1, int x2, int offset)
// 	{
// 		x1 += offset; if (x1 >= length) x1 -= length;
// 		x2 += offset; if (x2 >= length) x2 -= length;
// 	
// 		while (offset < length)
// 		{
// 			if (sequence[x1] != sequence[x2]) return offset;
// 			
// 			if (++x1 == length) x1 = 0;
// 			if (++x2 == length) x2 = 0;
// 			
// 			offset++;
// 		}
// 		cout << "ERROR, entire block is a repeat" << endl;
// 		return -1;
// 	}
// 	
// 	//Merge sort the blocks
// 	void sort(string &s, vector<int> &b)
// 	{
// 		//Initialize
// 		init(s,b);
// 		
// 		//For each n^2 iteration
// 		for (int bin=1; bin<length; bin*=2)
// 		{
// 			cout << "Sorting iteration: " << bin << endl;
// 			calls = 0; advances = 0; shortcuts = 0; iteration = bin;
// 			
// 			//Clear out the winners table
// 			for (int i=0; i<length; ++i)
// 			{
// 				winners[i] = -1;
// 				anchors[i] = -1;
// 			}
// 			
// 			//Merge each pair of bins
// 			for (int start=0; start<length; start+=2*bin)
// 			{
// 				//Determine the sizes of the two bins
// 				int sizeA = bin;
// 				int sizeB = bin;
// 				
// 				//Adjust bin B if necessary
// 				if (start + 2*bin > length)
// 				{
// 					sizeB = length - (start + bin);
// 					
// 					//No merge necessary if B is empty. Remember to reset the flag (this is the odd bin at the end)
// 					if (sizeB <= 0)
// 					{
// 						ups[blocks[start]] = -1;
// 						continue;
// 					}
// 				}
// 				
// 				//Check if the lead indicator is set
// 				int firstdiff = ups[blocks[start]];
// 				
// 				//Determine the point of first difference if required
// 				if (firstdiff == -1)
// 				{
// 					//Find the first point of difference between the parent sequences of the squares
// 					firstdiff = differ(start, start+bin, 0);
// 				
// 					//Set as many 'up' pointers as possible for this iteration (downstream)
// 					for (int i=0; i<=firstdiff; i+=2*bin)
// 					{
// 						if (start+i >= length) break;
// 						ups[blocks[start+i]] = firstdiff - i;
// 					}
// 				}
// 				
// 				//Set this offset as the 'up' pointer for the lead blocks
// 				ups[blocks[start]]     = firstdiff;
// 				ups[blocks[start+bin]] = firstdiff;
// 				
// 				if (verbose)
// 				{
// 					cout << "MERGE: " << iteration << "[" << start << "," << (start+bin) << "][" << sizeA << "," << sizeB << "]" << endl;
// 					dump(start, start+bin, start+(sizeA+sizeB)-1);
// 				}
// 
// 				//Merge the blocks taking a shortcut if possible
// 				if (firstdiff < bin)
// 				{
// 					//Bin sequence is not identical. Squares are not identical
// 					merge(start, sizeA, sizeB);
// 				}
// 				else if (firstdiff < 3*bin)
// 				{
// 					//Bin sequence is identical. Squares are not identical
// 					merge(start, sizeA, sizeB);
// 					// merge(start, sizeA, sizeB, offset);
// 				}
// 				else 
// 				{
// 					//Bin sequence is identical. Squares are identical
// 					merge(start, sizeA, sizeB);
// 					if (verbose) cout << "quick" << endl;
// 					
// 					//quickmerge(start, sizeA, sizeB, firstdiff);
// 				}
// 
// 				//Flush the merge queue
// 				flush(start,sizeA+sizeB);
// 				
// 				
// 				//Mark the current lead indicator for NEXT iteration if possible
// 				if (firstdiff >= 2*bin)
// 				{
// 					ups[blocks[start]] = firstdiff;
// 				}
// 
// 				// :: SHORTCUT IN PARENT MERGE
// 				// ----a-------b----------------------------------------------------
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a
// 				// 
// 				// ------------a-------b--------------------------------------------
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a
// 				// 
// 				// --------------------a-------b------------------------------------
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a, C->a
// 				// 
// 				// ----------------------------a-------b----------------------------
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a, C->a
// 				// 
// 				// ------------------------------------a-------b--------------------
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a, C->a, E->a
// 				// 
// 				// --------------------------------------------a-------b------------
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a, C->a, E->a
// 				// 
// 				// ----------------------------------------------------a-------b----
// 				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
// 				// A->a, C->a, E->a, G->a
// 			}
// 			cout << "Calls:" << calls << " Advances:" << advances << " Shortcuts: " << shortcuts << endl;
// 		}
// 
// 		//Set the sorted blocks
// 		b = blocks;
// 	}
// 	
// 	//This merge can be used when both SQUARES are identical (A==B==C)
// 	void quickmerge(int start, int sizeA, int sizeB, int offset)
// 	{
// 		int posA  = start;
// 		int posB  = start + sizeA;
// 		int bin   = sizeA;
// 		int size  = sizeA + sizeB;
// 		
// 		//Determine the best element
// 		bool abest = (compare(start, start+bin, offset) == -1);
// 		
// 		//Merge the two ranges into the queue
// 		for (int i=0; i<size; ++i)
// 		{
// 			//If a block is empty, flush the other and break
// 			if (sizeA == 0) { while (i<size) { copy.push(blocks[posB]); posB++; i++; } break; }
// 			if (sizeB == 0) { while (i<size) { copy.push(blocks[posA]); posA++; i++; } break; }
// 
// 			//Get the blocks and their 'up' pointers
// 			int blockA = blocks[posA];
// 			int blockB = blocks[posB];
// 			int upA = ups[blockA];
// 			int upB = ups[blockB];
// 			
// 			//If one of the offsets is longer then it's an instant win
// 			if (upA > upB) { copy.push(blockA); posA++; sizeA--; continue; }
// 			if (upB > upA) { copy.push(blockB); posB++; sizeB--; continue; }
// 
// 			//Sanity check
// 			if (posB - posA == bin)
// 			{
// 				//The winner is known from the start
// 				if (abest) { copy.push(blockA); ups[blockB] = offset - (blockB - (start+bin)); posA++; sizeA--; }
// 				else       { copy.push(blockB); ups[blockA] = offset - (blockA - start);       posB++; sizeB--; }
// 			}
// 			else
// 			{
// 				cout << "ERROR: this situation should be impossible" << endl;
// 			}
// 		}
// 	}
// 	
// 	//The crossthread-merge approach. Sorts based on offsets. Reliable, but slow due to forward lookups when offsets match.
// 	void merge(int start, int sizeA, int sizeB)
// 	{
// 		int posA = start;
// 		int posB = start + sizeA;
// 		int size = sizeA + sizeB;
// 		int bin  = sizeA;
// 
// 		//Set the initial offset for the elements: FIXME <- this can be set externally
// 		ups[blocks[posA]] = 0;
// 		ups[blocks[posB]] = 0;
// 		
// 		//Merge the two ranges into the queue
// 		for (int i=0; i<size; ++i)
// 		{
// 			//If a block is empty, flush the other and break
// 			if (sizeA == 0) { while (i<size) { copy.push(blocks[posB]); posB++; i++; } break; }
// 			if (sizeB == 0) { while (i<size) { copy.push(blocks[posA]); posA++; i++; } break; }
// 			
// 			//Link a pair by setting the 'up' pointer of the lesser element
// 			bool aWins = link(blocks[posA], blocks[posB]);
// 			
// 			//Queue the better element
// 			if (aWins) { copy.push(blocks[posA]); posA++; sizeA--; }
// 			else       { copy.push(blocks[posB]); posB++; sizeB--; }
// 		}
// 	}
// 	
// 	//Compare two blocks to determine order and link the lower to the upper. Returns true if the upper is A, false if it's B
// 	bool link(int blockA, int blockB)
// 	{
// 		//Copy the offset pointers
// 		int a = ups[blockA];
// 		int b = ups[blockB];
// 		
// 		//Instant winner if the offsets are different
// 		if (a != b) return a > b;
// 		
// 		//Convert relative offsets to absolute positions
// 		a += blockA; if (a >= length) a -= length;
// 		b += blockB; if (b >= length) b -= length;
// 		
// 		//Advance to find the first point of difference
// 		for (int offset=ups[blockA]; offset<length; ++offset)
// 		{
// 			//Resolution provided when the characters differ
// 			// if (sequence[a] < sequence[b]) { ups[blockB] = offset; update_winners(blockA,blockB,offset); return true;  }
// 			// if (sequence[b] < sequence[a]) { ups[blockA] = offset; update_winners(blockB,blockA,offset); return false; }
// 			if (sequence[a] < sequence[b]) { update(blockA, blockB, offset); return true;  }
// 			if (sequence[b] < sequence[a]) { update(blockB, blockA, offset); return false; }
// 
// 			//Winners table can provide a shortcut
// 			//if (winners[a] == b) { ups[blockB] = offset + anchors[a]; update_winners(blockA,blockB,offset); return true;  }
// 			//if (winners[b] == a) { ups[blockA] = offset + anchors[b]; update_winners(blockB,blockA,offset); return false; }
// 			if (winners[a] == b) { update(blockA, blockB, offset+anchors[a]); return true;  }
// 			if (winners[b] == a) { update(blockB, blockA, offset+anchors[b]); return false; }
// 
// 			if (++a == length) a = 0;
// 			if (++b == length) b = 0;
// 			
// 			advances++;
// 		}
// 		cout << "ERROR: entire block is a repeat" << endl;
// 		exit(1);
// 	}
// 	
// 	//Update link information between two blocks and any nearby blocks that are effected
// 	void update(int winner, int loser, int offset)
// 	{
// 		//Set the link
// 		ups[loser] = offset;
// 		
// 		int start = winner;
// 		int end = winner + offset;
// 		
// 		//Update other entries in this range
// 		for (int i=0; i<=offset; ++i)
// 		{
// 			if (winners[winner] == loser) return;
// 			//if (anchors[winner] > offset - i) return;
// 			
// 			winners[winner] = loser;
// 			anchors[winner] = offset - i;
// 			
// 			if (++winner >= length) winner -= length;
// 			if (++loser  >= length) loser  -= length;
// 			
// 			advances++;
// 		}
// 	}
// 	
// 	//Flush the queue (copy sorted element back into blocks)
// 	void flush(int start, int size)
// 	{
// 		//Error check
// 		if (copy.size() != size)
// 		{
// 			cout << "Queue size is not correct" << endl;
// 			exit(1);
// 		}
// 		
// 		//Flush the queue
// 		for (int i=0; i<size; ++i)
// 		{
// 			blocks[start+i] = copy.front();
// 			copy.pop();
// 			if (verbose) cout << "set:" << i << " to " << blocks[start+i] << endl;
// 		}
// 
// 		//Reset the lead element to point to a blank (as the offset is unknown)
// 		ups[blocks[start]] = -1;
// 	}
// };
	
	
/*
:: IDEAS ::
- use a vector instead of a queue
	- the vector is full length
	- the index is the stringpos
	- the value is the order in the queue
	- each iteration, it is reset to -1
	- values are set as A and B are merged (pushed onto the queue)
	- flushing the queue means random access operations (easy enough)
	
- on an advance operation, during downstream character matching...
	- a block is flagged as 'placed' if the queue value != -1 (could also use a bool flag list)
	- the two positions may be either flagged or not
	
	:: GOOD SHORTCUT ::
	- if the offset char being looked at is < first character of block then both MUST have been cached
	- in other words, all the 'a' blocks MUST have been cached before the 'b' are, so if the char is 'a' you KNOW they are both flagged
	- if the offset char is > first char then the two blocks CANNOT have been cached
	- if the offset char is == first char then any combination is possible
	- cached,uncached OR uncached,cached is ONLY possible when the offset char == first char
	
	:: FLAGGED, FLAGGED ::
	- this MUST occur whenever the match character is < the start character
	- the block with the better queue position is the winner
	- because, effectively, these two blocks have been compared before (cached) then the offset is easy to determine :-)
	- this situation MAY occur when the match character == the start character
	
	:: FLAGGED, RAW or RAW, FLAGGED ::
	- this situation can ONLY occur when the match character == the start character
	- the winner is KNOWN, it must be the element that is already cached (that element MUST be better)
	- but how do we determine the up pointer for the loser (exactly)???
	- the up pointer for the loser MUST be somewhere between the current position and the up 
	  pointer for the winner (known from the 'up' pointer of the cached element)
	- because the cached element is the winner, it is KNOWN that the entire dominant block MUST contain winners up to that point
		- because all elements are winners in the dominant block, none need their up values adjusted
		- all values in the recessive block will need their offsets left alone or adjusted up
		- the offset can be skipped forward by looking at the cached element and tracing down the cache to the end
		- it is KNOWN that the FLAGGED/RAW situation can ONLY occur when the current offset char is the same as the parent char
		- the offset adjustment for the loser CANNOT be greater than the final element in the current cache
		- knowing that is not especially helpful because the first point of difference must still be found
		- continue to read forward to the first point of difference (or the first FLAG,FLAG situation provides the point of difference)
		- now adjust the offset pointers of ALL the losing elements up to that block (there can't be any winners (not possible))
		- it is safe to adjust those pointers because none have been cached yet
		- an offset pointer can be advanced, but it can never be reduced
	
	:: UNFLAGGED, UNFLAGGED ::
	- this MUST occur whenever the match character is > the start character
	- it is not possible to know which is better without advancing
	- keep advancing until the first difference is known (from actual difference or FLAG/FLAG situation)

- the winner does not need its 'up' value adjusted
- the loser needs its up value adjusted to the first point of difference with the winner, HOW?	
- is there a possible shortcut to allow this to happen quickly? (is there actually any need?)
- up pointers can stay the same, or they can advance, they can NEVER decrease
- for a flagged entry, we know that the up position is final and refers to the preceding queue item	

- so there is no need for a special block search? Seems that way
- need to be able to look up the cache position of an element. The cache must be cleared between iterations (-1s)
- any single long offset match operation will result in the elements along the way being updated
- there is no need to consult the parent element

*/

//Represents a block in a Burrows-Wheeler transformation
/*
struct Block
{
	int a; //first possible position of the block
	int b; //last possible position of the block
	int i; //the original string index of the block
	int u; //offset from the start of the block to the first point of difference to the preceding block
};

//Sort blocks using ranges
struct BlockSortWithRanges
{
	string sequence;
	int length;
	queue<int> copy;
	vector<int> blocks;
	vector<int> ups;
	vector<int> cache;
	
	vector<int> block2pos;
	vector<int> pos2block;
	
	//Initialize the object
	void init(string &s, vector<int> &b)
	{
		blocks   = b;
		sequence = s;
		length   = s.size();

		blocks.clear();
		blocks.resize(length);
		
		ups.clear();
		ups.resize(length);
		
		cache.clear();
		cache.resize(length);
		
		block2pos.clear();
		block2pos.resize(length);
		
		pos2block.clear();
		pos2block.resize(length);
		
		//Initialize the blocks
		for (int i=0; i<length; ++i)
		{
			blocks[i].a = 0;
			blocks[i].b = length - 1;
			blocks[i].i = i;
			blocks[i].u = -1;
			
			//ups[i] = 0;
			//cache[i] = -1;
			//block2pos[i] = -1;
			//pos2block[i] = -1;
		}
	}
	
	//Clear out the cache
		
	//Merge sort the blocks
	void sort(string &s, vector<int> &b)
	{
		//Initialize
		init(s,b);
		
		//For each n^2 iteration
		for (int bin=1; bin<length; bin*=2)
		{
			cout << "Sorting iteration: " << bin << endl;
			
			//Clear out the cache
			for (int i=0; i<length; ++i)
			{
				cache[i] = -1;
				block2pos[i] = -1;
				pos2block[i] = -1;
			}
			
			//Merge each pair of bins
			for (int start=0; start<length-bin; start+=2*bin)
			{
				//Determine the sizes of the two bins
				int sizeA = bin;
				int sizeB = bin;
				
				//Adjust bin B if necessary
				if (start + 2*bin > length)
				{
					sizeB = length - (start + bin);
					
					//Sanity check
					if (sizeB < 0) {cout << "ERROR: this case should not be possible" << endl;exit(1);}
				}
				
				//Merge the two sets of blocks
				merge(start, sizeA, sizeB);
			}
		}

		//Set the sorted blocks
		b = blocks;
	}
	
	//The crossthread-merge approach. Sorts based on offsets using the cache to take shortcuts where possible
	void merge(int start, int sizeA, int sizeB)
	{
		int posA = start;
		int posB = start + sizeA;
		int size = sizeA + sizeB;
		int bin  = sizeA;
		int end  = start + size;
		
		ups[blocks[posA]] = 0;
		ups[blocks[posB]] = 0;
		
		//Merge the two ranges into the queue
		for (int i=start; i<end; ++i)
		{
			//If a block is empty, flush the other and break
			// if (sizeA == 0) { while (i<end) { cache[blocks[posB]] = i; posB++; i++; } break; }
			// if (sizeB == 0) { while (i<end) { cache[blocks[posA]] = i; posA++; i++; } break; }
			if (sizeA == 0) { while (i<end) { block2pos[blocks[posB]] = i; pos2block[i] = blocks[posB]; posB++; i++; } break; }
			if (sizeB == 0) { while (i<end) { block2pos[blocks[posA]] = i; pos2block[i] = blocks[posA]; posA++; i++; } break; }
			
			//Link a pair by setting the 'up' pointer of the lesser element
			bool aWins = link(blocks[posA], blocks[posB], i-1);
			
			//Queue the better element
			// if (aWins) { cache[blocks[posA]] = i; posA++; sizeA--; }
			// else       { cache[blocks[posB]] = i; posB++; sizeB--; }
			if (aWins) { block2pos[blocks[posA]] = i; pos2block[i] = blocks[posA]; posA++; sizeA--; }
			else       { block2pos[blocks[posB]] = i; pos2block[i] = blocks[posB]; posB++; sizeB--; }
		}
		
		//Flush the cache (copy sorted element back into blocks)
		for (int i=start; i<end; ++i)
		{
			// blocks[cache[i]] = i;
			// if (verbose) cout << "set:" << i << " to " << cache[i] << endl;
			blocks[i] = pos2block[i];
			if (verbose) cout << "set:" << i << " to " << blocks[i] << endl;
		}		
	}
	
	//Compare two blocks to determine order and link the lower to the upper. Returns true if the upper is A, false if it's B
	bool link(int blockA, int blockB, int lastcache)
	{
		//Copy the offset pointers
		int a = ups[blockA];
		int b = ups[blockB];
		
		//Instant winner if the offsets are different
		if (a != b) return a > b;
		
		//Convert relative offsets to absolute positions
		a += blockA; if (a >= length) a -= length;
		b += blockB; if (b >= length) b -= length;
		
		//Advance to find the first point of difference
		for (int offset=ups[blockA]; offset<length; ++offset)
		{			
			//Winner known if the characters are different
			if (sequence[a] < sequence[b]) { update(blockB, offset); return true;  }
			if (sequence[b] < sequence[a]) { update(blockA, offset); return false; }
			
			//Winner known if both blocks are cached (have to trace up through the pointers to find the offset)
			// if (cache[a] >= 0 && cache[b] >= 0)
			// {
			// 	if (cache[a] < cache[b]) { update(blockB, offset + ups[b]); return true;  }
			// 	if (cache[b] < cache[a]) { update(blockA, offset + ups[a]); return false; }
			// 	cout << "ERROR0" << endl; exit(1);
			// }
			int cacheA = block2pos[a];
			int cacheB = block2pos[b];
			
			if (cacheA != -1 || cacheB != -1)
			{
				if (cacheA != -1 && cacheB != -1)
				{
					if (cacheA < cacheB) { update(blockB, offset + trace(cacheA,cacheB)); cout << '.'; return true;  }
					if (cacheB < cacheA) { update(blockA, offset + trace(cacheB,cacheA)); cout << '.'; return false; }
					cout << "ERROR: cache values cannot be the same" << endl;exit(1);
				}
				//if (cacheA >= 0 && cacheA < lastcache) { update(blockB, offset + trace(cacheA,lastcache)); cout << '.'; return true; }
				//if (cacheB >= 0 && cacheB < lastcache) { update(blockA, offset + trace(cacheB,lastcache)); cout << '.'; return false; }
			}
			
			//Advance to the next position
			if (++a == length) a = 0;
			if (++b == length) b = 0;
			advances++;
		}
		cout << "ERROR: entire block is a repeat" << endl;
		exit(1);
	}
	
	//Trace finds the minimum offset ('up' pointer) between two cached elements from b (lower) to a (upper)
	//That minimum offset is guaranteed to be the first point of difference between the two blocks
	int trace(int cacheA, int cacheB)
	{
		if (cacheB <= cacheA) {cout << "Logical error " << cacheA << " " << cacheB << endl; exit(1); }
		int best = length;
		
		for (int i=cacheB; i>cacheA; --i)
		{
			int offset = ups[pos2block[i]];
			if (offset == 1) return 1;
			if (offset < best) best = offset;
		}
		return best;
	}
	
	//Adjust the offset pointers of all losing blocks up to the offset.
	//The process can stop when an equal offset is found
	//Offset pointers can be advanced but never reduced
	void update(int start, int offset)
	{
		cout << "Update loser: " << start << " " << offset << endl;
		
		ups[start] = offset;
		return;
		
		for (int i=0; i<offset; ++i)
		{
			//Sanity check (cannot have been cached)
			// if (cache[start] >= 0)
			// {
			// 	cout << "strlen" << sequence.size() << " cachelen" << cache.size() << endl;
			// 	
			// 	for (int j=0; j<sequence.size(); ++j)
			// 	{
			// 		cout << j << " " << sequence[j] << endl;
			// 	}
			// 	cout << "start and offset " << start << " " << offset << " " << (start+i) << ": ERROR3" << endl; exit(3);
			// }
			
			//No need to do anything further if the offset is already optimal
			if (offset-i <= ups[start]) break;

			//Set the up pointer
			//ups[start] = offset-i;
			
			//Move to the next position
			if (++start == length) start = 0;
		}
	}
	
	//When merging two blocks, they first need to be compared to find the dominant one
	
	//Find the first difference point between two blocks (specified by index) starting from a specified offset
	int firstdiff(int a, int b, int offset)
	{
		//Advance to the offset
		a += offset;
		b += offset;
		
		if (a >= length) a -= length;
		if (b >= length) b -= length;

		//Advance to find the first point of difference
		for (int i=0; i<length-offset; ++i)
		{			
			//Finished if the characters are different
			if (sequence[a] != sequence[b]) return offset + i;
			
			//Advance to the next position
			if (++a == length) a = 0;
			if (++b == length) b = 0;
		}
		return length;
	}
	
	//Given two blocks and an offset, determine which block is dominant at that point (-1,0,1)
	int order(int a, int b, int offset)
	{
		a += offset;
		b += offset;
		
		if (a >= length) a -= length;
		if (b >= length) b -= length;
		
		if (sequence[a] < sequence[b]) return -1;
		if (sequence[a] > sequence[b]) return  1;
		
		return 0;
	}
	
	//The first pass of the sequence should look for single character repeats and collapse them
	void collapse()
	{
		for (int i=0; i<length-1; ++i)
		{
			if (sequence[i] != sequence[i+1]) continue;
			
			for (int j=i+1; j<length-1; ++j)
			{
				if ()
			}
		}
		
	}
	
	find the number of repeating characters
	find the winner (a or b) based on where the repeat stops
	
	when there is a repeat, it MUST be possible to take some kind of shortcut

	if looking at a single character repeat then it is possible to assign an ordering to ALL elements within the repeat region
	this is a good shortcut
	
	but how can a generic solution be implemented for repeats of any complexity?
};
*/
/**
 * Basically a merge-sort that uses tail end recursion
 * 'Up' pointers are used to mark diagonal threads and accelerate the merge step
 */
struct CrossThreadSort
{
	string sequence;
	int length;
	queue<int> copy;
	vector<int> blocks;
	vector<int> ups;
	vector<int> cache;
	
	vector<int> block2pos;
	vector<int> pos2block;
	
	int calls;
	int advances;
	int iteration;
	int shortcuts;
	
	bool verbose;
		
	//Initialize the object
	void init(string &s, vector<int> &b)
	{
		verbose = true;
		
		blocks   = b;
		sequence = s;
		length   = s.size();

		blocks.clear();
		blocks.resize(length);
		
		ups.clear();
		ups.resize(length);
		
		cache.clear();
		cache.resize(length);
		
		block2pos.clear();
		block2pos.resize(length);
		
		pos2block.clear();
		pos2block.resize(length);
		
		calls = 0;
		advances = 0;
		
		//Initialize the blocks
		for (int i=0; i<length; ++i)
		{
			blocks[i] = i;
			ups[i] = 0;
			cache[i] = -1;
			block2pos[i] = -1;
			pos2block[i] = -1;
		}
	}	
	
	//Print out the stacks to the screen
	void dump(int x1, int x2, int end)
	{
		cout << "-------------------" << endl;

		int len = (length > 80) ? 80 : length;
		
		for (int i=x1; i<x2; ++i)
		{
			cout << blocks[i] << " : " << ups[blocks[i]] << " : ";
			for (int j=0; j<len; ++j)
			{
				int p = blocks[i] + j; if (p >= length) p -= length;
				cout << sequence[p];
			}
			cout << endl;
		}
		cout << endl;
		for (int i=x2; i<=end; ++i)
		{
			cout << blocks[i] << " : " << ups[blocks[i]] << " : ";
			for (int j=0; j<len; ++j)
			{
				int p = blocks[i] + j; if (p >= length) p -= length;
				cout << sequence[p];
			}
			cout << endl;
		}
	}
	
	//Merge sort the blocks
	void sort(string &s, vector<int> &b)
	{
		//Initialize
		init(s,b);
		
		//For each n^2 iteration
		for (int bin=1; bin<length; bin*=2)
		{
			cout << "Sorting iteration: " << bin << endl;
			
			calls = 0; advances = 0; shortcuts = 0; iteration = bin;
			
			//Clear out the cache
			for (int i=0; i<length; ++i)
			{
				cache[i] = -1;
				block2pos[i] = -1;
				pos2block[i] = -1;
			}
			
			//Merge each pair of bins
			for (int start=0; start<length-bin; start+=2*bin)
			{
				//Determine the sizes of the two bins
				int sizeA = bin;
				int sizeB = bin;
				
				//Adjust bin B if necessary
				if (start + 2*bin > length)
				{
					sizeB = length - (start + bin);
					
					//Sanity check
					if (sizeB < 0) {cout << "ERROR: this case should not be possible" << endl;exit(1);}
				}
				
				if (verbose)
				{
					cout << endl;
					cout << "MERGE: " << iteration << "[" << start << "," << (start+bin) << "][" << sizeA << "," << sizeB << "]" << endl;
					dump(start, start+bin, start+(sizeA+sizeB)-1);
				}

				merge(start, sizeA, sizeB);
			}
			cout << "Calls:" << calls << " Advances:" << advances << " Shortcuts: " << shortcuts << endl;
		}

		//Set the sorted blocks
		b = blocks;
		
		if (verbose)
		{
			cout << "Final Blocks" << endl;
			dump(0,length,length-1);
		}
	}
	
	//The crossthread-merge approach. Sorts based on offsets using the cache to take shortcuts where possible
	void merge(int start, int sizeA, int sizeB)
	{
		int posA = start;
		int posB = start + sizeA;
		int size = sizeA + sizeB;
		int bin  = sizeA;
		int end  = start + size;
		
		ups[blocks[posA]] = 0;
		ups[blocks[posB]] = 0;
		
		//Merge the two ranges into the queue
		for (int i=start; i<end; ++i)
		{
			//If a block is empty, flush the other and break
			// if (sizeA == 0) { while (i<end) { cache[blocks[posB]] = i; posB++; i++; } break; }
			// if (sizeB == 0) { while (i<end) { cache[blocks[posA]] = i; posA++; i++; } break; }
			if (sizeA == 0) { while (i<end) { block2pos[blocks[posB]] = i; pos2block[i] = blocks[posB]; posB++; i++; } break; }
			if (sizeB == 0) { while (i<end) { block2pos[blocks[posA]] = i; pos2block[i] = blocks[posA]; posA++; i++; } break; }
			
			//Link a pair by setting the 'up' pointer of the lesser element
			bool aWins = link(blocks[posA], blocks[posB], i-1);
			
			//Queue the better element
			// if (aWins) { cache[blocks[posA]] = i; posA++; sizeA--; }
			// else       { cache[blocks[posB]] = i; posB++; sizeB--; }
			if (aWins) { block2pos[blocks[posA]] = i; pos2block[i] = blocks[posA]; posA++; sizeA--; }
			else       { block2pos[blocks[posB]] = i; pos2block[i] = blocks[posB]; posB++; sizeB--; }
		}
		
		//Flush the cache (copy sorted element back into blocks)
		for (int i=start; i<end; ++i)
		{
			// blocks[cache[i]] = i;
			// if (verbose) cout << "set:" << i << " to " << cache[i] << endl;
			blocks[i] = pos2block[i];
			if (verbose) cout << "set:" << i << " to " << blocks[i] << endl;
		}		
	}
	
	//Compare two blocks to determine order and link the lower to the upper. Returns true if the upper is A, false if it's B
	bool link(int blockA, int blockB, int lastcache)
	{
		//Copy the offset pointers
		int a = ups[blockA];
		int b = ups[blockB];
		
		//Instant winner if the offsets are different
		if (a != b) return a > b;
		
		//Convert relative offsets to absolute positions
		a += blockA; if (a >= length) a -= length;
		b += blockB; if (b >= length) b -= length;
		
		//Advance to find the first point of difference
		for (int offset=ups[blockA]; offset<length; ++offset)
		{			
			//Winner known if the characters are different
			if (sequence[a] < sequence[b]) { update(blockB, offset); return true;  }
			if (sequence[b] < sequence[a]) { update(blockA, offset); return false; }
			
			//Winner known if both blocks are cached (have to trace up through the pointers to find the offset)
			// if (cache[a] >= 0 && cache[b] >= 0)
			// {
			// 	if (cache[a] < cache[b]) { update(blockB, offset + ups[b]); return true;  }
			// 	if (cache[b] < cache[a]) { update(blockA, offset + ups[a]); return false; }
			// 	cout << "ERROR0" << endl; exit(1);
			// }
			int cacheA = block2pos[a];
			int cacheB = block2pos[b];
			
			if (cacheA != -1 || cacheB != -1)
			{
				if (cacheA != -1 && cacheB != -1)
				{
					if (cacheA < cacheB) { update(blockB, offset + trace(cacheA,cacheB)); cout << '.'; return true;  }
					if (cacheB < cacheA) { update(blockA, offset + trace(cacheB,cacheA)); cout << '.'; return false; }
					cout << "ERROR: cache values cannot be the same" << endl;exit(1);
				}
				//if (cacheA >= 0 && cacheA < lastcache) { update(blockB, offset + trace(cacheA,lastcache)); cout << '.'; return true; }
				//if (cacheB >= 0 && cacheB < lastcache) { update(blockA, offset + trace(cacheB,lastcache)); cout << '.'; return false; }
			}
			
			//Advance to the next position
			if (++a == length) a = 0;
			if (++b == length) b = 0;
			advances++;
		}
		cout << "ERROR: entire block is a repeat" << endl;
		exit(1);
	}
	
	//Trace finds the minimum offset ('up' pointer) between two cached elements from b (lower) to a (upper)
	//That minimum offset is guaranteed to be the first point of difference between the two blocks
	int trace(int cacheA, int cacheB)
	{
		if (cacheB <= cacheA) {cout << "Logical error " << cacheA << " " << cacheB << endl; exit(1); }
		int best = length;
		
		for (int i=cacheB; i>cacheA; --i)
		{
			int offset = ups[pos2block[i]];
			if (offset == 1) return 1;
			if (offset < best) best = offset;
		}
		return best;
	}
	
	//Adjust the offset pointers of all losing blocks up to the offset.
	//The process can stop when an equal offset is found
	//Offset pointers can be advanced but never reduced
	void update(int start, int offset)
	{
		cout << "Update loser: " << start << " " << offset << endl;
		
		ups[start] = offset;
		return;
		
		for (int i=0; i<offset; ++i)
		{
			//Sanity check (cannot have been cached)
			// if (cache[start] >= 0)
			// {
			// 	cout << "strlen" << sequence.size() << " cachelen" << cache.size() << endl;
			// 	
			// 	for (int j=0; j<sequence.size(); ++j)
			// 	{
			// 		cout << j << " " << sequence[j] << endl;
			// 	}
			// 	cout << "start and offset " << start << " " << offset << " " << (start+i) << ": ERROR3" << endl; exit(3);
			// }
			
			//No need to do anything further if the offset is already optimal
			if (offset-i <= ups[start]) break;

			//Set the up pointer
			//ups[start] = offset-i;
			
			//Move to the next position
			if (++start == length) start = 0;
		}
	}
};


/*

I need to find a shortcut to deal with situations where there is a very long advance process due to an internal repeat
and that repeat does not contain characters that have been cached

There is a quick shortcut that can be taken when the entire block is a repeat (using the winners approach)

Recursive internal minisort?
	- this would work I think
	- a new collection of sorted elements would be created
	- the new collection would then need to be merged back into the old one
	
	
SQUARESORT
	- conducted whenever the two parent sequences are identical (repeats)
	- the shape of the block is a square
	- ie, if the repeat is 100 characters long, then 100 blocks are taken for each A and B
	- the point of difference is determined for the first elements (which will never be located in A as they are repeats)
	- the dominant square is then known and the spacing between the squares is known
	- standard sort is then conducted
	- if the elements have the same offset, then defer to the dominant block
	- on defer, the up pointer is adjusted by the square spacing (which is known)
	- ASSERT: the same offset should only occur when the square spacing between two blocks is observed

RULE
	- when comparing two elements the UP pointer can NEVER decrease
	- the winner keeps the same pointer
	- the loser either keeps the same pointer or has it INCREASED
	- this can be used
	
INTERNAL REPEAT
	- when an internal repeat is discovered


//0 aaabaa
//1 aabaaa
//2 abaaaa
//3 baaaaa
//4 aaaaab
//5 aaaaba


/*
build the master radix table

reorder elements into radix groups

determine the outer running order (smallest to largest)

for each outer radix (smallest to largest)
{
	determine the inner running order (smallest to largest)
	
	for each inner radix (smallest to largest)
	{
		skip if the radix [outer,inner] has already been processed
		
		clear the radix stack
		clear the merge stack
	
		push the range to the radix stack
	
		while the radix stack has values
		{
			pop the radix range
			
			if range < 2
			{
				continue
			}
			if range == 2
			{
				compare the two blocks directly
				swap if necessary
				continue
			}
			
			sort the range at the current offset (results in radix sort with each radix split into cached(A) and uncached(B))

			for each radix block (A is already sorted if it exists)
			{
				if A == range
				{
					continue
				}
				
				if A > 0
				{
					push [A,B,offset] onto the merge stack
				}

				push B onto the range stack with an increased offset
			}
		}
	
		while the merge stack has values
		{
			pop a merge element
	
			if A == 0 continue
			if B == 0 continue
			
			merge A & B
		}
	
		set the caches for all elements in radix[outer,inner]
	}
	set the caches for all elements in radix[X,outer]
}




:: SCENARIOS ::

1: same, no cache
	advance offset
	
2: same, cache
	partition into [cache,nocache]
	sort cache
	advance offset for nocache

3: different, no cache OR cache
	partition into radix groups
	diagnose each radix group


Sort all the blocks based on a certain offset. The result is a radix sort with cached elements in a radix being sorted before uncached
Walk through
	if all the elements are cached then no further sorting is needed
	if there is 1 element then no sorting is needed
	if there are 2 elements then a simple comparison is needed
	if there is an uncached section, then it must be sorted on the next offset
	when there is both a cached and uncached section, then they must be merged
*/





/*

:: SHORTCUT IN PARENT MERGE
----a-------b----------------------------------------------------
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a

------------a-------b--------------------------------------------
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a

--------------------a-------b------------------------------------
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a, C->a

----------------------------a-------b----------------------------
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a, C->a

------------------------------------a-------b--------------------
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a, C->a, E->a

--------------------------------------------a-------b------------
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a, C->a, E->a

----------------------------------------------------a-------b----
|	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
A->a, C->a, E->a, G->a				


:: CROSS-THREAD ::
------------X--------------------
|	A	|	B	|	C	|	D	|

- sequenceA != sequenceB
- blocksA != blocksB
- greatest offset always wins
- same offset -> advance to first difference (can be slow)

:: ANCHOR ::
--------------------X------------
|	A	|	B	|	C	|	D	|

- sequenceA == sequenceB
- blocksA != blocksB
- cross-thread merge with a shortcut
	- if blocks are spaced by 'bin' -> defer to dominant block
	- otherwise advance

:: FAST ANCHOR ::
----------------------------X----
|	A	|	B	|	C	|	D	|

- sequenceA == sequenceB == sequenceC
- blocksA == blocksB
- same as above but faster as advance is never needed

:: SORT ::
{
	for each n^2 iteration
	{
		if the lead indicator is marked
		{
			the first anchor is known
		}
		else
		{
			determine the first anchor
		}

		merge the blocks (start sizeA sizeB anchor)

		if the anchor is inside the next bin
		{
			unmark the lead indicator
		}
		else
		{
			mark as many lead indicators downstream as possible
		}
	}
}

:: MERGE ::
{
	given ranges and anchor

	determine the dominant block
	set the 'up' values for the head elements

	while there are items to merge
	{
		if A is empty, flush B and exit
		if B is empty, flush A and exit
	
		get the 'up' values for the lead elements
	
		if the values are not equal
		{
			queue the greater one
			continue
		}
	
		if the blocks are bin-spaced and before the anchor
		{
			defer to the dominant block
			queue the element from the dominant block
			set the 'up' pointer for the other element to be the anchor
			continue
		}
		
		advance to the first difference
		queue the better element
		set the 'up' pointer of the other element to be the difference
	}
	
	
	
	
xxxxxxxx|oooxxxxx|xxxoooB	
xxxxxxxo|oooxxxxx|xxxoooB
xxxxxxoo|ooxxxxxx|xxoooB
xxxxxooo|oxxxxxxx|xoooB
xxxxooox|xxxxxxxx|oooB
xxxoooxx|xxxxxxxo|ooB
xxoooxxx|xxxxxxoo|oB
xoooxxxx|xxxxxooo|B
oooxxxxx|xxxxoooB|
}

13-aaatpalplantntan
14-aatpalplantntana
02-alplantntanaaatp
11-anaaatpalplantnt
06-antntanaaatpalpl
15-atpalplantntanaa
05-lantntanaaatpalp
03-lplantntanaaatpa
12-naaatpalplantnta
09-ntanaaatpalplant
07-ntntanaaatpalpla
01-palplantntanaaat
04-plantntanaaatpal
10-tanaaatpalplantn
08-tntanaaatpalplan
00-tpalplantntanaaa

30-alpplanttnalpptn
24-alpptnalpplanttn
19-anttnalpptnalppl
18-lanttnalpptnalpp
31-lpplanttnalpptna
25-lpptnalpplanttna
29-nalpplanttnalppt
23-nalpptnalpplantt
20-nttnalpptnalppla
17-planttnalpptnalp
16-pplanttnalpptnal
26-pptnalpplanttnal
27-ptnalpplanttnalp
28-tnalpplanttnalpp
22-tnalpptnalpplant
21-ttnalpptnalpplan




13-aaatpalplantntan
14-aatpalplantntana
02-alplantntanaaatp
11-anaaatpalplantnt
06-antntanaaatpalpl
15-atpalplantntanaa
05-lantntanaaatpalp
03-lplantntanaaatpa
12-naaatpalplantnta
09-ntanaaatpalplant
07-ntntanaaatpalpla
01-palplantntanaaat
04-plantntanaaatpal <-
10-tanaaatpalplantn
08-tntanaaatpalplan
00-tpalplantntanaaa

30-alpplanttnalpptn
24-alpptnalpplanttn
19-anttnalpptnalppl
18-lanttnalpptnalpp
31-lpplanttnalpptna
25-lpptnalpplanttna
29-nalpplanttnalppt
23-nalpptnalpplantt
20-nttnalpptnalppla
17-planttnalpptnalp <-
16-pplanttnalpptnal
26-pptnalpplanttnal
27-ptnalpplanttnalp
28-tnalpplanttnalpp
22-tnalpptnalpplant
21-ttnalpptnalpplan





*/


/*
:: Range Sorter ::
Idea here is to give each block a range of possible positions. As the sort proceeds
block ranges are refined until, at the end of the sort, each block's range covers
only one position.

Standard merge sort using upwards pointers works well until repeats are encountered.
The purpose of the range is to avoid excessive comparisons that arise from repeats.
Whenever the comparison between two blocks proceeds for more than one base (in other
words, whenever the first character(s) of the blocks are the same), then the ranges
for all blocks of an index up to the first point of difference between the two blocks
being compared, can be adjusted. Adjustment of this range allows future comparisons
to abort early with information about the proper ordering.

The upward arrows should be adjustable during this phase because a more downstream
arrow indicates greater similarity, meaning that the block in question does not need
an arrow adjustment...or perhaps this would degrade into bubble sort.

Binary search of the arrow space above a block would allow insertion of a new block.
This would also require the use of some kind of linked-list type structure. Binary
search and linked list tend to not work well together so I would need a solution 
for that.

*/

/*
:: Link Sort ::
See the associated powerpoint presentation to explain stepwise how this sorting works.

Reads are sorted using mergesort which is a divide and conquer approach that works with
O log(n) comlexity. Reads are compared and ordered in a doubly linked list. The left
link also carries a value denoting the relative point between the two reads where a 
difference is first detected. When merging reads for order, the offset can be used to
take a shortcut (the higher offset automatically wins). If there is a deadlock then
a comparison is conducted to determine the winner and the offset adjusted.

When a read with an adjusted pointer is appended to the merge list, then an attempt is
made to take a shortcut and auto insert all upstream and downstream pairs from the 
offset, aborting if a better pointer is encountered.

This extension process can be length, but once done once, the same ground does not 
have to be travelled. In the worst case there will be 2n comparisons per iteration.

So the final process should end up having a complexity of n.log(n).
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
 *
 * When two orphan groups are to be merged, the head elements are compared to find
 * the dominant block. This then defines the dominant and submissive group. The 
 * submissive group is always merged into to the dominant. Where a group is not an
 * orphan, it is immediately defined as submissive and merged into the group to 
 * which the head element points.
 *
 * To ensure optimal operation, a merge is aborted if the number of appends from
 * either group exceeds the current log iteration. Where a crossed-pointer is 
 * encountered (when a block points to a different group) a splice is conducted
 * at that point with the crossed block then defining the submissive group. The 
 * merge then continues.
 */

/*

Two groups are compared
- the dominant group becomes A
- the submissive group becomes B
- B is then merged into A
- if {b} is cross-linked
	- A is redefined as the group that {b} points to
	- B remains the same
- if {a} is cross-linked
	- A is redefined as the group that {a} points to
	- B is redefined as the remainder of the old A group
	
Counters keep track of the number of merged blocks from each group
- if a counter exceeds the bin limit
	- abort the merge and return {b}
	- {b} replaces the B group header
- otherwise if the merge runs to completion
	- return NULL
	- remove the group
*/
struct BlockSort
{
	struct block
	{
		unsigned int index; //index of this block
		unsigned int gnext; //index of the next group
		unsigned int gprev; //index of the prev group
		unsigned int next;  //closest next block
		unsigned int prev;  //closest prev block
		unsigned int diff;  //first difference between this block and prev block
	};

	const char * sequence;
	unsigned int length;
	unsigned int BLANK;
	vector<bool> flags;
	vector<block> blocks;

	block * gfirst;
	
	//Get a pointer to a block
	block * _b(unsigned int index)
	{
		assert(index <= length);
		return (index == length) ? NULL : &(blocks[index]);
	}

	//Initialize the object
	void init(string &s)
	{
		length = s.size();
		BLANK = length;
		sequence = s.c_str();
				
		blocks.resize(length);
		flags.resize(length);
		
		for (int i=0; i<length; ++i)
		{
			blocks[i].index = i;
			blocks[i].next  = BLANK;
			blocks[i].prev  = BLANK;
			blocks[i].diff  = 0;
			
			blocks[i].gnext = i+1;
			blocks[i].gprev = i-1;

			flags[i] = false;
		}
		blocks[0].gprev = BLANK;
		blocks[length-1].gnext = BLANK;
		
		gfirst = _b(0);
	}
		
	//Remove a group
	void remove(block * b)
	{
		assert(b != NULL);
		assert(b->gprev != BLANK || b->gnext != BLANK);
		
		if (b->gprev != BLANK) _b(b->gprev)->gnext = b->gnext;
		if (b->gnext != BLANK) _b(b->gnext)->gprev = b->gprev;
		
		if (b == gfirst) gfirst = _b(b->gnext);
		
		b->gprev = BLANK;
		b->gnext = BLANK;
	}
	
	//Replace a group with another one
	void replace(block * old, block * fresh)
	{
		assert(old != NULL && fresh != NULL);
		assert(fresh != gfirst);
		assert(old->gprev != BLANK || old->gnext != BLANK);

		fresh->gnext = old->gnext;
		fresh->gprev = old->gprev;
		
		if (old->gprev != BLANK) _b(old->gprev)->gnext = fresh->index;
		if (old->gnext != BLANK) _b(old->gnext)->gprev = fresh->index;
		
		if (old == gfirst) gfirst = fresh;
		
		old->gnext = BLANK;
		old->gprev = BLANK;
	}
	
	void dump(block * b)
	{
		//cout << b->index << ':' << b->gprev << ' ' << b->gnext << endl;
	}
	
	void matrix()
	{
		block * p = gfirst;
		
		for (block * p = gfirst; p != NULL; p = _b(p->gnext))
		{
			for (block * q = p; q != NULL; q = _b(q->next))
			{
				//cout << q->index << ' ' << q->diff << endl;
			}
			//cout << endl;
		}
	}
	
	//Set the winner between two blocks. True if {a} wins.
	bool resolve(block * a, block  * b)
	{
		//Cannot resolve blocks that don't point to the same parent. Note that if both blocks
		//have *no* parent, then the resolution will be successful.
		assert(a != b);
		assert(a->prev == b->prev);

		//cout << "Compare " << a->index << '&' << b->index << '=';
		
		//Auto win to the bigger up pointer
		if (a->diff != b->diff) {
			//cout << ((a->diff > b->diff) ? a->index : b->index) << endl;
			return a->diff > b->diff;
		}

		//Find the point of first difference
		unsigned int p1 = a->index + a->diff;
		unsigned int p2 = b->index + b->diff;

		if (p1 >= length) p1 -= length;
		if (p2 >= length) p2 -= length;

		for (unsigned int i=a->diff; i<length; ++i,++p1,++p2)
		{
			if (p1 == length) p1 = 0;
			if (p2 == length) p2 = 0;

			if (sequence[p1] == sequence[p2]) continue;
			
			if (sequence[p1] < sequence[p2])
			{
				b->diff = i;
				b->prev = a->index;
				flags[b->index] = true;
				//cout << a->index << endl;
				return true;
			}
			else
			{
				a->diff = i;
				a->prev = b->index;
				flags[a->index] = true;
				//cout << b->index << endl;
				return false;
			}
		}
		
		//Should not be possible to get here
		cout << "Whole string is a repeat" << endl;
		assert(false);
	}
	
	//Given a newly placed block, try to advance downstream pairs
	void cascade(block * z)
	{
		//No need to do anything if not flagged
		if (!flags[z->index]) return;
		
		//Unflag the block
		flags[z->index] = false;
		
		if (z->diff == 0) return;
		
		//assert(z->diff > 0);
		assert(_b(z->prev) != NULL); //shouldn't be possible for a flagged block to point nowhere
		unsigned int a = _b(z->prev)->index + 1;
		unsigned int b = z->index + 1;
		
		//cout << "Cascading: " << a << '/' << b << ' ' << z->diff << endl;
		
		for (unsigned int i=z->diff - 1; i>0; --i,++a,++b)
		{
			if (a == length) a = 0;
			if (b == length) b = 0;
			
			//cout << "Cascade compare: " << a << ' ' << b << endl;
			
			//Abort if a better placement is already known
			//cout << "Diff: " << blocks[b].diff << " i: " << i << endl;
			if (blocks[b].diff > i) break;
			
			blocks[b].diff = i;
			blocks[b].prev = a;
		}
	}
	
	//Merge group B into group A (A is defined by what B points to)
	void merge(block * group, unsigned int bin)
	{
		assert(group != NULL);
		assert(group->gprev != BLANK || group->gnext != BLANK);
		
		block * b = group;       assert(b != NULL); //head element from group B
		block * z = _b(b->prev); assert(z != NULL); //tail element from AB
		block * a = _b(z->next); //head element from group A
		
		//cout << "Merging " << b->index << " into " << z->index << endl;
		
		assert(a != b);
		
		unsigned int count_a = 0;
		unsigned int count_b = 0;
				
		//Merge group B into group A
		while (true)
		{
			//Finish if A is empty
			if (a == NULL)
			{
				//Only update B if it's not empty
				if (b != NULL)
				{
					//cout << "Empty A, append {b}: " << b->index << endl;
					
					z->next = b->index;
					//assert(b->prev == z->index);
				}
				else
				{
					assert(z->next == BLANK);
				}
				remove(group);
				
				return;
			}
			
			//Finish if B is empty (A can't be empty to get here)
			if (b == NULL)
			{
				//cout << "Empty B, append {a}: " << a->index << endl;
				z->next = a->index;
				//assert(a->prev == z->index);
				remove(group);
				
				return;
			}
			
			//Abort if the bin size is exceeded by either counter
			// if (count_a > bin || count_b > bin)
			// {
			// 	z->next = a->index;
			// 	replace(group,b);
			// 	//cout << "Appended (abort)" << a->index << endl;
			// 	return;
			// }
			
			//Splice if {a} has a crossed pointer
			if (a->prev != z->index)
			{
				//cout << "Splice A: " << a->index << '>' << a->prev << endl;
				
				z->next = b->index;
				z = _b(a->prev);
				b = a;
				a = _b(z->next);
				continue;
			}
			
			//Splice if {b} has a crossed pointer
			if (b->prev != z->index)
			{
				//cout << "Splice B: " << b->index << '>' << b->prev << endl;
				z->next = a->index;
				z = _b(b->prev);
				a = _b(z->next);
				assert(a != b);
				continue;
			}
			assert(a->prev == b->prev);

			if (resolve(a,b))
			{
				z->next = a->index;
				b->prev = a->index;
				z = a;
				a = _b(a->next);
				count_a++;
			}
			else
			{
				z->next = b->index;
				a->prev = b->index;
				z = b;
				b = _b(b->next);
				count_b++;
			}
			//cout << "Appended " << z->index << endl;
			//cout << "Calling to cascade with " << z->index << endl;
			//cascade(z);
		}
		assert(false);
	}
	
	//Determine if all characters in the string are the same
	bool mono()
	{
		for (unsigned int i=1; i<length; ++i)
		{
			if (sequence[i] != sequence[i-1]) return false;
		}
		return true;
	}
	
	//Merge sort the blocks
	void sort(string &s, vector<int> &bks)
	{
		//Initialize
		init(s);
		
		//No need to do anything if all characters are the same
		//if (mono()) return;
		
		//For each n^2 iteration
		for (int bin=1; bin<length; bin*=2)
		{
			cout << "Sorting iteration: " << bin << endl;
			//matrix();
			
			block * node = NULL;
			block * next = NULL;
			block * hold = NULL;
						
			for (node = gfirst; node != NULL; node = next)
			{
				//assert(node->gnext != BLANK || node->gprev != BLANK);
				next = _b(node->gnext);
				//cout << "GROUP IS: " << node->index << " NEXT IS: " << ((next == NULL) ? -1 : next->index) << endl;
				
				//Merge immediately if there is a trace (not an orphan)
				if (node->prev != BLANK)
				{
					assert(node->gnext != BLANK || node->gprev != BLANK);
					//cout << "PROBLEM Immediate merge: " << node->index << endl;
					merge(node,bin);
				}
				//Put the group on hold if possible (orphan)
				else if (hold == NULL)
				{
					// if (gfirst->gnext == BLANK)
					// {
					// 	assert(false);
					// }
					// assert(node->gnext != BLANK || node->gprev != BLANK);
					//cout << "Placed on hold: " << node->index << endl;
					hold = node;
				}
				//Find the winner of two orphans and merge
				else
				{
					assert(node->gnext != BLANK || node->gprev != BLANK);
					assert(hold->gnext != BLANK || hold->gprev != BLANK);
					//dump(hold);dump(node);

					//cout << "Pair of orphans: " << hold->index << '&' << node->index << endl;
					//Catch a condition where cascade has given the held group a parent
					if (hold->prev != node->prev)
					{
						hold = node;
					}
					else
					{
						merge(resolve(hold,node) ? node : hold, bin);
						hold = NULL;
					}
				}
			}
		}
		assert(gfirst != NULL && gfirst->gnext == BLANK);
		
		bks.resize(length);

		unsigned int i = 0;
		
		for (block * p = gfirst; p != NULL; p = _b(p->next), ++i)
		{
			bks[i] = p->index;
		}
		
		cout << "Finished" << endl;
	}
};

/*
- merge has to take care of deletion / replacement



00 plantenergy
01 lantenergyp
02 antenergypl
03 ntenergypla
04 tenergyplan
05 energyplant
06 nergyplante
07 ergyplanten
08 rgyplantene
09 gyplantener
10 yplantenetg
--------------
01 lantenergyp
00 plantenergy

02 antenergypl
03 ntenergypla

05 energyplant
04 tenergyplan

07 ergyplanten
06 nergyplante

09 gyplantener
08 rgyplantene

10 yplantenetg
--------------
02 antenergypl
01 lantenergyp
03 ntenergypla
00 plantenergy

05 energyplant
07 ergyplanten
06 nergyplante
04 tenergyplan

09 gyplantener
08 rgyplantene
10 yplantenetg
--------------
02 antenergypl
05 energyplant
07 ergyplanten
01 lantenergyp
06 nergyplante
03 ntenergypla
00 plantenergy
04 tenergyplan

09 gyplantener
08 rgyplantene
10 yplantenetg
--------------
02 antenergypl
05 energyplant
07 ergyplanten
09 gyplantener
01 lantenergyp
06 nergyplante
03 ntenergypla
00 plantenergy
08 rgyplantene
04 tenergyplan
10 yplantenetg
--------------
plantenergy
ltnrpeayeng

*/