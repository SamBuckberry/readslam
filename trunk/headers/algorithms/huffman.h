/*
 * Huffman encode a string
 */
#pragma once

namespace Algorithms
{
	struct Huffman
	{
		struct Node
		{
			string code;
			bool leaf;
			char letter;
			int count;
			Node * childA;
			Node * childB;
		};
		
		bool compare(Node * a, Node * b)
		{
			return a->count < b->count;
		}
		
		vector<Node *> list;
		vector<int> counts;
		static const int LETTERS = 256;
		priority_queue<Node *, vector<Node *>, compare> tree;
		
		 Huffman() {}
		~Huffman() {}
	
		//Clear the tree
		void clear()
		{
			counts.clear();
			counts.resize(LETTERS,0);
		}
		
		//Build the Huffman key table for a string
		void build(string &source)
		{
			clear();
			
			//Count the number of each character
			for (int i=0, len=source.size(); i<len; ++i)
			{
				counts[(unsigned char)source[i]]++;
			}
			
			//Build and sort the initial tree
			for (int i=0; i<LETTERS; ++i)
			{
				if (counts[i] == 0) continue;
				Node node;
				node.letter = (char)i;
				node.count = counts[i];
				node.leaf = true;
				tree.push(node);
			}
			
			//Collapse the tree
			while (tree.size() > 1)
			{
				Node a = tree.pop();
				Node b = tree.pop();
				Node n;
				n.count = a.count + b.count;
				n.leaf = false;
				n.childA = a;
				n.childB = b;
				tree.push(n);
			}
			
			//Populate the codes of the tree elements
			Node node = tree.pop();
			
			set_code(node);
			while (!node.leaf)
		}
		
		//Walk the nodes of a tree to set the huffman code
		void set_code(Node &node)
		{
			if (node.leaf) return;

			node.childA.code += "0";
			node.childB.code += "1";
			
			set_code(node.childA);
			set_code(node.childB);
		}
		
		//Encode a string using Huffman encoding
		string encode(string original)
		{
			string encoded = "";
		}
	
		//Decode a Huffman-encoded string
		string decode(string infile, string outfile)
		{
			cout << "Decompressing " << infile << " to " << outfile << endl;

			ifstream in (infile.c_str(), ios::binary);

			if(!in)
			{
				perror(source);
				return 1;
			}

			in >> padding;

			char c;
			
			for(int i=0; i<MAX_VALUE; i++)
			{
				in >> root[i].weight;
			}
			root[MAX_VALUE*2-1].weight = INT_MAX;
			root[MAX_VALUE*2-2].index = -1;

			in.read(&c,1);
			
			while(in.read(&c,1))
			{
				for(int i=0; i<8; ++i)
				{
					iobuf += (c & 0x80 >> i)? "1" : "0";
				}
			}
			CreateTree();
			flush(outfile,1);
			cout << "Decompression finished\n";

			return 0;
		}
		
		struct Tree
		{
			Node *root;

			//flush iobuf after compression
			int flush(string outfile)
			{
				ofstream out(outfile.c_str(), ios::binary);
				
				if (!out)
				{
					stderr << "Unable to open file: " << outfile << endl;
					return;
				}
				out << padding << " ";
				
				for (int i=0; i<MAX_VALUE; ++i)
				{
					out << weight[i] << " ";
				}
				
				int i = 0;

				while(i < iobuf.length())
				{
					char c = 0;
					
					//read 8 bits,output as one single ASCII code
					for (int j=0; j<8; ++j)
					{
						c += (char)pow(2.0,7-j)*(iobuf[i++] - '0');
					}
					out.write(&c,1);
				}
				out.close();
			}

			//flush iobuf after decompression
			int flush(char *destination,int n) {}
			
			//Count the numbers of each character in the file
			int count(string infile)
			{
				ifstream in;
				in.open(infile.c_str());
				char c;
				
				while (in.good())
				{
					c = is.get();
					counts[(unsigned char)c]++;
				}
				in.close();
				
				for (int i=0; i<LETTERS; ++i)
				{
					root[i].count = counts[i];
				}
				root[LETTERS*2 - 1].count = INT_MAX;
				root[LETTERS*2 - 2].index = -1;
			}

			//return the node with the least weight
			//among the first one to the mth one
			int Get_Least(int m);

			//coding according to aggregate of each node
			void Coding(int cur,std::string temcode);

			void CreateTree(void);

			//copy-constructor
			HuffmanTree(const HuffmanTree &h);

			//operator =,no significance here
			HuffmanTree& operator=(const HuffmanTree& h)
			{
				//check for self-assignment
				if(&h != this)
					root = h.root;
				return *this;
			}
			int Compress(char *source,char *destination);
			int Decompress(char *source,char *destination);
			
		};
		
		struct Node
		{
			
		};
	};
}

public:
	int ascii;
	int index;		//decide whether it's the left child or the right one
	int weight;		//weight of the leaf node
	string code;	//keep the code of the leaf node
	int parent;		
	int left,right;		//left child and right child
	HuffmanNode():ascii(0),index (0),weight(0),code("0"),parent(0),left(0),right(0){}
	~HuffmanNode(void){}
};

//Huf.cpp
//implementation of functions in Huffman.h
//last modified:2009.02.25,21:35
#include "Huffman.h"
using namespace std;

//--------------------------------------------------------------------------
const int MAX_VALUE = 256;	//Max code(255)+1
int weight[256] = {0};		//keep the weights of 256 ASCII code
std::string iobuf;		
int padding = 0;		//

enum {LEFT = 1,RIGHT};		//if index == LEFT,then it's the left child

//----------------------------------------------------------------------------

