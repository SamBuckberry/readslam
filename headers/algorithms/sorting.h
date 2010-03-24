#pragma once

#include <cmath>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>

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
struct BlockSort
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
	vector<int> winners;
	vector<int> anchors;
	
	int calls;
	int advances;
	int iteration;
	int shortcuts;
	
	//Print out the stacks to the screen
	void dump(int x1, int x2, int end)
	{
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
		cout << "-------------------" << endl;
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

	// //Compare two blocks at an offset and advance the offset until they differ
	// int advance(int x1, int x2, int offset)
	// {
	// 	x1 = blocks[x1] + offset; if (x1 >= length) x1 -= length;
	// 	x2 = blocks[x2] + offset; if (x2 >= length) x2 -= length;
	// 
	// 	while (offset < length)
	// 	{
	// 		if (sequence[x1] != sequence[x2]) return offset;
	// 		
	// 		if (++x1 == length) x1 = 0;
	// 		if (++x2 == length) x2 = 0;
	// 		
	// 		offset++;
	// 	}
	// 	return -1;
	// }
	
	// //Compare two blocks (specified by string position) and advance the offset until they differ
	// int advance_old(int x1, int x2, int offset)
	// {
	// 	x1 += offset; if (x1 >= length) x1 -= length;
	// 	x2 += offset; if (x2 >= length) x2 -= length;
	// 
	// 	while (offset < length)
	// 	{
	// 		if (sequence[x1] != sequence[x2]) return offset;
	// 		
	// 		if (++x1 == length) x1 = 0;
	// 		if (++x2 == length) x2 = 0;
	// 		
	// 		offset++;
	// 	}
	// 	cout << "ERROR, entire block is a repeat" << endl;
	// 	return -1;
	// }
	
	// //Advance through two blocks until a difference can be found. Set the 'up' pointer for the loser. Return true if A wins, false otherwise
	// bool advance(int x1, int x2, int offset)
	// {
	// 	x1 += offset; if (x1 >= length) x1 -= length;
	// 	x2 += offset; if (x2 >= length) x2 -= length;
	// 
	// 	while (offset < length)
	// 	{
	// 		if (winners[x1] == x2) { ups[x2] = anchors[x1]; shortcuts += anchors[x1] - offset; return true;  }
	// 		if (winners[x2] == x1) { ups[x1] = anchors[x2]; shortcuts += anchors[x1] - offset; return false; }
	// 
	// 		if (sequence[x1] < sequence[x2]) { ups[x2] = offset; return true;  }
	// 		if (sequence[x1] > sequence[x2]) { ups[x1] = offset; return false; }
	// 
	// 		if (++x1 == length) x1 = 0;
	// 		if (++x2 == length) x2 = 0;
	// 	
	// 		offset++;
	// 		advances++;
	// 	}
	// 	cout << "ERROR, entire block is a repeat" << endl;
	// 	exit(1);
	// }	
	
	// //Compare two blocks (specified by string position) at a specific offset for ordering
	// int compare(int x1, int x2, int offset)
	// {
	// 	x1 += offset; if (x1 >= length) x1 -= length;
	// 	x2 += offset; if (x2 >= length) x2 -= length;
	// 	
	// 	if      (sequence[x1] < sequence[x2]) return -1;
	// 	else if (sequence[x1] > sequence[x2]) return  1;
	// 	else                                  return  0;
	// }
		
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
		
		winners.clear();
		winners.resize(length);
		
		anchors.clear();
		anchors.resize(length);
		
		calls = 0;
		advances = 0;
		
		//Initialize the blocks
		for (int i=0; i<length; ++i)
		{
			blocks[i] = i;
			ups[i] = -1;
			winners[i] = -1;
			anchors[i] = -1;
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
			
			//Clear out the winners table
			for (int i=0; i<length; ++i)
			{
				winners[i] = -1;
				anchors[i] = -1;
			}
			
			//Merge each pair of bins
			for (int start=0; start<length; start+=2*bin)
			{
				//Determine the sizes of the two bins
				int sizeA = bin;
				int sizeB = bin;
				
				//Adjust bin B if necessary
				if (start + 2*bin > length)
				{
					sizeB = length - (start + bin);
					
					//No merge necessary if B is empty. Remember to reset the flag (this is the odd bin at the end)
					if (sizeB <= 0)
					{
						ups[blocks[start]] = -1;
						continue;
					}
				}
				
				// //Check if the lead indicator is set
				// int firstdiff = ups[blocks[start]];
				// 
				// //Determine the point of first difference if required
				// if (firstdiff == -1)
				// {
				// 	//Find the first point of difference between the parent sequences of the squares
				// 	firstdiff = start + advance(start, start+bin, 0);
				// 
				// 	//Set as many 'up' pointers as possible for this iteration (downstream)
				// 	for (int i=start; i<=firstdiff; i+=2*bin)
				// 	{
				// 		ups[blocks[i]] = firstdiff;
				// 	}
				// }
				// 
				// //Determine the offset
				// int offset = firstdiff - start;
				
				//Set this offset as the 'up' pointer for the lead blocks
				// ups[start]     = offset;
				// ups[start+bin] = offset;
				
				//Merge the blocks taking a shortcut if possible
				merge(start, sizeA, sizeB);
				
				// if (offset < bin)
				// {
				// 	//Bin sequence is not identical. Squares are not identical
				// 	merge(start, sizeA, sizeB, offset);
				// }
				// else if (offset < 2*bin)
				// {
				// 	//Bin sequence is identical. Squares are not identical
				// 	merge(start, sizeA, sizeB, offset);
				// }
				// else 
				// {
				// 	//Bin sequence is identical. Squares are identical
				// 	merge(start, sizeA, sizeB, offset);
				// }
				
				//Mark the current lead indicator for NEXT iteration if possible
				// if (firstdiff >= 2*bin)
				// {
				// 	ups[blocks[start]] = firstdiff;
				// }

				// :: SHORTCUT IN PARENT MERGE
				// ----a-------b----------------------------------------------------
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a
				// 
				// ------------a-------b--------------------------------------------
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a
				// 
				// --------------------a-------b------------------------------------
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a, C->a
				// 
				// ----------------------------a-------b----------------------------
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a, C->a
				// 
				// ------------------------------------a-------b--------------------
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a, C->a, E->a
				// 
				// --------------------------------------------a-------b------------
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a, C->a, E->a
				// 
				// ----------------------------------------------------a-------b----
				// |	A	|	B	|	C	|	D	|	E	|	F	|	G	|	H	|
				// A->a, C->a, E->a, G->a
			}
			cout << "Calls:" << calls << " Advances:" << advances << " Shortcuts: " << shortcuts << endl;
		}

		//Set the sorted blocks
		b = blocks;
	}
	
	//Merge two squares together
	void merge(int start, int sizeA, int sizeB)
	{
		int posA = start;
		int posB = start + sizeA;
		int size = sizeA + sizeB;
		int bin  = sizeA;

		//Set the initial offset for the elements
		ups[blocks[posA]] = 0;
		ups[blocks[posB]] = 0;
		
		//Merge the two ranges into the queue
		for (int i=0; i<size; ++i)
		{
			//If a block is empty, flush the other and break
			if (sizeA == 0) { while (i<size) { copy.push(blocks[posB]); posB++; i++; } break; }
			if (sizeB == 0) { while (i<size) { copy.push(blocks[posA]); posA++; i++; } break; }

			//Queue the better block
			bool ok = merge_pair(blocks[posA], blocks[posB]);
			
			if (ok) { copy.push(blocks[posA]); posA++; sizeA--; }
			else    { copy.push(blocks[posB]); posB++; sizeB--; }
		}
		
		//cout << "MERGE: " << iteration << "[" << start << "," << (start+bin) << "][" << sizeA << "," << sizeB << "]" << endl;
		//dump(start, start+bin, size-1);		
		flush(start,size);
	}
	
	//Used during merge to compare lead blocks. Returns true if A is the winner. Adjusts 'up' pointer of the loser
	bool merge_pair(int startA, int startB)
	{
		int upA = ups[startA];
		int upB = ups[startB];
		
		//If the offset pointers are different, return the instant winner
		if (upA != upB) return upA > upB;
		
		int x1 = startA + upA; if (x1 >= length) x1 -= length;
		int x2 = startB + upB; if (x2 >= length) x2 -= length;

		for (int offset=upA; offset<length; ++offset)
		{
			//Winners table can provide a shortcut
			if (winners[x1] == x2) { ups[startB] = offset + anchors[x1]; update_winners(startA,startB,offset); return true;  }
			if (winners[x2] == x1) { ups[startA] = offset + anchors[x2]; update_winners(startB,startA,offset); return false; }

			//Return when the sequence differs
			if (sequence[x1] < sequence[x2]) { ups[startB] = offset; update_winners(startA,startB,offset); return true;  }
			if (sequence[x1] > sequence[x2]) { ups[startA] = offset; update_winners(startB,startA,offset); return false; }
			
			x1++; if (x1 == length) x1 = 0;
			x2++; if (x2 == length) x2 = 0;
		}
		cout << "ERROR: entire block is a repeat" << endl;
		exit(1);
	}
	// 	//Advance to the first point of difference
	// 	upA = advance(posA, posB, upA);
	// 	
	// 	//Determine if A is the winner
	// 	bool ok = (compare(posA, posB, upA) == -1);
	// 
	// 	//If an advance occurred, count it and set the 'up' pointer of the loser
	// 	if (upA > upB)
	// 	{
	// 		if (ok) { ups[posB] = upA; update_winners(posA, posB, upA); }
	// 		else    { ups[posA] = upA; update_winners(posB, posA, upA); }
	// 		// advances += (upA - upB);
	// 	}
	// 	
	// 	//Return true if A is the winner
	// 	return ok;
	// }
	
	//Update the winners table. This table helps a lot with repeats.
	//The table is only updated if the repeat is LONGER
	void update_winners(int winner, int loser, int offset)
	{
		//return;
		
		for (int i=0; i<=offset; ++i)
		{
			if (winners[winner] == loser) return;
			if (anchors[winner] > offset - i) return;
			
			winners[winner] = loser;
			anchors[winner] = offset - i;
			
			if (++winner >= length) winner -= length;
			if (++loser  >= length) loser  -= length;
			
			advances++;
		}
	}
	
	//Flush the queue (copy sorted element back into blocks)
	void flush(int start, int size)
	{
		//Error check
		if (copy.size() != size)
		{
			cout << "Queue size is not correct" << endl;
			exit(1);
		}
		
		//Flush the queue
		for (int i=0; i<size; ++i)
		{
			blocks[start+i] = copy.front();
			copy.pop();
			//cout << "set:" << i << " to " << blocks[start+i] << endl;
		}

		//Reset the lead element to point to a blank (as the offset is unknown)
		ups[blocks[start]] = -1;
	}	
		
	// 
	// //Merge two blocks together (offset is the first point of difference between the PARENT strings. It indicates repetitiveness)
	// void merge_old(int start, int sizeA, int sizeB, int offset)
	// {
	// 	// if (++calls % 100000 == 0)
	// 	// {
	// 	// 	////cout << calls << "\r" << flush;
	// 	// }
	// 	// if (iteration >= 64)
	// 	// {
	// 	// 	if (calls == 958620)
	// 	// 	{
	// 	// 		dump(start, start+sizeA, start+sizeA+sizeB);
	// 	// 	}
	// 	// }
	// 	
	// 	int posA = start;
	// 	int posB = start + sizeA;
	// 	int size = sizeA + sizeB;
	// 	int bin  = sizeA;
	// 	
	// 	cout<<endl;
	// 	dump(posA, posB, posA+size-1);
	// 
	// 	////cout << "MERGE: " << start << ":" << posA << ":" << posB << endl;
	// 	//Determine the dominant block
	// 	
	// 	//Find the offset between the first 2 elements
	// 	int offset = advance(posA,posB,0);
	// 	
	// 	//Determine which if the fist 2 elements is better
	// 	bool a_best = (compare(posA, posB, offset) == -1);
	// 	
	// 	//Set the initial offset for the elements
	// 	ups[blocks[posA]] = (a_best) ? offset+1 : offset;
	// 	ups[blocks[posB]] = (a_best) ? offset : offset+1;
	// 	// if (a_best) { copy.push(blocks[posA]); posA++; sizeA--; ups[blocks[posB]] = offset; }
	// 	// else        { copy.push(blocks[posB]); posB++; sizeB--; ups[blocks[posA]] = offset; }
	// 	
	// 	//Merge the two ranges into the queue
	// 	for (int i=0; i<size; ++i)
	// 	{
	// 		//If a block is empty, flush the other and break
	// 		if (sizeA == 0) { while(i<size) { copy.push(blocks[posB]); posB++; i++; } break; }
	// 		if (sizeB == 0) { while(i<size) { copy.push(blocks[posA]); posA++; i++; } break; }
	// 
	// 		//Compare the offsets of the two elements
	// 		int a = ups[blocks[posA]];
	// 		int b = ups[blocks[posB]];
	// 		
	// 		//If they are different, queue the better one and continue
	// 		if (a > b) { copy.push(blocks[posA]); posA++; sizeA--; continue; }
	// 		if (a < b) { copy.push(blocks[posB]); posB++; sizeB--; continue; }
	// 		
	// 		//Compare the letters at the sticking point
	// 		int cmp = compare(posA, posB, a);
	// 		
	// 		//If they are different, queue the better one and continue
	// 		if (cmp < 0) { copy.push(blocks[posA]); posA++; sizeA--; continue; }
	// 		if (cmp > 0) { copy.push(blocks[posB]); posB++; sizeB--; continue; }
	// 		
	// 		//If the blocks are bin-spaced and the block is before the anchor (limit of repetitiveness) SHORTCUT
	// 		//if (blocks[posB] - blocks[posA] == bin)// && offset >= 2*sizeA)
	// 		if (blocks[posB] - blocks[posA] == bin && blocks[posA] <= anchor)
	// 		{
	// 			shortcuts++;
	// 			////cout << "shortcut" << endl;
	// 			//Queue the dominant block and set the thread pointer for the submissive block (to the anchor)
	// 			if (dominant) { copy.push(blocks[posA]); posA++; sizeA--; ups[blocks[posB]] = anchor - blocks[posA]; }
	// 			else          { copy.push(blocks[posB]); posB++; sizeB--; ups[blocks[posA]] = anchor - blocks[posA]; }
	// 		}
	// 		//Otherwise, a forward lookup is required (worst case)
	// 		else
	// 		{
	// 			//Advance to the first difference
	// 			a = advance(posA, posB, a);
	// 			//a = advance(posA, posB, 0);
	// 			if (a != b) advances++;
	// 			//cout << "bad" << (b-a) << endl;
	// 
	// 			//Queue the dominant block and update the thread pointer for the submissive block
	// 			if (compare(posA, posB, a) == -1) { copy.push(blocks[posA]); posA++; sizeA--; ups[blocks[posB]] = a; }
	// 			else                              { copy.push(blocks[posB]); posB++; sizeB--; ups[blocks[posA]] = a; }
	// 		}
	// 	}
	// 	
	// 	//Copy sorted elements back into the original vector
	// 	for (int i=0; i<size; ++i)
	// 	{
	// 		if (copy.empty())
	// 		{
	// 			////cout << "LOGICAL ERROR: 1886" << endl;
	// 		}
	// 		blocks[start+i] = copy.front();
	// 		//cout << "set:" << i << " to " << blocks[start+i] << endl;
	// 		copy.pop();
	// 	}
	// 	
	// 	//Reset the lead element to point to a blank (up offset is unknown)
	// 	ups[blocks[start]] = -1;
	// }
	// 
	// 
	// //This merge can be done when the two blocks are absolutely identical (this happens when the 'b' anchor is in D or further)
	// //--------------------a-------b------------------------------------
	// //|   A   |   B   |   C   |   D   |   E   |   F   |   G   |   H   |
	// //
	// //It can be guaranteed that the first two elements are separated by 'bin' spacing
	// void mergeIdentical(int start, int sizeA, int sizeB, int offset, bool a_best)
	// {
	// }	
	// 
	// //Merge two sub-blocks together (returns the offset of the first difference between the top two blocks)
	// int merge2(int startA, int startB, int end, int offset)
	// {
	// 	if (++calls % 100000 == 0)
	// 	{
	// 		cout << calls << "\r" << flush;
	// 	}
	// 	if (iteration >= 64)
	// 	{
	// 		if (calls == 958620)
	// 		{
	// 			dump(startA, startB, end);
	// 		}
	// 	}
	// 	cout<<endl;
	// 	dump(startA, startB, end);
	// 	
	// 	int sizeA = startB - startA;
	// 	int sizeB = end - startB + 1;
	// 	int size  = end - startA + 1;
	// 
	// 	if (sizeA > size) sizeA = size;
	// 	
	// 	//No need to do anything in these cases
	// 	if (size <= 1) return -1;
	// 	if (sizeB <= 0) return -1;
	// 	
	// 	int posA = startA;
	// 	int posB = startB;
	// 	
	// 	//Advance the offset to the point of first difference
	// 	offset = advance(posA, posB, 0);//offset);
	// 	
	// 	cout << "MERGE: " << startA << ":" << posA << ":" << posB << endl;
	// 	if (offset == -1)
	// 	{
	// 		cout << "\n" << "ERROR" << endl;
	// 	}
	// 	
	// 	
	// 	//Determine the better element
	// 	bool a_best = compare(posA, posB, offset);
	// 	bool repeat = offset >= sizeA;
	// 	
	// 	//Queue the better element and update the 'up' pointer for the worse element
	// 	if (a_best) { copy.push(blocks[posA]); posA++; sizeA--; ups[blocks[posB]] = offset; }
	// 	else        { copy.push(blocks[posB]); posB++; sizeB--; ups[blocks[posA]] = offset; }
	// 	
	// 	//Merge the two ranges into the queue
	// 	for (int i=1; i<size; ++i)
	// 	{
	// 		//If a block is empty, flush the other and break
	// 		if (sizeA == 0) { while(i<size) { copy.push(blocks[posB]); posB++; i++; } break; }
	// 		if (sizeB == 0) { while(i<size) { copy.push(blocks[posA]); posA++; i++; } break; }
	// 
	// 		//Compare the offsets of the two elements
	// 		int a = ups[blocks[posA]];
	// 		int b = ups[blocks[posB]];
	// 
	// 		//If they are different, queue the better one and continue
	// 		if (a < b) { copy.push(blocks[posB]); posB++; sizeB--; continue; }
	// 		if (a > b) { copy.push(blocks[posA]); posA++; sizeA--; continue; }
	// 
	// 		//They have the same offset. If not a repeat then advance to find the first difference
	// 		// if (!repeat)
	// 		// {
	// 			a = advance(posA, posB, b);
	// 			if (a != b) advances++;
	// 
	// 			//Determine the better one
	// 			a_best = compare(posA, posB, a);
	// 			repeat = a >= sizeA;
	// 		// } 
	// 		
	// 		//Queue the better one
	// 		if (a_best) { copy.push(blocks[posA]); posA++; sizeA--; }
	// 		else        { copy.push(blocks[posB]); posB++; sizeB--; }
	// 		
	// 		//If the offset didn't change, there is nothing more to do
	// 		if (a == b) continue;
	// 		
	// 		//Set the 'up' value of the worse element
	// 		if (a_best) ups[blocks[posB]] = a;
	// 		else        ups[blocks[posA]] = a;
	// 	}
	// 	
	// 	//Copy sorted elements back into the original vector
	// 	for (int i=startA; i<=end; ++i)
	// 	{
	// 		if (copy.empty())
	// 		{
	// 			cout << "LOGICAL ERROR: 1886" << endl;
	// 		}
	// 		blocks[i] = copy.front();
	// 		cout << "set:" << i << " to " << blocks[i] << endl;
	// 		copy.pop();
	// 	}
	// 	
	// 	//Return the offset difference between the first two elements
	// 	return ups[blocks[startA+1]];
	// }
	// 	
	// //If two merge blocks have been determined to be repeats, then quickly sort them
	// void quick(int posA, int posB, int endA, int endB, int offset)
	// {
	// 	int sizeA = endA - posA + 1;
	// 	int sizeB = endB - posB + 1;
	// 	int size = sizeA + sizeB;
	// 
	// 	//Determine which is best
	// 	bool a_best = compare(posA, posB, offset);
	// 	
	// 	//Push the better one to the stack
	// 	if (a_best) { copy.push(blocks[posA]); posA++; sizeA--; }
	// 	else        { copy.push(blocks[posB]); posB++; sizeB--; }
	// 	
	// 	//Merge the two ranges into the queue
	// 	for (int i=1; i<size; ++i)
	// 	{
	// 		//If either block is empty, flush the other and break
	// 		if (sizeA == 0) { while (i<size) { copy.push(blocks[posB]); posB++; i++; } break; }
	// 		if (sizeB == 0) { while (i<size) { copy.push(blocks[posA]); posA++; i++; } break; }
	// 
	// 		//Check the offsets of the two elements
	// 		int a = ups[blocks[posA]];
	// 		int b = ups[blocks[posB]];
	// 		
	// 		//If the offsets are not the same then queue the better element
	// 		if (a < b) { copy.push(blocks[posB]); posB++; sizeB--; continue; }
	// 		if (a > b) { copy.push(blocks[posA]); posA++; sizeA--; continue; }
	// 		
	// 		//They have the same offset. The initial comparison dictates which is queued (this is the shortcut)
	// 		//ERROR: MAKE SURE THAT THE CHARS ARE THE SAME AT THIS POINT. IF THEY ARE DIFFERENT, QUEUE THE BETTER ONE
	// 		if (a_best) { copy.push(blocks[posA]); posA++; sizeA--; }
	// 		else        { copy.push(blocks[posB]); posB++; sizeB--; }
	// 	}
	// }
};

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


