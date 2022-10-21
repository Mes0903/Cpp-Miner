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
  T x = f();    //只呼叫一次建構子
  T y = T(T(f()));    //只呼叫一次建構子
  T *z = new T(T(f()));    //只呼叫一次建構子
  delete z;
}