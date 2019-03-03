all: main.cpp
	g++ -lpthread -o proj2 balancedSort.cpp -std=c++11
<<<<<<< HEAD
gen: gen_test_file.cpp
=======
gen: gen_test_file.cpp 
>>>>>>> 689adb364957c7f6dfc85fbae283fa91b987d464
	g++ -o test_case gen_test_file.cpp
clean:
	rm sort
