#include <iostream>

static int cnt{};

struct S {
  S()
  {
    std::cout << R"(S()
        Construct object at )"
              << this << '\n'
              << '\n';
    ++cnt;
  }

  S(const S &other)
  {
    std::cout << R"(S(const S& other)
        Construct object at )"
              << this << '\n'
              << '\n';
    ++cnt;
  }

  S(S &&other)
  {
    std::cout << R"(S(S &&other)
        Construct object at )"
              << this << '\n'
              << '\n';
    ++cnt;
  }

  S &operator=(const S &other)
  {
    std::cout << R"(S& operator=(const S& other)
        Construct object at )"
              << this << '\n'
              << '\n';
    ++cnt;
    return *this;
  }

  S &operator=(S &&other)
  {
    std::cout << R"(S& operator=(S&& other)
        Construct object at )"
              << this << '\n'
              << '\n';
    ++cnt;
    return *this;
  }

  ~S()
  {
    std::cout << R"(~S()
        Destructed the object at )"
              << this << '\n'
              << '\n';
  }
};

S func()
{
  S B;
  return B;
}

int main()
{
  S A = func();
  std::cout << "\nthe address of A in main is " << &A << "\n\n";

  std::cout << "cnt = " << cnt << "\n\n";
  return 0;
}