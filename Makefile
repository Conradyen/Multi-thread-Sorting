all: balancedSort.cpp
	g++ -lpthread -o sort balancedSort.cpp -std=c++11
clean:
	rm sort
