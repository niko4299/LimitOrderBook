#include<iostream>
#include "utils/ringbuffer.hpp"

int main(int argc, char* argv[]) {
  auto rb = RingBuffer<int>(10);
  int i = 1;
  rb.push(i);
  int j;
  rb.pop(j);

  std::cout<<i<<" "<<j;
  return 0;
}