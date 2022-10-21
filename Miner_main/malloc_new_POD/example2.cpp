#include <cstring>
#include <iostream>
#include <type_traits>

class T {
public:
  int i, j;
  char a, b, c;
};

int main()
{
  static_assert(std::is_pod_v<T> == true);    // pass

  std::byte buf[sizeof(T)];    // or char/unsinged char array

  T obj = { .i = 1, .j = 2, .a = 'a', .b = 'b', .c = 'c' };    // obj initialized to its original value
  std::cout << obj.i << ' ' << obj.j << ' ' << obj.a << ' ' << obj.b << ' ' << obj.c << '\n';

  memcpy(buf, &obj, sizeof(T));    // between these two calls to memcpy,
  // obj be modified
  obj.i = 0, obj.j = 0, obj.a = '0', obj.b = '0', obj.c = '0';
  std::cout << obj.i << ' ' << obj.j << ' ' << obj.a << ' ' << obj.b << ' ' << obj.c << '\n';    // all 0

  memcpy(&obj, buf, sizeof(T));    // at this point, each subobject of obj of scalar type
  // holds its original value
  std::cout << obj.i << ' ' << obj.j << ' ' << obj.a << ' ' << obj.b << ' ' << obj.c << '\n';
}