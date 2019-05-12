all: main.cpp balancedSort.cpp MTsort.h
	g++ -lpthread -o sort main.cpp -std=c++11
clean:
	rm sort
