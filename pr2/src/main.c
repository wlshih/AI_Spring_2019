#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char* argv[]) {
	char cmd[256];

	printf("%-13s", "t/v");
	printf("%-7s", "bag");
	printf("%-7s", "gini");
	printf("%-6s", "for");
	printf("%-10s", "oob");
	printf("%-10s", "val");
	printf("%-10s", "time(s)");
	printf("\n");

	for(int i=1; i<10; i+=1) {
		sprintf(cmd, "./RF iris.data 0.6 0.2 0.0 %d", i);
		system(cmd);
	}
	for(int i=10; i<100; i+=10) {
		sprintf(cmd, "./RF iris.data 0.6 0.1 0.0 %d", i);
		system(cmd);
	}
	for(int i=100; i<600; i+=100) {
		sprintf(cmd, "./RF iris.data 0.6 0.1 0.0 %d", i);
		system(cmd);
	}

	return 0;
}