#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

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

Ibarrier_type barr;


void error(const char *msg){
  perror(msg);
  exit(1);
}

float solver(float y){
  /*
  don't need this, to be delete
   */
  float x = y * array_len;
  return x;
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
    if((i % 8) == 0 && i != 0){
      cout<<endl;
    }
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
    cout<<"thread "<<threadnum+1<<" finished "<<"stage "<<stage+1<<" "<<"phase "<<phase+1<<endl;
    pthread_mutex_unlock(&stdoutLock);
  }
}

void Ibarrier_init(Ibarrier_type *barr, int num){

  ifstream file;
  file.open("sema.init");
  if(!file.is_open()){
    error("file not found");
  }
  int M;
  file >> M; // first line
  int tmp_sem[M];
  float line;
  int i = 0;

  while(file >> line){
    if(line < 1){
      line = solver(line);
    }
    tmp_sem[i++] = (int)line;
  }
  file.close();
  sem_init(&barr->mutex, 0, tmp_sem[0]);
  sem_init(&barr->in, 0, tmp_sem[1]);
  sem_init(&barr->out, 0, tmp_sem[2]);
  barr->num_barrier = num;
  barr->count = 0;

}

void Ibarrier_in(Ibarrier_type *barr){
  /*
  barrier in function,only call by barrier wait
   */
  sem_wait(&barr->mutex);
  if(++barr->count == barr->num_barrier){
    for(int i = 0;i < barr->num_barrier;i++){
      sem_post(&barr->in);
    }
  }
  sem_post(&barr->mutex);
  sem_wait(&barr->in);
}

void Ibarrier_out(Ibarrier_type *barr){
  /*
  barrier out function on call by barrier wait
   */
  sem_wait(&barr->mutex);
  if(--barr->count == 0){
    for(int i = 0;i < barr->num_barrier;i++){
      sem_post(&barr->out);
    }
  }
  sem_post(&barr->mutex);
  sem_wait(&barr->out);
}

void Ibarrier_wait(Ibarrier_type *barr){
  /*
  function call in thread
   */
  Ibarrier_in(barr);
  Ibarrier_out(barr);
}

void Ibarrier_destory(Ibarrier_type *barr){
  /*
  destory all barriers
   */
  sem_destroy(&barr->mutex);
  sem_destroy(&barr->in);
  sem_destroy(&barr->out);

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

  for(int stage = 0;stage < param->num_stages; stage++){
    if(is_sort(array_ptr,param->size)){
      break;
    }
    for(int phase = 0;phase < param->num_stages; phase++){
        int group_size = param->size/_pow(2,phase);
        int g = (threadnum)/(group_size/2);
        int gidx = (threadnum) % (group_size/2);
        int start = g * group_size + gidx;
        int end = (((g+1)*group_size)-1)- gidx;
        if(array_ptr[start] > array_ptr[end]){
          swap(start,end);
        }

        printstatus(out,threadnum,stage,phase);
        Ibarrier_wait(&barr);

        if(out && threadnum == 0){
          pthread_mutex_lock(&stdoutLock);
          printArray();
          pthread_mutex_unlock(&stdoutLock);
        }

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

  ifstream file;
  file.open(argv[1]);
  if(!file.is_open()){
    error("file not found");
  }

  int M;
  int line;
  file >> M; // first line
  while(M != 0){
    int count = M;
    int i = 0;
    array_ptr = new int[M];
    while(count > 0 && file >> line){
        array_ptr[i++] = line;
        count--;
      }
      if(!is_sort(array_ptr,M)){
        pthread_mutex_init(&stdoutLock, NULL);
        array_len = M;
        int num_threads = array_len/2;
        pthread_t tid[num_threads];
      //==============================================================
        bsortarg idx;
        cout<<"Before :"<<endl;
        printArray();

        bsortarg arg[num_threads];

        for(int i = 0;i<num_threads;i++){
          arg[i].num_stages = logn(array_len);
          arg[i].size = array_len;
          arg[i].out = o_mode;
          arg[i].idx = i;
        }

        Ibarrier_init(&barr,num_threads);

        //create threads
        for(int i = 0; i < num_threads; i++){
          pthread_create(&tid[i],NULL,bsort_thread,(void*)&arg[i]);
        }
        //join all threads
        for(int i = 0;i < num_threads;i++){
          pthread_join(tid[i],NULL);
        }
        cout<<"After :"<<endl;
        printArray();

        Ibarrier_destory(&barr);
        pthread_mutex_destroy(&stdoutLock);

      }

      file>>M;
      if(M == 0){
        break;
      }

    }
    file.close();
    delete array_ptr;
}
