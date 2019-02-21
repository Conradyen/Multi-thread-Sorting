#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

sem_t semaphore;
int* array_ptr;
int array_len;
pair<int,int>* idx_array;
int test_arr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned thread_block_counter = 1;
pthread_barrier_t barr;

void error(const char *msg){
  perror(msg);
  exit(1);
}

int logn(int n){
  return (n > 1)? 1+logn(n/2):0;
}

int _pow(int val,int pow){
  if(pow <= 0){
    return 1;
  }
  return val * _pow(val,pow-1);
}

int num_thread(int num){
  if(num <= 1){
    return 0;
  }
  return (num/2) + num_thread(num/2);
}

void make_idxArray(int len){
  idx_array = new pair<int,int>[num_thread(len)];
  idx_array[0] = make_pair(0,len-1);
  int mid = (len-1)/2;
  idx_array[1] = make_pair(0,mid);
  idx_array[2] = make_pair(mid+1,len-1);
  for(int k = 3;k<num_thread(len)-1;k+=2){
    int mid = (idx_array[k/2].first+idx_array[k/2].second)/2;
    idx_array[k] = make_pair(idx_array[k/2].first,mid);
    idx_array[k+1] = make_pair(mid+1,idx_array[k/2].second);
  }
}

int read_input(string filename){
  ifstream file;
  file.open(filename);
  if(!file.is_open()){
    error("file not found");
  }
  int M;
  file >> M; // first line
  array_ptr = new int[M];
  make_idxArray(M);
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
  for(int i = 0;i< array_len;i++){
    cout<<array_ptr[i]<<" ";
  }
  cout<<"\n";
}


struct bsortarg{
  //int start;
  //int end;
  int idx;
};

void bsort(int start,int end){
  //sem_wait( &semaphore );
  cout<<start<<","<<end<<endl;
  while(start < end){
    if(array_ptr[start] > array_ptr[end]){
      int tmp = array_ptr[start];
      array_ptr[start] = array_ptr[end];
      array_ptr[end] = tmp;
    }
    start++;
    end--;
  }
  //sem_post( &semaphore );
}

void* bsort_thread(void* args){

  struct bsortarg* param = (struct bsortarg*) args;
  //cout<<"start"<<param->start<<endl;
  //cout<<"end"<<param->end<<endl;
  bsort(idx_array[param->idx].first,idx_array[param->idx].second);
  sem_post(&semaphore);
  pthread_barrier_wait(&barr);
  //pthread_barrier_wait(&barr);
  return 0;
}

int main(int argc,char* argv[]){

  bool o_mode = false;
  //cout<<argv[1]<<endl;
  if(strcmp(argv[2],"-o") == 0){
    o_mode = true;
  }else if(strcmp(argv[2],"-o") != 0 || strcmp(argv[1],"-r") != 0){
    error("unknown flag");
  }

  array_len = read_input(string(argv[1]));
  cout<<array_len<<endl;
  pthread_t tid[array_len-1];
  make_idxArray(16);
  for(int g = 0;g<15;g++){
    cout<<idx_array[g].first<<","<<idx_array[g].second<<" ";
  }
  cout<<"\n";
  bsortarg idx;
  cout<<"before"<<endl;
  for(int i = 0;i < array_len;i++){
    cout<<array_ptr[i]<<" ";
  }
  cout<<"\n";
  sem_init( &semaphore, 0, 1);
  //pthread_barrier_init(&barr,NULL,1);
  for(int i = 0;i<4;i++){
    //pthread_barrier_destroy(&barr);
    int num_t = _pow(2,i);
    int k = 0;
    pthread_barrier_init(&barr,NULL,num_t);

    while(k < num_t){
    sem_wait(&semaphore);
    int index = num_t+k-1;
    cout<<"idx :"<<num_t+k-1<<endl;
    //cout<<idx_array[num_t+k-1].first<<","<<idx_array[num_t+k-1].second<<endl;

    //idx.start = idx_array[index].first;
    //idx.end = idx_array[index].second;
    idx.idx = index;
    pthread_create(&tid[index], NULL, bsort_thread, (void *)&idx);
    k++;
    }

    //sem_wait(&semaphore);
    int s = num_t;
    int count = 0;
    while(count < num_t){
      cout<<"join"<<"("<<idx_array[s+count-1].first<<","<<idx_array[s+count-1].second<<")"<<endl;
      //cout<<s + count-1<<endl;
      pthread_join(tid[s+count-1],NULL);
      count++;
    }
    //sem_post( &semaphore );
    pthread_barrier_destroy(&barr);
    if(o_mode){
      cout<<"phase :"<<i+1<<endl;
       for(int a = 0;a < array_len;a++){
         cout<<array_ptr[a]<<" ";
       }
       cout<<"\n";
      }
   }

  delete array_ptr;
  delete idx_array;
}