//copy-constructor
HuffmanTree::HuffmanTree(const HuffmanTree &h){}

HuffmanTree::HuffmanTree(void)
{
	root = new HuffmanNode[MAX_VALUE*2-1];
}

HuffmanTree::~HuffmanTree(void){delete []root;}

int HuffmanTree::Get_Least(int m)
{
	//this is of very low efficience.
	int a = MAX_VALUE*2-1;
	for (int i = 0; i < m; i++)
		if ((root[a].weight > root[i].weight) && (root[i].parent == 0))
			a = i;
	return a;
}

void HuffmanTree::CreateTree(void)
{
	//create tree according to ASCII code
	int s1,s2;
	for (int i = MAX_VALUE; i < MAX_VALUE*2-1; ++i)
	{
		s1 = Get_Least(i);
		root[s1].parent = i;
		root[s1].index = LEFT;
		s2 = Get_Least(i);			
		root[s2].parent = i;
		root[s2].index = RIGHT;
		root[i].left = s1;
		root[i].right = s2;
		root[i].weight = root[s1].weight + root[s2].weight;
	}
	Coding(MAX_VALUE*2-2,"");//call Coding() recursivelly
}

void HuffmanTree::Coding(int cur,string temcode)
{
	if (cur < MAX_VALUE)
	{
		root[cur].code = temcode;
		return;
	}
	Coding(root[cur].left, temcode + "1");
	Coding(root[cur].right, temcode + "0");
}

//--------------------override flush----------------------
int HuffmanTree::flush(char *destination,int n){
	ofstream out(destination,ios::binary);
	if(!out)
	{
		perror(destination);
		return 1;
	}
	
	char c = 0;
	int h = MAX_VALUE*2-2;
	int i = 0;
	
	while(i <= iobuf.length() - padding)/*delete the padding 0s*/
	{
		//find the leaf node
		if(h >= MAX_VALUE)
		{
			if(iobuf[i] == '1')
				h = root[h].left;
			else
				h = root[h].right;
			++i;
		}
		else{//found
			c = h;
			out.write(&c,1);
			h = MAX_VALUE*2-2;
		}
	}
	out.close();
	return 0;
}

int HuffmanTree::Compress(char *source,char *destination)
{
	cout << "Compressing " << source << " to " << destination << endl;
	
	OpenFile(source);
	CreateTree();
	
	ifstream in(source,ios::binary);
	if(!in)
	{
		perror(source);
		return 1;
	}
	
	char c;
	while(in.read(&c,1))
		iobuf += root[(unsigned char)c].code;
	
	in.close();

	//calculate padding, add 0s to iobuf at the end
	padding = 8 - iobuf.size() % 8;
	for(int i = 0; i < padding; i++)
		iobuf += "0";
	
	flush(destination);
	cout << "Compression finished\n";
	
	return 0;
}



void PrintUsage()
{
	std::cout << "--------------------------------------------------\n"
		<< "Usage:\tHuffman [-c]/[-d] [source] [destination]\n"
		<< "e.g.\tHuffman -c c:\\a.txt d:\\c.rs\n"
		<< "-c:\tcompress source file to destination\n"
		<< "-d:\tdecompress source file to destination\n"
		<< "--------------------------------------------------\n";
}



void encode(string &in)
{
	int length = in.size();
	count(in);
	build();
	return compress(in);
}

bool comparator(const Node &a, const Node &b)
{
	return a.count < b.count;
}

vector<int> counts;
static const int LETTERS = 256;

void count(string str)
{
	counts.clear();
	counts.resize(LETTERS,0);
	
	for (int i=0, len=str.size(); i<len; ++i)
	{
		counts[i]++;
	}
}

void build()
{
	stack stackA;
	stack stackB;

	
this.getNext = function() {
if (tree.length > 0 && secondTree.length > 0 && tree[0].prob < secondTree[0].prob)
return tree.shift();

if (tree.length > 0 && secondTree.length > 0 && tree[0].prob > secondTree[0].prob)
return secondTree.shift();

	if (stackA.size() > 0) return tree.shift();

	return secondTree.shift();
}
var sortedProb = new Array();
var codes = new Array();

var x = 0;
for (var elem in prob) {
sortedProb[x] = new Array(elem, prob[elem]);
x = x + 1;
}

sortedProb = sortedProb.sort(sortNumberAsc);
x = 0;

for (var elem in sortedProb) {
tree[x] = new node();
tree[x].prob = sortedProb[elem][1];
tree[x].value = sortedProb[elem][0];
x = x + 1;
}
while (tree.length + secondTree.length > 1) {
var left = getNext();
var right = getNext();
var newnode = new node();
newnode.left = left;
newnode.right = right;
newnode.prob = left.prob + right.prob;
newnode.left.parent = newnode;
newnode.right.parent = newnode;
secondTree.push(newnode);
}

var currentnode = secondTree[0];
var code = "";
while (currentnode) {
if (currentnode.value) {
codes[currentnode.value] = code;
code = code.substr(0, code.length - 1);
currentnode.visited = true;
currentnode = currentnode.parent;
}
else if (!currentnode.left.visited) {
currentnode = currentnode.left;
code += "0";
}
else if (!currentnode.right.visited) {
currentnode = currentnode.right;
code += "1";
}
else {
currentnode.visited = true;
currentnode = currentnode.parent;
code = code.substr(0, code.length - 1);
}
}
return codes;
}


void encode(string &in, ostream out)
{
	for (int i=0, len=in.size(); i<len; ++i)
	{
		out << codes[(unsigned char)in[i]];
	}
}



struct Node
{
	bool visited;
	Node parent;
	Node left;
	Node right;
	int count;
	char value;
	string code = '';
};
