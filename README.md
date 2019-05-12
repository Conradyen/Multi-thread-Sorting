# Multi-thread-Sorting

Using pthread to sort a input list.

![](timeline.png)

## Input file
Every line contain only one number. The first number is the length of the list to be sort.

#### gen_test_file.cpp 

Generates test file. File name will be "test.c".

To compile:
```
make gen
```
To make test file:
```
./test_case
2 to the power of : 5
```
Input can be any integer. It will generate a file contain a list of integer with length 2 to the power of input.

## Reference

[1] Little book of semaphores, By Allen B. Downey, version 2.1.1

