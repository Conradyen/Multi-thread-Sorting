#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>


using namespace std;

sem_t semaphore;
int* array_ptr;
pair<int,int>* idx_array;

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
  for(int k = 3;k<num_thread(len);k+=2){
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

struct bsortarg{
  int start;
  int end;
};

void bsort(int start,int end){

  sem_wait( &semaphore );
  while(start < end){
    if(array_ptr[start] > array_ptr[end]){
      int tmp = array_ptr[start];
      array_ptr[start] = array_ptr[end];
      array_ptr[end] = tmp;
    }
    start++;
    end--;
  }
  sem_post( &semaphore );
}

void* bsort_tread(void* args){
  struct bsortarg* param = (struct bsortarg*) args;
  bsort(param->start,param->end);
}


int main(int argc,char* argv[]){

  bool o_mode = false;
  //cout<<argv[1]<<endl;
  if(strcmp(argv[2],"-o") == 0){
    o_mode = true;
  }else if(strcmp(argv[2],"-o") != 0 || strcmp(argv[1],"-r") != 0){
    error("unknown flag");
  }

  int len = read_input(string(argv[1]));
  cout<<len<<endl;
  pthread_t tid[num_thread(len)];
  bsortarg idx;
  make_idxArray(16);
  sem_init( &semaphore, 0, 1 );

  cout<<"before"<<endl;
  for(int i = 0;i < len;i++){
    cout<<array_ptr[i]<<" ";
  }
  cout<<"\n";
  for(int i = 0;i<4;i++){
    int num_t = _pow(2,i);
    int k = 0;
    while(k < num_t){
    //cout<<idx_array[num_t+k-1].first<<","<<idx_array[num_t+k-1].second<<endl;
    idx.start = idx_array[num_t+k-1].first;
    idx.end = idx_array[num_t+k-1].second;
    pthread_create( &tid[num_t+k-1], NULL, bsort_tread, (void *)&idx);
    k++;
   }
   for(int j = 0; j < num_t;j++){
     pthread_join( tid[num_t+j-1], NULL );
   }
  if(o_mode){
    cout<<"phase :"<<i+1<<endl;
     for(int a = 0;a < len;a++){
       cout<<array_ptr[a]<<" ";
     }
     cout<<"\n";
    }
  }
  cout<<"after"<<endl;
  for(int i = 0;i < len;i++){
    cout<<array_ptr[i]<<" ";
  }
  cout<<"\n";
}
