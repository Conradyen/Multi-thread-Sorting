all: main.cpp
	g++ -lpthread -o proj2 balancedSort.cpp -std=c++11
gen: gen_test_file.cpp 
	g++ -o test_case gen_test_file.cpp
clean:
	rm proj2
