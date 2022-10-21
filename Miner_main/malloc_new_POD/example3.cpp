#include <cassert>
#include <iostream>

class POD {
public:
  [[noreturn]] int fn1() {}
  [[noreturn]] int fn2() {}
  int i1, i2;
};

class NonPOD {
public:
  NonPOD() {}
  virtual int fn1() { return 0; }
  virtual int fn2() { return 0; }

  int i3 = 10, i4 = 20;
};

int main()
{
  POD *ptr1 = new POD{ 1, 2 };
  std::cout << *reinterpret_cast<int *>(ptr1) << '\n';    // 1

  NonPOD *ptr2 = new NonPOD();
  std::cout << *reinterpret_cast<int *>(ptr2) << '\n';    // 亂數

  assert(reinterpret_cast<int *>(ptr1) == &(ptr1->i1));
  // assert(((void) "Not equal", reinterpret_cast<int *>(ptr2) == &(ptr2->i3)));    // error
}