#include <iostream>
#include <type_traits>

class NotAggregate1 {
  virtual void f() {}    // remember? no virtual functions
};

class NotAggregate2 {
  int x;    // x is private by default and non-static
};

class NotAggregate3 {
public:
  NotAggregate3(int) {}    // oops, user-defined constructor
};

class Aggregate1 {
public:
  NotAggregate1 member1;    // ok, public member
  [[noreturn]] Aggregate1 &operator=(Aggregate1 const &rhs) {}    // ok, copy-assignment
private:
  void f() {}    // ok, just a private function
};

int main()
{
  std::cout.setf(std::ios::boolalpha);
  std::cout << std::is_aggregate_v<NotAggregate1> << '\n'    // false
            << std::is_aggregate_v<NotAggregate2> << '\n'    // false
            << std::is_aggregate_v<NotAggregate3> << '\n'    // false
            << std::is_aggregate_v<Aggregate1> << '\n'    // true
            << std::is_aggregate_v<NotAggregate1[3]>;    // true
}