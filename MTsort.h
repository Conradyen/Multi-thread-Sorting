#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int* array_ptr; //pointer to inputer array
int array_len;//length of array

pthread_mutex_t stdoutLock;//stdout lock

typedef struct {
  /*
  barrier type holds all variables for berrier
   */
  sem_t mutex;
  sem_t in;
  sem_t out;
  int num_barrier;
  int count;
} Ibarrier_type;

struct bsortarg{
  /*
  holding for bsort_thread argument
   */
  int idx;
  int num_stages;
  int size;
  bool out;
};

Ibarrier_type barr;

void error(const char *msg);
int logn(int n);
int _pow(int val,int pow);
bool is_sort(int arr[],int len);
void printArray();
void printstatus(bool o_mode,int threadnum,int stage,int phase);
void Ibarrier_init(Ibarrier_type *barr, int num);
void Ibarrier_in(Ibarrier_type *barr);
void Ibarrier_out(Ibarrier_type *barr);
void Ibarrier_wait(Ibarrier_type *barr,bool out,int threadnum);
void Ibarrier_destory(Ibarrier_type *barr);
void swap(int a,int b);
void* bsort_thread(void* args);
