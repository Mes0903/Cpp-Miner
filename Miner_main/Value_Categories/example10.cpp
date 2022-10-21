#include <iostream>

int s_cnt{};

struct S {
  int m;
  S()
      : m{}
  {
    puts("S()");
    ++s_cnt;
  }
  S(const S &other)
      : m{ 2 }
  {
    puts("S(const S &other)");
    ++s_cnt;
  }
  ~S()
  {
    puts("~S()");
  }
};

S fn()
{
  return S{};
}

int main()
{
  int i = S(S(S(S(S(S(S(S(S(S(S(S(S(fn()))))))))))))).m;
  std::cout << "i = " << i << " s_cnt = " << s_cnt;

  int &&rr = 5;
}