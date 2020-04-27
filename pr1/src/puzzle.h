#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <stack>
#include <vector>
#include <climits>

#define MAX_X 20
#define MAX_Y 20
#define MAX_DOMAIN 3000
#define MAX_FIELD 50
#define STR_SIZE 20

extern struct Word word_list[3000];

// struct Line {
// 	int start_x;
// 	int start_y;
// 	int length;
// 	char dir;
// };

// for word list only
struct Word {
	char str[STR_SIZE];
	unsigned int len;
};

struct Node {
	int fid; // field id (variable)
	int wid; // word id of the value
	char val[STR_SIZE];
	bool domain[MAX_FIELD][MAX_DOMAIN];
	unsigned int d_cnt[MAX_FIELD];
	bool root;
	bool expanded[MAX_FIELD+1];
};

struct Field {
	int id; // variable
	char val[STR_SIZE];
	char* itrsc[STR_SIZE];
	bool domain[MAX_DOMAIN]; // trimmed domain before search
	unsigned int d_cnt;
	// struct Line line;
	int start_x;
	int start_y;
	int length;
	char dir;
};

// the main CSP
class Puzzle {
public:
	char map[MAX_X][MAX_Y];
	struct Field field[MAX_FIELD];
	int cnt;

public:
	Puzzle();
	void setField(char* str);
	void setIntersect(Word* list);
	void setDomain(Word* list);
	void forwardCheck();
	void AC_3();
	void AC_3(bool &domain);
	int nodeExpand(std::stack<Node> &s, Node &n);
	int backtrack();
	int searchMRV(Node n);
	void printPuzzle();
};

bool intersect(Field a, Field b);
unsigned int countTrue(bool* b, int n);
void printStack(std::stack<Node> s); // debug

// the compare operator of the priority queue
struct MRV_cmp {
	bool operator()(const Node &a, const Node &b) const {
		return a.d_cnt < b.d_cnt;
	}
};
