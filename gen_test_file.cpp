#include <iostream>
#include <fstream>

using namespace std;

int _pow(int val,int pow){
  /*
  calculate val to the power of val
   */
  if(pow <= 0){
    return 1;
  }
  return val * _pow(val,pow-1);
}

int main(int argc,char* argv[]){
  string line;
  cout<<"2 to the power of :";
  cin>>line;
  int len = _pow(2,stoi(line));
  ofstream outfile("test.c");
  outfile<<len<<endl;
  srand(time(NULL));
  for(int i = 0;i < len;i++){
    outfile<<to_string(rand()%100)<<endl;
  }
}
