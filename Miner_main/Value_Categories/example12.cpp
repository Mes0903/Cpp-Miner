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

struct U : T {
  U()
  {
    puts("U()");
  }
  U(U &&other)
  {
    puts("U(U &&other)");
  }
  U(const T &other)
  {
    puts("U(const U &other)");
  }
  ~U()
  {
    puts("~U()");
  }
};


T f()
{
  U result;
  return result;
}

int main()
{
  T x = f();

  return 0;
}