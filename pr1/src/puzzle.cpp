#include "puzzle.h"

Puzzle::Puzzle() {
	memset(map, 0, sizeof(map));
	memset(field, 0, sizeof(field));
	cnt = 0;
}

void Puzzle::setField(char* str) {
	char* ptr = str;
	int str_len = strlen(str);
	cnt = 0;

	for(int i=0; ptr<str+str_len; i++) {
		field[i].id = i;
		sscanf(ptr, "%d %d %d %c", &field[i].start_x, &field[i].start_y, &field[i].length, &field[i].dir);
		ptr += 10;
		cnt ++;
		// printf("%d %d %d %c\n", field[i].start_x, field[i].start_y, field[i].length, field[i].dir);
	}
	// printf("------%d-------\n", cnt);

	return;
}


// add arc consistency check
void Puzzle::setIntersect() {

	int delta_x = 0;
	int delta_y = 0;
	for(int i=0; i<cnt-1; i++) {
		for(int j=i+1; j<cnt; j++) {
			if(intersect(field[i], field[j])) {
				delta_x = abs(field[i].start_x - field[j].start_x);
				delta_y = abs(field[i].start_y - field[j].start_y);
				if(field[i].dir == 'A') {
					field[i].itrsc[delta_x] = &field[j].val[delta_y];
					field[j].itrsc[delta_y] = &field[i].val[delta_x];

				}
				else if(field[i].dir == 'D') {
					field[i].itrsc[delta_y] = &field[j].val[delta_x];
					field[j].itrsc[delta_x] = &field[i].val[delta_y];
				}
				// printf("%d <---> %d\n", i, j);
				// printf("%d: ", i);
				// for(int ii=0; ii<field[i].length; ii++) printf("%x ", field[i].itrsc[ii]);
				// printf("-\n");
				// printf("%d: ", j);
				// for(int jj=0; jj<field[j].length; jj++) printf("%x ", field[j].itrsc[jj]);
				// printf("-\n");
			}
		}
	}
	return;
}

// node consistent, remove values in all nodes that don't have same length
void Puzzle::setDomain(Word* list) { // word array not pointer
	// int size = sizeof(field[0].domain)/sizeof(bool);
	for(int i=0; i<cnt; i++) {
		for(int j=0; j<MAX_DOMAIN; j++) {
			field[i].domain[j] = (list[j].len == field[i].length) ? true : false;
		}
		field[i].d_cnt = countTrue(field[i].domain, MAX_DOMAIN);
		// printf("field[%d].d_cnt = %d\n", i, field[i].d_cnt);
	}
	// printf("%d\n", cnt);
	return;	
}

// // trim before search**
// void Puzzle::AC_3() {
// // trim domain with ac
// 	for(int ii=0; ii<MAX_DOMAIN; ii++) {
// 		for(int jj=0; jj<MAX_DOMAIN; jj++) {
// 			if(field[i].domain[ii] && field[j].domain[jj]) {
// 				char ch_a, ch_b;
// 				if(field[i].dir == 'A') {
// 					ch_a = list[ii].str[delta_x];
// 					ch_b = list[jj].str[delta_y];
// 				}
// 				else if(field[i].dir == 'D') {
// 					ch_a = list[ii].str[delta_y];
// 					ch_b = list[jj].str[delta_x];
// 				}
// 				else printf("error\n");

// 				if(ch_a == ch_b) {
// 					// printf("(%c, %c)\n", ch_a, ch_b);
// 					field[i].domain[ii] = false;
// 					field[j].domain[jj] = false;
// 					field[i].d_cnt--;
// 					field[j].d_cnt--;
// 				}
// 				// else printf("(%c, %c)\n", ch_a, ch_b);
// 			}
// 		}
// 	}

// }

// trim after each assignment
void Puzzle::AC_3(bool &domain) {

}

void Puzzle::forwardCheck() {

}

// return value: (-1)fail, (0)had expanded, (1)success
int Puzzle::nodeExpand(std::stack<Node> &s, Node &n) {
	int fail = 0;
		// printStack(s);

	if(n.expanded[n.fid+1]) {
		if(!n.root) { // remove value in the field(*)
			memset(field[n.fid].val, 0, STR_SIZE);
		}
		return 0;
	}
	if(n.root) {
		n.expanded[0] = true;
		return 1;
	}

	// domain contains only the assigned value
	memset(n.domain[n.fid], 0, sizeof(n.domain[n.fid]));
	n.domain[n.fid][n.wid] = true;
	n.d_cnt[n.fid] = 1;
	// inferences(forward)
	// consistency check: empty domain(forward)
	// consistency check: intersection
	for(int i=0; i<field[n.fid].length; i++) {
		char* c_ptr = field[n.fid].itrsc[i]; // pointer to the intersected char
		char c_val = n.val[i]; // current char value
		// printf("-----%d-%d-----\n", n.fid, i);
		if(c_ptr == NULL) continue; // no intersection
		else if(*c_ptr == 0 || *c_ptr == c_val) continue; // not assigned yet or same
		// printf("(%c, %c)\n", *c_ptr, c_val);
		return -1;
	}

	// store the value of the current node
	// if(n.fid == 0)
		// printf("field %d: %s\n", n.fid,n.val);
	strncpy(field[n.fid].val, n.val, STR_SIZE);
	n.expanded[n.fid+1] = true;
	return 1;
}

