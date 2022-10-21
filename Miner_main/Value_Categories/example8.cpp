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

T f();

struct D;

D g();

struct D : T {
  D()
      : T(f())
  {
    puts("D()");
  }

  D(int)
      : D(g())
  {
    puts("D(int) : D(g())");
  }
};

T f()
{
  return {};
}

D g()
{
  return {};
}

int main()
{
  D x;
  D y = g();
  D z{ 5 };
}