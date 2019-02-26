#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

sem_t sem_1,sem_2,sem_3;//three semaphores
int* array_ptr; //pointer to inputer array
int array_len;//length of array

pthread_barrier_t barr;//thread barrier
pthread_mutex_t stdoutLock;//stdout lock

void error(const char *msg){
  perror(msg);
  exit(1);
}

void sem_init(){
  ifstream file;
  file.open("sema.init");
  if(!file.is_open()){
    error("file not found");
  }
  int M;
  file >> M; // first line
  int tmp_sem[M];
  int line;
  int i = 0;

  while(file >> line){
    tmp_sem[i++] = line;
  }
  sem_init(&sem_1,0,tmp_sem[0]);//empty, should be 0;
  sem_init(&sem_2,0,tmp_sem[1]);//full, should be N/2, but any number works fine.
  sem_init(&sem_3,0,tmp_sem[2]);//for thread index make sure thread is initialize, should be 1
  file.close();
}

int logn(int n){
  /*
  calculate log2n
   */
  return (n > 1)? 1+logn(n/2):0;
}

int _pow(int val,int pow){
  /*
  calculate val to the power of val
   */
  if(pow <= 0){
    return 1;
  }
  return val * _pow(val,pow-1);
}

int num_thread(int num){
  /*
  calculate number of threads needed for balanced sort
   */
  if(num <= 1){
    return 0;
  }
  return (num/2) + num_thread(num/2);
}

int read_input(string filename){
  /*
  read input file
   */
  ifstream file;
  file.open(filename);
  if(!file.is_open()){
    error("file not found");
  }
  int M;
  file >> M; // first line
  array_ptr = new int[M];
  //make_idxArray(M);
  int line;
  int i = 0;

  while(file >> line){
    array_ptr[i++] = line;
  }
  file.close();
  //return array length
  return M;
}

bool is_sort(int arr[],int len){
  /*
  check if the array is sorted
   */
  int n = len;
  if(n ==1 || n == 0){
    return true;
  }
  if(arr[n-1] < arr[n-2]){
    return false;
  }
  return is_sort(arr,n-1);
}

void printArray(){
  /*
  print out the array
  use mutex to tidy stdout
   */

  for(int i = 0;i< array_len;i++){
    cout<<array_ptr[i]<<" ";
  }
  cout<<"\n";

}

void printstatus(bool o_mode,int threadnum,int stage,int phase){
  /*
  if "-o" flag print out thread number, current stage and current phase
  use mutex to tidy stdout
   */
  if(o_mode == true){
    pthread_mutex_lock(&stdoutLock);
    cout<<"thread "<<threadnum+1<<" "<<"stage "<<stage+1<<" "<<"phase "<<phase+1<<endl;
    pthread_mutex_unlock(&stdoutLock);
  }

}

struct bsortarg{
  /*
  holding for bsort_thread argument
   */
  int idx;
  int num_stages;
  int size;
  bool out;
};

void swap(int a,int b){
  /*
  swap two number in array
   */
  int tmp = array_ptr[a];
  array_ptr[a] = array_ptr[b];
  array_ptr[b] = tmp;
}

void* bsort_thread(void* args){
  /*
  thread function
   */
  struct bsortarg* param = (struct bsortarg*) args;
  int threadnum = param->idx;
  int out = param->out;
  sem_wait(&sem_1);
  for(int stage = 0;stage < param->num_stages; stage++){
    if(is_sort(array_ptr,param->size)){
      break;
    }
    for(int phase = 0;phase < param->num_stages; phase++){
        printstatus(out,threadnum,stage,phase);
        int group_size = param->size/_pow(2,phase);
        int g = (threadnum)/(group_size/2);
        int gidx = (threadnum) % (group_size/2);
        int start = g * group_size + gidx;
        int end = (((g+1)*group_size)-1)- gidx;
        if(array_ptr[start] > array_ptr[end]){
          swap(start,end);
        }
        sem_post(&sem_3);
        sem_post(&sem_2);
        pthread_barrier_wait(&barr);
        pthread_mutex_lock(&stdoutLock);
        if(out && threadnum == 0){
          printArray();
        }
        pthread_mutex_unlock(&stdoutLock);
    }
  }
}

int main(int argc,char* argv[]){

  bool o_mode = false;

  if(strcmp(argv[2],"-o") == 0){
    o_mode = true;
  }else if(strcmp(argv[2],"-r") == 0){
    o_mode = false;
  }
  else{
    error("unknown flag");
  }

  array_len = read_input(string(argv[1]));
  int num_threads = array_len/2;
  pthread_t tid[num_threads];

  bsortarg idx;
  cout<<"Before :"<<endl;
  printArray();

  sem_init();
  pthread_mutex_init(&stdoutLock, NULL);

  bsortarg arg;
  arg.num_stages = logn(array_len);
  arg.size = array_len;
  arg.out = o_mode;
  pthread_barrier_init(&barr,NULL,num_threads);

  //create threads
  for(int i = 0; i < num_threads; i++){
    sem_wait(&sem_2);
    sem_wait(&sem_3);
    arg.idx = i;
    pthread_create(&tid[i],NULL,bsort_thread,(void*)&arg);
    sem_post(&sem_1);
  }
  //join all threads
  for(int i = 0;i < num_threads;i++){
    pthread_join(tid[i],NULL);
  }

  cout<<"After :"<<endl;
  printArray();

  pthread_barrier_destroy(&barr);
  delete array_ptr;
}
