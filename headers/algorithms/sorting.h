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
		unsigned int next;  //closest next block
		unsigned int prev;  //closest prev block
		unsigned int diff;  //first difference between this block and prev block
		unsigned int base;  //historical difference between the previous inline element
	};

	const char * sequence;
	unsigned int length;
	unsigned int BLANK;
	vector<block> blocks;
	vector<unsigned int> heads;

	//Debugging
	unsigned int splices;
	unsigned int nulls;
	unsigned int appends;
	unsigned int resolves;
	unsigned int cascades;
	unsigned int cascades2;
	unsigned int groups;
	unsigned int bcount;
	unsigned int ccount;
	
	bool debug;
	
	//Get a pointer to a block
	inline block * _b(unsigned int index)
	{
		++bcount;
		return (index == length) ? NULL : &(blocks[index]);
	}
	
	inline char _c(unsigned int index)
	{
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
			//cout << q->prev << " <- " << q->index << " -> " << q->next << " (" << q->diff << ") ";

			if (q->prev != BLANK)
			{
				if (_b(q->prev)->next != q->index) cout << '*';
			}
			cout << endl;
		}
	}
	
	void matrix()
	{
		cout << "--------------------" << endl;
		
		for (unsigned int i=0; i<length; ++i)
		{
			block * p = _b(heads[i]);
			
			if (p == NULL) continue;
			
			for (block * q = p; q != NULL; q = _b(q->next))
			{
				dump(q);
			}
			cout << endl;
		}		
		cout << "--------------------" << endl;
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
		heads.resize(length);
		
		for (int i=0; i<length; ++i)
		{
			blocks[i].index = i;
			blocks[i].next  = BLANK;
			blocks[i].prev  = BLANK;
			blocks[i].diff  = BLANK;
			blocks[i].base = BLANK;
			
			heads[i] = i;
		}
	}
	
	//Find the first point of difference between two reads starting from a specified offset
	unsigned int firstdiff(block * a, block * b, unsigned int offset)
	{
		if (offset == BLANK) offset = 0;
		
		unsigned int p1 = a->index + offset;
		unsigned int p2 = b->index + offset;
		
		if (p1 >= length) p1 -= length;
		if (p2 >= length) p2 -= length;

		//Not faster to try to take a shortcut looking for a<-->b
		while (offset < length)
		{
			++resolves;

			if (_c(p1) != _c(p2))
			{
				return offset;
			}
			
			//Advance to the next character
			if (++p1 == length) p1 = 0;
			if (++p2 == length) p2 = 0;
			
			++offset;
		}
		
		//Winner catches the length condition
		cout << "Whole string is a repeat" << endl;
		return length; //FIXME: can put a condition in "winner" to let the lesser index win on whole repeat
	}
	
	//Determine the winner between two reads at a specified point
	bool winner(block * a, block * b, unsigned int offset)
	{
		unsigned int p1 = a->index + offset;
		unsigned int p2 = b->index + offset;
		
		if (p1 >= length) p1 -= length;
		if (p2 >= length) p2 -= length;

		return _c(p1) < _c(p2);
	}
	
	//Link two groups together. Returns true if A is dominant.
	bool resolve(block * a, block * b)
	{	
		//Find the point of first difference
		unsigned int diff = firstdiff(a,b,0);

		if (winner(a,b,diff))
		{
			b->diff = diff;
			b->prev = a->index;
			b->base = b->diff;
			return true;
		}
		a->diff = diff;
		a->prev = b->index;
		a->base = a->diff;
		return false;
	}
	
	//Determine if two reads are in adjacent bin groups
	inline bool adjacent(unsigned int a, unsigned int b, unsigned int bin)
	{
		a /= bin;
		b /= bin;

		a /= 2;
		b /= 2;
		
		return a == b;
	}		
	
	//When the diff pointer of a newly placed read has been advanced then cascade upstream and downstream
	//Cascade identifies the bounds of the repeated segment and updates all character pairs
	void cascade(block * z)
	{
		unsigned int p1 = z->prev;
		unsigned int p2 = z->index;
		unsigned int diff = z->diff;
		
		block * p;
				
		//Update pointers upstream (diagonal line plots with corrugation)
		while (true)
		{
			if (p1 == 0) p1 = length;
			if (p2 == 0) p2 = length;
			
			--p1; --p2; ++diff; ++cascades2;
		
			if (sequence[p1] != sequence[p2]) break;
			p = _b(p2);
			
			if (p->diff == diff) break;
			
			if (p->diff == BLANK || p->diff < diff)
			{
				p->prev = p1;
				p->diff = diff;
			}
		}
		
		p1 = z->prev;
		p2 = z->index;
		diff = z->diff;
		
		//Update all pairs through to the end of the matching segment
		while (diff != 0)
		{
			if (++p1 == length) p1 = 0;
			if (++p2 == length) p2 = 0;

			--diff; ++cascades;

			p = _b(p2);

			//Update if blank or worse
			if (p->diff < diff || p->diff == BLANK)
			{
				//Update the loser in the pair
				p->diff = diff;
				p->prev = p1;
				continue;
			}
			if (p->prev == z->index) continue;
			break;
		}
	}
	
	//Append a block {b} to the tail of a merged list {z} and return the displaced read (if there is one)
	inline void append(block * b, block * z)
	{
		if (z->next != b->index)
		{
			z->next = b->index;
			
			if (b->prev == z->index)
			{
				cascade(b);
			}
		}
		++appends;
	}
	
	//Merge group B into group A (A is defined by what B points to)
	unsigned int merge(block * group, unsigned int bin)
	{	
		block * b = group;       //head element from group B
		block * z = _b(b->prev); //tail element from AB
		block * a = _b(z->next); //head element from group A
		
		if (b->base == BLANK)
		{
			b->base = b->diff;
		}		
		
		//Merge group B into group A
		while (a != NULL && b != NULL)
		{
			//Resolve when there is not a crossed pointer
			if (a->prev == b->prev)
			{
				//Auto win to the bigger up pointer
				if (a->diff != b->diff)
				{
					if (a->diff > b->diff)
					{
						b->prev = a->index;
						b->base = b->diff;
						append(a,z);
						z = a;
						a = _b(a->next);					
					}
					else
					{
						a->prev = b->index;
						a->base = a->diff;
						append(b,z);
						z = b;
						b = _b(b->next);					
					}
				}
				else
				{
					//Find the point of first difference
					unsigned int diff = firstdiff(a, b, a->diff);
			
					if (winner(a,b,diff))
					{
						b->diff = diff;
						b->base = diff;
						b->prev = a->index;
						append(a,z);
						z = a;
						a = _b(a->next);					
					}
					else
					{
						a->diff = diff;
						a->base = diff;
						a->prev = b->index;
						append(b,z);
						z = b;
						b = _b(b->next);					
					}
				}
			}
			
			//There is a crossed pointer
			else
			{
				//Instant win if either points to the other
				// if (b->prev == a->index)
				// {
				// 	b->base = b->diff;
				// 	append(a,z);
				// 	z = a;
				// 	a = _b(a->next);
				// 	continue;
				// }
				// if (a->prev == b->index)
				// {					
				// 	a->base = a->diff;
				// 	append(b,z);
				// 	z = b;
				// 	b = _b(b->next);
				// 	continue;
				// }
				
				//Try a shortcut if {b} is spliced
				if (b->prev != z->index)
				{
					//Instant win if b points to a
					if (b->prev == a->index)
					{
						b->base = b->diff;
						append(a,z);
						z = a;
						a = _b(a->next);
						continue;
					}
					
					//Follow {b} splice if adjacent
					if (adjacent(b->prev, b->index, bin))
					{
						append(a,z);
						b->base = b->diff;
						z = _b(b->prev);
						a = _b(z->next);
						++splices;
						continue;
					}
				}
				
				//Try a shortcut if {a} is spliced
				if (a->prev != z->index)
				{	
					//Instant win if a points to b
					if (a->prev == b->index)
					{					
						a->base = a->diff;
						append(b,z);
						z = b;
						b = _b(b->next);
						continue;
					}
					
					//Follow {a} splice if adjacent
					if (adjacent(a->prev, a->index, bin))
					{
						append(b,z);
						a->base = a->diff;
						z = _b(a->prev);
						b = a;
						a = _b(z->next);
						++splices;
						continue;
					}
				}

				//Auto win to the bigger base pointer
				if (a->base != b->base)
				{
					if (a->base > b->base)
					{
						if (b->prev == z->index) b->prev = a->index;
						append(a,z);
						z = a;
						a = _b(a->next);					
					}
					else
					{
						if (a->prev == z->index) a->prev = b->index;
						append(b,z);
						z = b;
						b = _b(b->next);					
					}
					continue;
				}

				//Find the first point of difference between the reads
				unsigned int diff = firstdiff(a,b,a->base);
				
				if (winner(a,b,diff))
				{
					if (diff >= b->diff)
					{
						b->diff = diff;
						b->prev = a->index;
					}
					b->base = diff;
					append(a,z);
					z = a;
					a = _b(a->next);					
				}
				else
				{
					if (diff >= a->diff)
					{
						a->diff = diff;
						a->prev = b->index;
					}
					a->base = diff;
					append(b,z);
					z = b;
					b = _b(b->next);					
				}
			}
		}

		//One group must be null to get here. Append the non-null group
		append(a == NULL ? b : a, z);
		++nulls;
		return BLANK;
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
			resolves = 0;
			cascades = 0;
			cascades2 = 0;
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
				
				if (node == NULL) continue;
								
				//If the node has a trace then follow it immediately
				if (node->prev != BLANK)
				{
					if (debug) cout << "Trace " << node->index << endl;
					
					if (adjacent(node->index, node->prev, bin))
					{
						heads[npos] = merge(node,bin);
					}
				}
				//Put the group on hold if possible (orphan)
				else if (hold == NULL)
				{
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
						}
						hold = node;
						hpos = npos;
					}
					else
					{
						if (adjacent(node->index, hold->index, bin))
						{
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
			end = length;
			
			if (debug) matrix();
		
			unsigned int d = 1000000;
			
			cout << " Groups: " << groups;
			cout << " B:" << (bcount/d);
			cout << " C:" << (ccount/d);
			cout << " Appends:" << (appends/d);
			cout << " Resolves:" << (resolves/d);
			cout << " Cascades:" << (cascades/d);
			cout << "+" << (cascades2/d);
			cout << " Splices:" << (splices/d);
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