#include <oneList.h>

#include <iostream>
using namespace std;

constexpr auto tmp{list("rui",1967,11,11)};

int main() {
  cout<<tmp.head<<endl;
  cout<<list(1,2,123).foldl([](auto a,auto b){return a+b;},10)<<endl;
  return 0;
}