---
title: 礦坑系列 ── malloc、new 與 POD Type
date: 2022-09-19
tag: C++ Miner
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?" height = 50><br>malloc、new 與 POD Type</center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 前言

`new` 是 C\+\+ 內一個幫助我們操作記憶體的工具，傳統的 pure C 內也有類似的工具 `malloc`，然而由於 C\+\+ 語言上內建多支援了物件導向的特性，導致 `new` 比 `malloc` 多做了一些事情。

本篇大部分內容為整理 Effective C\+\+ 而來，並額外補上一些細節。

# malloc 與 POD type

首先先講一下 `malloc` 在 C\+\+ 的行為，要想講這個我們最好先知道兩個東西，一個為 Aggregates，一個為 POD，全名為 Plain Old Data，這兩個東西對於 C-style coding 非常的有幫助，在某些 Modern C\+\+ 的場合也會有幫助。

這邊我主要翻譯一篇 stackoverflow 上的解釋，這篇寫的挺好的，大家也可以去看看原文( [連結](https://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special/7189821#7189821) )。

Aggregate 與 POD 的定義在 C\+\+11、C\+\+14、C\+\+17 甚至 C\+\+20 時都有更動，但基本上都是增加了新的東西進去，因此這裡主要以 C\+\+03 為主來去解釋，幫助大家先快速理解，後面再回頭介紹 C\+\+11 後的定義。

## Aggregates

### 什麼是 Aggregate?
一般我們理解的定義來自於 Standard 的定義：

> (C++03 8.5.1 §1)：
>
> An aggregate is an array or a class (clause 9) with no user-declared constructors (12.1), no private or protected non-static data members (clause 11), no base classes (clause 10), and no virtual functions (10.3).

較新的定義(C\+\+17)：

> [n4659(11.6.1 - 1)](https://timsong-cpp.github.io/cppwp/n4659/dcl.init.aggr#1)：
> An aggregate is an array or a class with  
> ── no user-provided, explicit, or inherited constructors ([class.ctor]),  
> ── no private or protected non-static data members (Clause [class.access]),   
> ── no virtual functions, and  
> ── no virtual, private, or protected base classes ([class.mi]).   
>
> [ Note: Aggregate initialization does not allow accessing protected and private base class' members or constructors.  — end note ]

所以你可以看見一個 <span class = "yellow">array 肯定是個 Aggregate</span>，而 class 的話則要符合上面那些條件，注意 draft 裡面的 class 有包含 struct 與 union。

我們詳細看一下有關 class 的幾個重點

+ Aggregate class 不能有自己定義的建構函數或 copy constructor 等等的，但如果是 compiler 幫你生成的可以，不是 user-provided 的就好。
+ 不能有 `private` 或 `protected` 的 non-static data member，但如果是 static data member 就沒關係，非 constructor 的 member function 也沒關係，<span class = "yellow">但不能有 virtual function</span>
+ 可以有 user-declared/user-defined 的 copy-assignment operator 或 destructor
+ 就算是 array of non-aggregate class type 也是一個 Aggregate
+ 不具有繼承關係
用一個例子來確認一下 ([連結](https://godbolt.org/z/G1W43vcvs))：

```cpp
// ensure the cpp version is C++17 or newer version
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
```

### Aggregate 的好處?

現在我們就來看看 Aggregate 有什麼好處，主要是可以使用 Aggregate initialization `{}` 來初始化，你可以常常在 array 的初始化裡看到它，舉個例子：

<code>Type arr[n] = {a<sub>1</sub>, a<sub>2</sub>, …, a<sub>m</sub>};</code>

根據 `n` 與 `m` 的不同會有幾個情況：

+ `m == n`
    陣列裡的第 i 個元素 (i<sup>th</sup>) 會被初始化為 a<sub>i</sub>
+ `m < n`
    陣列中的前 m 個元素會被初始化為 a<sub>1</sub>, a<sub>2</sub>, …, a<sub>m</sub>，而剩下的 n-m 個元素，如果可以的話會嘗試使用 value-initialized，如果 element 是 non-Aggregate type 的話可能會失敗。
+ `m > n`
    compiler 會報錯
+ 沒有寫 n 的情況
    n 會被設為 m，也就是說 `n == m`

看個例子：

```cpp
class A {
public:
  A(int) {}    // no default constructor
};

class B {
public:
  B() {}    // default constructor available
};

int main()
{
  A a1[3] = { A(2), A(1), A(14) };    // OK n == m
  A a2[3] = { A(2) };    // ERROR A has no default constructor. Unable to value-initialize a2[1] and a2[2]
  B b1[3] = { B() };    // OK b1[1] and b1[2] are value initialized, in this case with the default-ctor
  int Array1[1000] = { 0 };    // All elements are initialized with 0;
  int Array2[1000] = { 1 };    // Attention: only the first element is 1, the rest are 0;
  bool Array3[1000] = {};    // the braces can be empty too. All elements initialized with false
  int Array4[1000];    // no initializer. This is different from an empty {} initializer in that
  // the elements in this case are not value-initialized, but have indeterminate values
  //(unless, of course, Array4 is a global array)
  int array[2] = { 1, 2, 3, 4 };    // ERROR, too many initializers
}
```

而對於 class，初始化的方法長的差不多，`{}` 內會照 class definition 的順序對其 non-static data member 做初始化，同樣的如果 `{}` 太少，剩下的元素也是使用 value-initialized，如果它不能使用 value-initialized，就會導致 compile-time error，看個例子：

```cpp
struct X {
  int i1;
  int i2;
};

struct Y {
  char c;
  X x;
  int i[2];
  float f;

protected:
  static double d;

private:
  void g() {}
};

Y y = { 'a', { 10, 20 }, { 20, 30 } };
```

上例中這裡 `y.c` 被初始化為 `a`，`y.x.i1` 初始化為 `10`，`y.i[1]` 初始化為 `30`，`y.f` 初始化為 `0.0`。

### 結論

看到這裡你可能會想說為什麼需要這些設定，前面也講了，這可以幫助 C-style coding，某種程度上也代表這個 Type 類似 built-in type，如 `int`、`char` 等可以幫助 compiler 去做優化。

使用 `{}` 初始化代表這個 class 內的元素就只有 `{}` 內的元素，且初始化時不需要額外特別的操作。

當你的 class 有自定義的 constructor 時，通常表示在初始化內部成員時需要一些額外的操作，因此使用 aggregate-initialized 會造成 compile-time error
```cpp
struct Z {
  Z() {}
  double d;
};

Z z = { 0.0 }; // error
```

而如果你的 class 有 virtual function，在大多數的 compiler 實作下，這個 class 會有對應的 vtable，而指向 vtable 的指標 `this->__vptr` 通常會利用 constructor 的 member initialize list 初始化，因此使用 aggregate-initialized 也是不恰當的。

對於 vtable，有興趣的可以看看這篇：[What happens in the hardware when I call a virtual function? How many layers of indirection are there? How much overhead is there?](https://isocpp.org/wiki/faq/virtual-functions#dyn-binding2)

## POD

### 定義

POD 全名叫 Plain Old Data，一般我們理解的 POD 與 C++03 標準內的定義相同：

> (C++ 03 9 §4)：
> A POD-struct is an aggregate class that has no non-static data members of type non-POD-struct, non-POD-union (or array of such types) or reference, and has no user-defined copy assignment operator and no user-defined destructor. Similarly, a POD-union is an aggregate union that has no non-static data members of type non-POD-struct, non-POD-union (or array of such types) or reference, and has no user-defined copy assignment operator and no user-defined destructor. A POD class is a class that is either a POD-struct or a POD-union.

先不看 union 與 enum，把上面的文字稍微過濾一下：
> An aggregate class is called a POD if it has no user-defined copy-assignment operator and destructor and none of its nonstatic members is a non-POD class, array of non-POD, or a reference.

這段話基本上表達了幾件事：

+ 所有的 POD class 都是 Aggregate，換句話說，如果一個 class 不是 Aggregate，那它也不會是 POD
+ 即使術語是 POD-struct，class 也可以是 POD
+ 跟前面 Aggregate 的情況一樣，判斷標準基本上跟 class 內的 static member 無關

順便看一下比較新的例子 (C++17)：

> [n4659 (12 - 10)](https://timsong-cpp.github.io/cppwp/n4659/class#10)：
> A POD struct109 is a non-union class that is both a trivial class and a standard-layout class, and has no non-static data members of type non-POD struct, non-POD union (or array of such types). Similarly, a POD union is a union that is both a trivial class and a standard-layout class, and has no non-static data members of type non-POD struct, non-POD union (or array of such types). A POD class is a class that is either a POD struct or a POD union.

舉一些 POD 的例子：
```cpp
#include <iostream>
#include <vector>
#include <type_traits>

struct POD {
  int x;
  char y;
  void f() {}    // no harm if there's a function
  static std::vector<char> v;    // static members do not matter
};

struct AggregateButNotPOD1 {
  int x;
  ~AggregateButNotPOD1() {}    // user-defined destructor
};

struct AggregateButNotPOD2 {
  AggregateButNotPOD1 arrOfNonPod[3];    // array of non-POD class
};

int main()
{
  std::cout.setf(std::ios::boolalpha);
  std::cout << std::is_pod_v<POD> << '\n'    // true
            << std::is_pod_v<AggregateButNotPOD1> << '\n'    // false
            << std::is_pod_v<AggregateButNotPOD2> << '\n'    // false
            << std::is_pod_v<POD[3]> << '\n'    // true
            << std::is_pod_v<AggregateButNotPOD1[3]>;    // false
}
```

而對於 POD-class、POD-union、scalar types 和它們的 array，C\+\+ 給了他們一個名字叫 POD type。

> [n4659 (6.9 - 9)](https://timsong-cpp.github.io/cppwp/n4659/basic.types#9)：
> Arithmetic types, enumeration types, pointer types, pointer to member types, std::nullptr_t, and cv-qualified versions of these types are collectively called scalar types. Scalar types, POD classes, arrays of such types and cv-qualified versions of these types are collectively called POD types.

### POD Type 的好處

POD 的好處就很多了，這邊舉幾個例子：

+ POD-class 與 C struct 非常接近，但 POD class 可以有 member function、static member，不過這兩者並不會影響 memory layout。 所以如果你想要寫一個給 C 或甚至 .NET 用的 portable dll，你就需要讓你 exported function 的參數和回傳值都使用 POD-type，簡單來說就是 POD 對序列化很有幫助。

+ non-POD class 的生命週期從 constructor 完成開始，到 destructor 完成時結束；POD-class 的生命週期則是從物件占用記憶體開始，不用等到建構子執行完畢，而生命週期也是到其釋放記憶體結束。

+ 對於 POD types 的物件，標準保證其可以直接使用 `memcpy`，當你將 POD-class 的內容使用 `memcpy` 複製到 char/unsinged char array，再對陣列使用 `memcpy` 把內容複製回物件時，物件會維持原先的值，內容不變。 這件事對 non-POD type object 是沒有保證的。

  看個[例子](https://godbolt.org/z/YheTjYPeP)：

  ```cpp
  #include <iostream>
  #include <type_traits>
  #include <cstring>

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
  ```

+ goto 可以跳過 POD-type 物件的宣告，進入宣告後段的程式碼：

  ```cpp
  int f()
  {
    struct NonPOD {
      NonPOD() {}
    };

    goto label;    // error, x haven't been declared
    NonPOD x;
  label:
    return 0;
  }

  int g()
  {
    struct POD {
      int i;
      char c;
    };

    goto label;    // okay since x is POD
    POD x;
  label:
    return 0;
  }
  ```

+ POD-type 物件的第一個成員位址會和物件本身的位址一樣，舉個[例子](https://godbolt.org/z/n4rYjMMzd)：
  ```cpp
  #include <iostream>
  #include <cassert>

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
    assert(((void) "Not equal", reinterpret_cast<int *>(ptr2) == &(ptr2->i3)));    // error
  }
	```

## malloc

由於 windows 不好分析，這邊以 Linux 環境為主來講 malloc 的原理。

這部分要講很深的話會牽扯到整個 Linux 的記憶體管理，但這並不是我們的重點，所以我盡量不要講太多，提到必要的東西就好，

### 記憶體分配

記憶體的分配分為動態分配與靜態分配，靜態分配發生在編譯與 linking 時期，而動態分配則是在程式調入和執行的時候發生的。

# new

## std::set_new_handler

# Placement new

Placement new 本身是一個能夠幫助我們使用 memory pool 的工具，它能夠在預先配置好的一段記憶體建構物件。

這在某些時刻特別有用，如監聽程式、伺服器程式、或是要實作如 `std::vector` 這類高效能的容器時。因為這類東西，記憶體配置與釋放的頻率很高，若此時使用傳統的 new 與 delete 來配置與釋放記憶體，可能會有效率上的疑慮(new 在申請時需要查表)，也有可能會造成 memory fragmentation 等問題。

甚至有時候有些核心程式不允許記憶體申請失敗，我們希望每次的記憶體申請都是成功的，這種極端的要求下，使用 memory pool 的好處就很大。

# new operator 與 operator new

首先，new operator/delete operator 為運算子，而 operator new/operator delete 為函式。

Operator new 是一個幫忙配置 raw memory 的「函式」，它可以被 overload，同樣的 operator delete 也可以，如：

```cpp
#include <iostream>

// class-specific allocation functions
struct X
{
  static void* operator new(std::size_t count)
  {
    std::cout << "custom new for size " << count << '\n';
    return ::operator new(count);
  }

  static void* operator new[](std::size_t count)
  {
    std::cout << "custom new[] for size " << count << '\n';
    return ::operator new[](count);
  }
};

int main()
{
  X* p1 = new X;
  delete p1;
  X* p2 = new X[10];
  delete[] p2;
}
```

operator new 也有可能是透過一般的函式呼叫形式來呼叫的；呼叫的形式有很多種，可以到這邊參考：[operator new, operator new[ ]](https://en.cppreference.com/w/cpp/memory/new/operator_new)。

而 new operator 就是我們平常用來分配記憶體的 operator，如

```cpp
int i = new int(5);
```

要注意 new operator 不能被 overload，挺合理的，因為它是 operator。

使用 new operator 時它會先去<span class = "yellow">呼叫 operator new</span>，幫我們分配足夠的 raw memory，再去<span class = "yellow">呼叫對象的建構子</span>。

# Placement new

Placement new 是 operator new 的一種標準的 global overload，它不像一般的 operator new 可以被替換掉。

它的形式主要長：

```cpp
void* operator new  ( std::size_t count, void* ptr );
void* operator new[]( std::size_t count, void* ptr );
```

不過 placement new 在執行時會忽略第一個 `size_t` 的參數，並回傳第二個參數。

placement new 的用處是於 `ptr` 處建構我們想要的物件，通常我們會於前方先分配好 memory pool，並將其指標傳入，像是：

```cpp
// within any block scope...
{
  // Statically allocate the storage with automatic storage duration
  // which is large enough for any object of type `T`.
  alignas(T) unsigned char buf[sizeof(T)];

  T* tptr = new(buf) T; // Construct a `T` object, placing it directly into your 
                        // pre-allocated storage at memory address `buf`.

  tptr->~T();           // You must **manually** call the object's destructor
                        // if its side effects is depended by the program.
}                       // Leaving this block scope automatically deallocates `buf`.
```

此處的 `alignas(T) unsigned char buf[sizeof(T)];` 就為我們分配的 memory pool，大小為一個 `T` 的大小，前方的 `alignas` 是幫助對齊的，不是我們這邊的重點，而這裡使用 `unsigned char` 的原因是因為 `sizeof(char) == 1`，你也可以用 `char` 或 `std::byte` 等等，我自己傾向使用 `std::byte`。

需要注意的是物件使用完畢後需要自己手動呼叫解構子，所以上例使用了 `tptr->~T();`，如此一來前面建構的物件會被解構，但先前分配好的 `buf` 不會被釋放。

# 參考資料

**<a href = "https://www.dsi.fceia.unr.edu.ar/downloads/informatica/info_II/c++/Effective%20C++%20+%20More%20Effective%20C++.pdf" class = "redlink">1. Effective C++</a>**

**<a href = "https://stackoverflow.com/questions/184537/in-what-cases-do-i-use-malloc-and-or-new" class = "redlink">2. In what cases do I use malloc and/or new?</a>**

**<a href = "https://stackoverflow.com/questions/146452/what-are-pod-types-in-c" class = "redlink">3. What are POD types in C++?</a>**

**<a href = "https://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special/7189821#7189821" class = "redlink">4. What are Aggregates and PODs and how/why are they special?</a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**