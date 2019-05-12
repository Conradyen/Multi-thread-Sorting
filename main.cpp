#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "MTsort.h"

using namespace std;

void error(const char *msg){
  perror(msg);
  exit(1);
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
