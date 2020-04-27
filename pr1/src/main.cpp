#include "puzzle.h"

using namespace std;

bool flag_ac3 = 0;
bool flag_fc = 0;
bool flag_mrv = 0;


struct Word word_list[3000];

int main(int argc, char* argv[]) {

	FILE* fp = fopen("./English words 3000.txt", "r");
	for(int i=0; fscanf(fp, "%s\n", word_list[i].str) != EOF; i++) {
		word_list[i].len = strlen(word_list[i].str);
		// printf("%s, %d\n", word_list[i].str, word_list[i].len);
	}

	fclose(fp);

	char buf[500];
	char* ptr;

	fp = fopen("./puzzle.txt", "r");

	while(fgets(buf, sizeof(buf), fp)) {
		struct Puzzle puzzle;
		
		puzzle.setField(buf);
		puzzle.setDomain(word_list); // check unary constraint
		puzzle.setIntersect();
		puzzle.AC_3();
		if(puzzle.backtrack() != -1) {
			puzzle.printPuzzle();
			printf("\n--------------------success---------------------\n\n");
		}
		else {
			puzzle.printPuzzle();
			fprintf(stderr, "the puzzle has no solution\n");
		}
		// break;

	}

	fclose(fp);

	return 0;
}