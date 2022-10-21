#include <iostream>

struct T {
  T()
  {
    puts("T()");
  }
  T(T &&other)
  {
    puts("T(T &&other)");
  }
  T(const T &other)
  {
    puts("T(const T &other)");
  }
  ~T()
  {
    puts("~T()");
  }
};

T f()
{
  return T();
}

int main()
{
  f();    // 只會 call 一次建構子
}