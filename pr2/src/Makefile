.PHONY: all clean


all: main.c random_forest.cpp
	gcc -o main main.c
	g++ -o RF random_forest.cpp
	./main iris.data

clean:
	rm -f main RF
