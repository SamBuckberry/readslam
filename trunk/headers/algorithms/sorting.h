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
		this->starts.resize(LETTERS,-1);
		this->flags.resize(this->length,false);
		
		for (int i=0; i<length; ++i)
		{
			this->blocks[i] = i;
		}		
		
		range(0, this->length-1, 0);
		
		while (!this->ranges.empty())
		{
			int p3 = ranges.top(); ranges.pop();
			int p2 = ranges.top(); ranges.pop();
			int p1 = ranges.top(); ranges.pop();

			range(p1, p1+p2-1, p3);
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
		//if (offset > 100) return;
		if (x1 >= x2) return;
		
		//Take a shortcut if possible
		while(same(x1,x2,offset))
		{
			//if (++offset >= 100) return;
			if (++offset >= length) return;
		}		

		//Count characters in the range
		counts.clear();
		counts.resize(LETTERS,0);
	
		for (int i=x1; i<=x2; ++i)
		{
			int pos = blocks[i] + offset;
			if (pos >= length) pos -= length;
			unsigned char c = sequence[pos];
			counts[c]++;
			flags[i] = false;
		}

		//Set the starts for each character
		starts.clear();
		starts.resize(LETTERS,-1);
		int total = 0;
	
		for (int i=0; i<LETTERS; ++i)
		{
			if (counts[i] == 0) continue;
			starts[i] = total;
			total += counts[i];
			
			//Record subrange on the stack
			if (counts[i] > 1)
			{
				ranges.push(x1+starts[i]);
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
				unsigned char c = sequence[pos];
				int dest = x1 + starts[c];

				//Increment the visit count
				starts[c]++;

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
