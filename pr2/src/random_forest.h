#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

#define LABELSIZE 32
#define BUFSIZE 128
#define DATAMAX 200

struct Data {
	std::vector<float> attr;
	char label[LABELSIZE];
};

struct Node {
	std::vector<Data> dataset;
	int attribute;
	double threshold;
	Node* left;
	Node* right;
	Node* parent;
	int isleaf;
	char label[LABELSIZE];
	Node();
};

class Tree {
public:
	Node *root;
	std::vector<int> used_attr;
public:
	Tree();
	void bagging();
	double giniIndex(std::vector<Data>);
	double impurity(Node*);
	void split(Node*, int, double);
	double selectThreshold(Node*, int);
	void selectAttribute(Node*);
	void buildTree(Node*);
	int isPure(std::vector<Data>);
	int remainAttr();
	void selectLabel(Node*);
	int checkLeaf(Node*);


	void printDataset(); // debug
	void printDataset(Node*);
	void printDataset(std::vector<Data>);
};

int genRandom(int);
void timeStart();
void readData(const char*);
void divideDataset();
void buildForest();
char* traverse(Node*, Data);
char* ensemble(Data);
double correctRate(std::vector<Data>);
void printResult();