int Puzzle::backtrack() {
	std::stack<Node> s;
	int depth = 0;
	
	// push root before search
	Node root;
	memset(&root, 0, sizeof(root));
	for(int i=0; i<cnt; i++) {
		memcpy(root.domain[i], field[i].domain, sizeof(root.domain[i]));
		root.d_cnt[i] = field[i].d_cnt;
	}
	root.fid = -1;
	root.wid = -1;
	root.root = true;

	s.push(root);


	while(!s.empty()) {
		Node curr = s.top();
		s.pop();
		// node expansion
		int flag = nodeExpand(s, curr);
		if(flag == -1) { // fail expansion
			continue;
		}
		else if(flag == 0) { // already expanded -> next child
			depth--;
			// printf("%d depth\n", depth);
			continue;
		}
		else
			s.push(curr);
		
		// all variable has been assigned
		if(depth == cnt) return 0;

		// choose the next field to expand
		// Field* next = &field[depth];
		Field* next = &field[searchMRV(curr)];
		// generate children for each value in the domain
		for(int i=0; i<MAX_DOMAIN; i++) {
			if(next->domain[i] == false) continue;
			Node child;
			child.fid = next->id;
			child.wid = i;
			strncpy(child.val, word_list[i].str, STR_SIZE);
			for(int j=0; j<cnt; j++) {
				memcpy(child.domain[j], curr.domain[j], sizeof(child.domain[j]));
				child.d_cnt[j] = curr.d_cnt[j];
				child.expanded[j+1] = curr.expanded[j+1];
			}
			child.root = false;
			s.push(child);
			// printf("%s\n", child.val);
		}
		depth++;
		// printf("stack size: %ld\n", s.size());
		// printStack(s);
		// return 0;
	}

	return -1; // stack become empty
}

// return the fid of field which have minimum d_cnt
int Puzzle::searchMRV(Node n) {
	std::vector<int> arr;
	int mrv = INT_MAX;
	int var;
	for(int i=0; i<cnt; i++) {
		if(!n.expanded[i+1] && (n.d_cnt[i] < mrv)) {
			// printf("%d %d %d\n", i, mrv, n.d_cnt[i]);
			mrv = n.d_cnt[i];
			var = i;
		}
	}
	return var;
}

void Puzzle::printPuzzle() {
	for(int i=0; i<cnt; i++) {
		Field* f = &field[i];
		int _x = 0;
		int _y = 0;
		if(f->dir == 'A') { _x = 1; }
		if(f->dir == 'D') { _y = 1; }

		for(int j=0; j<f->length; j++) {
			map[f->start_y + (_y*j)][f->start_x + (_x*j)] = f->val[j];			
		}
		printf("%3d %s\n", i, f->val);
	}

	// print
	for(int i=0; i<MAX_X+1; i++) printf(" _");
	printf("\n");
	for(int i=0; i<MAX_Y; i++) {
		printf("|");
		for(int j=0; j<MAX_X; j++) {
			if(map[i][j] == 0) printf("  ");
			else printf(" %c", map[i][j]);
		}
		printf(" |\n");
	}
	for(int i=0; i<MAX_X+1; i++) printf(" _");
	printf("\n");
	return;
}

bool intersect(Field a, Field b) {
	if(a.dir == b.dir) return false;
	if((a.start_x > b.start_x) && (a.start_y > b.start_y)) return false;
	if((a.start_x < b.start_x) && (a.start_y < b.start_y)) return false;
	return true;
}


unsigned int countTrue(bool* b, int size) {
	unsigned int count = 0; 
	for(int i=0; i<size; i++) {
		count += b[i] & 1;
	}
	return count;
}

void printStack(std::stack<Node> s) {
	while(!s.empty()) {
		printf("%s, %d\n", s.top().val, s.top().d_cnt[s.top().fid]);
		s.pop();
	}
	printf("- - - - -\n");
}