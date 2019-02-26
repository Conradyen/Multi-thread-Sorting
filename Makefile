all: main.cpp
	g++ -lpthread -o proj2 balancedSort.cpp -std=c++11
clean:
	rm proj2
