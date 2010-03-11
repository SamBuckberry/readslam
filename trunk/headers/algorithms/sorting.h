#pragma once

#include <cmath>
#include <stack>
#include <vector>
#include <string>

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
 */
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

		bool operator() (int x1, int x2)
		{
			for (int i=0; i<length; ++i)
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
