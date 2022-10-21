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

T f(bool condition)
{
  T result;

  if (condition)
    return T{};

  return result;
}

int main()
{
  T x = f(true);
  T y = f(false);

  return 0;
}