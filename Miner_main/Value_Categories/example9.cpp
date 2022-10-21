#include <iostream>

struct Test {
  int i;
  Test()
      : i{}
  {
  }
  Test(const Test &other)
      : i{}
  {
  }
  ~Test()
  {
  }
};

Test test_fn()
{
  Test B;
  return B;    // 回傳的這個我們先叫他 X
}

int main()
{
  Test A = test_fn();
  return 0;
}