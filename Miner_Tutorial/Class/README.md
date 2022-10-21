<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 ── Class <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>


點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 前言

Class 背後最核心的概念是資料的抽象化(data abstraction)與資料封裝(data encapsulation)。這些是一種程式設計上的技巧、想法，其清楚區分了介面(interface)和實作(implementation)的不同，介面是與使用者直接互動的運算，實作則是利用了 Class 內的資料成員與成員函式來寫出的演算法，而封裝則是強制分開了介面與實作，實作會被隱藏，使用者只看的到介面，但看不到實作。

舉個例子，有一個肯x雞的自動販賣機，可以讓使用者購買炸雞，使用者可以利用販賣機上的按鈕來購買炸雞，但使用者不會知道炸雞是怎麼被做出來的，此時這個販賣機就是一種封裝，介面為機器上的按鈕，而實作就是製作炸雞的細節。

封裝的目的是為了將程式切分為很多模組，盡量讓模組與模組之間沒有關聯，有點像模組化設計的感覺，而封裝出來的東西我們就稱為 Class，以上例來說，這個販賣機就會是一個 Class。

額外閱讀：[What is the big difference between modular and object oriented programming?](https://stackoverflow.com/questions/18034683/what-is-the-big-difference-between-modular-and-object-oriented-programming)

# 類別(Class)

因此 Class 你可以把他想像為一個設計藍圖，設計者可以利用這個藍圖來去建立物件，換句話說 Class 是一種使用者定義的型態，

> n4868(11.1)：A class is a type. Its name becomes a class-name within its scope.

## Class Declaration 與 Class Definition

Class Declaration 與 Class Definition 都是屬於 Simple Declaration 中的 Declaration Specifier，幫大家複習一下，一個 Simple Declaration 的形式會長：

> decl-specifier-seq init-declarator-list(opt);

所以就是前面 decl-specifier-seq 那部分。

Class Declaration 的意思為我們要引入 Class 的名字，告訴編譯器這是一個我們自己定義的型態，語法很簡單，會在最一開始加上 `class` 關鍵字，後面接上名字：

```cpp
class T;
```

這樣的話 `T` 就是一個 Class，但尚未有 Class 的定義，此時這個 `T` 是個不完整的型態(incomplete type)，如果要使其成為 complete type，就要定義 Class，要定義 Class 的話我們需要在後面接上大括號 `{}`，語法像這樣：

```cpp
class { member-specification(opt) }
```

大括號內填的是 Class 成員的資訊，成員的所有權與 scope 屬於這個 Class，可以有 function、object declaration、using、enum 等([一覽](https://timsong-cpp.github.io/cppwp/n4868/class#nt:member-declaration))，一樣用前面販賣機的例子：

```cpp
#include <iostream>

class Vending_machine {
public:
  void ordered_chicken()
  {
    std::cout << "the machine get your order\n";
    price = 50;
  }

  int money()
  {
    return price;
  }

private:
  int price;
};
```

這個 Class 叫做 Vending_machine，裡面有兩個 member function 與一個 data member，其中有兩個關鍵字 `public` 與 `private`，`public` 代表可以給使用者使用的成員，`private` 表示只有自己這個 Class type 可以使用的成員，但同一種 Class type 的不同 object 是可以使用 private 成員的：
```cpp
#include <iostream>

class T {
  int val;

public:
  T()
      : val() {}

  void set_other_val(T &other) { other.val = 100; };
  int get_val() { return val; }
};

int main()
{
  T t1, t2;
  t1.set_other_val(t2);
  std::cout << t2.get_val();
}
```

還有一個關鍵字叫 `protected`，表示繼承的子類和自己可以使用的成員，先記前面兩個就好，第三個在之後講繼承時會提到。

成員的宣告與定義也可以分開來寫，我們通常會在 `.h` 內寫 member 的宣告，並在對應的 cpp file 內寫定義，舉個例子：

在 Vending_machine.h 內：

```cpp
#ifndef VENDING_MACHINE
#define VENDING_MACHINE

class Vending_machine {
public:
  void ordered_chicken();
  int money();

private:
  int price;
};

#endif
```

在 Vending_machine.cpp 內：

```cpp
#include "Vending_machine.h"
#include <iostream>

void Vending_machine::ordered_chicken()
{
  std::cout << "the machine get your order\n";
  price = 50;
}

int Vending_machine::money()
{
  return price;
}
```

要注意的是我們需要透過範圍解析運算子 `::` 來定義成員函式，否則編譯器會以為我們在定義的是一般的函式。

這樣的話我們就設計好了一張「藍圖」、「設計圖」，我們可以利用這個設計圖去建構物件：

```cpp
int main()
{
  Vending_machine machine1, machine2;

  machine1.ordered_chicken();
  std::cout << machine1.money();
}
```

這裡我們建構了兩個販賣機出來，Class 的物件可以透過成員訪問運算子 `.` 來使用 Class 中存取權限為 `public` 的成員，以上面這個例子，我們使用了第一台機器的 `ordered_chicken()` 這個函式，又再使用了 `money()` 這個函數來確認金錢。

## 建構子(Constructor) 與解構子(Destructor)

在前面的章節我們很強調建構與賦值不同，大家可能沒什麼感覺，但在這裡就會很強烈的發現兩者的不同了。

一個 Class type 的物件在建構時可能會有預設的狀態，以上方的販賣機為例，一開始 price 需要初始化為 0，否則可能為亂數，此時就要利用建構子來操作。

建構子的語法和一般的函式不同，其沒有回傳型態，也沒有 function name，並且有一個特殊的組件叫做初始化清單(initialization list)，可以幫助我們初始化物件，速度會比較快，建構子的語法長這樣：

```
Class名() initialization_list(opt) {}
```

舉個例子：

```cpp
class T {
public:
  T() {}
};

class T2 {
public:
  T2() : val(5) { std::cout << "build T2\n"; }

private:
  int val;
};
```

Class `T` 的建構子沒有任何內容，也沒有 initialization list，這種建構子我們稱他為預設建構子(default constructor)。

而 `T2` 的建構子有 initialization list，將 `val` 初始化為 5，因此如果我們去訪問一個剛建構出來的 `T2` 的 `val`，就會得到 5：

```cpp
#include <iostream>

class T2 {
public:
  T2()
      : val(5) {}
  void print_val() { std::cout << "val = " << val << '\n'; }

private:
  int val;
};

int main()
{
  T2 obj;
  obj.print_val();    // val = 5
}
```

但要注意初始化清單並沒有指定成員初始化的順序：
```cpp
#include <iostream>

class A {
public:
  A() { puts("A()"); }
};

class B {
public:
  B() { puts("B()"); }
};

class C {
public:
  C() { puts("C()"); }
};


class T {
  A a;
  B b;
  C c;

public:
  T()
      : c(), b(), a() {}    // A() B() C()
};

int main()
{
  T t1;
}
```

初始化的順序不是透過初始化清單來決定，而是透過 non-static data member 被宣告的順序來決定，所以這裡初始化的順序是 A->B->C。

> [11.9.3](https://eel.is/c++draft/class.base.init#13.3)：Then, non-static data members are initialized in the order they were declared in the class definition (again regardless of the order of the mem-initializers).

而在解構時也有解構子可以使用，例如原本有在 Class 內動態配置記憶體，那解構時就要幫忙呼叫 delete，解構子的語法為：

```cpp
~Class名(){}
```

舉個例子

```cpp
class T {
public:
  T()
      : ptr(new int) {}

  ~T() { delete ptr; }

private:
  int *ptr;
};
```

我們也可以傳遞參數進去建構子，根據傳遞進去的參數不同，有分移動建構子與複製建構子，同樣的我們可以透過 `operator=` 的 overload 來建立複製運算子與移動運算子，這些會在複製、移動 Class 的實例時用到：

```cpp
#include <iostream>

class T {
public:
  T()
      : val() { std::cout << "calling T()\n"; }    // default constructor

  T(const T &other)
      : val(other.val) { std::cout << "calling T(const T &other)\n"; }    // copy constructor

  T(T &&other)
      : val(std::move(other.val)) {}    // move constructor

  T &operator=(const T &other)
  {
    std::cout << "calling operator=(const T &other)\n";
    val = other.val;
    return *this;
  }    // copy operator=
    
  T &operator=(T &&other)
  {
    std::cout << "calling operator=(T &&other)\n";
    val = std::move(other.val);
    return *this;
  }    // move operator=

  ~T() {}    // destructor

private:
  int val;
};

int main()
{
  T t1;
  T t2 = t1;
}
```

### Rule of three、Rule of five、Rule of zero

而當我們自己定義了解構子、複製建構子或複製運算符時，通常就代表我們需要自己管理 Class 的內部資源，因此就會需要同時定義這三個建構子，這就是 R3，而如果考慮了移動進去，則多了移動建構子與移動運算子進去，因此是 R5，而如果都沒有動到，就一個都不用寫，這就是 R0。

# this

`this` 是一個特殊的指標，基本上會出現在 class 的 non-static member function 內，`this` 會指向呼叫他的 member function 所屬的 class instance。

舉個例子：
```cpp
#include <iostream>

class T {
public:
  void print_this()
  {
    std::cout << this << "\n";
  }
};

int main()
{
  T t1, t2;
  std::cout << &t1 << " ";
  t1.print_this();

  std::cout << &t2 << " ";
  t2.print_this();
}
```

這個 class instance，也就是呼叫的物件有個名字叫 implicit object。

> [7.5.2](https://eel.is/c++draft/expr.prim.this#1)：The keyword this names a pointer to the object for which an implicit object member function ([class.mfct.non.static]) is invoked or a non-static data member's initializer ([class.mem]) is evaluated.

在 parameter 與 data member 名稱衝突時，我們需要 `this` 來告訴 compiler 說我們使用的是哪個變數：

```cpp
#include <iostream>

class T {
public:
  void print_var(int var)
  {
    std::cout << this->var << " " << var << "\n";
  }

  T()
      : var() {}

  int var;
};

int main()
{
  T t1;
  t1.print_var(100);
}
```

在需要回傳自己或自己的位址時，我們也會透過 `this` 來回傳：

```cpp
class T {
public:
  T &self_obj() { return *this; }
  T *self_addr() { return this; }

  T()
      : var() {}

  int var;
};
```

這在前一小節的 `operator=` 也可以看到。

對於 member function 可以使用 `this` 這件事，有一個 compiler 很常用的實作方式，應該說到現在我只看過這種方式，其會把 `this` 當作第一個參數傳入 member function：

```cpp
#include <iostream>

class T {
public:
  void fn() { std::cout << this; }
};

int main()
{
  T t1;
  t1.fn();
}
```

在 `t1.fn()` 這裡，通常 compiler 會改寫為 `t1.fn(&t1)`，如此一來就可以實作 `this` 了，但要注意這不是標準，我一開始一直以為這是標準規定的，後來花了很多時間去理解才發現這件事，標準只規定了行為，說了 `this` 是一個 prvalue pointer，值是 implicit object 的位址，並沒有規定如何實作。

# friends

前面說了權限為 private 的成員只有同一個 class type 的 object 能使用，如果今天有外部的函式或 class 想存取 private 成員時，可以將其設為 friend，使用方法如下：

```cpp
#include <iostream>

class T {
  int val;

public:
  T()
      : val() {}

  friend void set_val(T &obj, int i) { obj.val = i; }    // define in class
  friend int get_val(T &obj);    // define outside
};

void set_val(T &obj, int i);
int get_val(T &obj)
{
  return obj.val;
}

int main()
{
  T t1;
  set_val(t1, 20);
  std::cout << get_val(t1);
}
```

我們需要在外部函式或 class 的前面加上 `friend` 關鍵字，而其一樣可以在 class 內或外定義，要注意的是我們需要在外面宣告它，否則由於 scope 的關係，於 class 外面我們無法存取它，而因為它是外部函式或 class，本身並不屬於 class 的成員，因此也沒辦法透過 `::` 來存取。

# 運算子重載 (operator overload)

當我們要定義一個運算子如何操作自定義的 class 時，我們可以使用運算子重載，在上面建構子的地方我們其實已經用過運算子重載了，那邊重載了 `=`，改變了 `=` 的行為，這邊再多舉一個例子，假設有個 2\*2 矩陣的 class，我們要定義矩陣的加法：

```cpp
#include <iostream>

class M {
  int arr[2][2];

public:
  M()
      : arr{ 0, 0, 0, 0 } {}

  M(int a, int b, int c, int d)
      : arr{ a, b, c, d } {}

  M(int (&other)[2][2])
      : arr{ other[0][0], other[0][1], other[1][0], other[1][1] } {}

  M &operator+(M &other)
  {
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 2; ++j)
        arr[i][j] += other.arr[i][j];

    return *this;
  }

  void print_arr()
  {
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 2; ++j)
        std::cout << arr[i][j] << " ";

    std::cout << '\n';
  }
};

int main()
{
  M m1{ 4, 5, 6, 7 }, m2{ 1, 2, 3, 4 };
  m1 + m2;

  m1.print_arr();
}
```

# const、mutable

如果一個 class instance 有被 `const` 修飾，此時若有用到沒有用 `const` 修飾的 member function 便不會通過編譯。

對 member function 加上 `const` 修飾的方法很簡單，在 function body 與 parameter list 之間加上 `const` 就好，這代表此 member function 無法對其 data member 進行改動，但改別人的 data member 是沒問題的：

```cpp
#include <iostream>

class T {
  int val;

public:
  T()
      : val() {}

  T(int i)
      : val(i) {}

  // error
  //  int get_val() { return val; }
  int get_val() const { return val; }

  // error
  // void set_val(int i) const { val = 5; }
  void set_other(T &other) const { other.val = 5; }

  // error
  // T &get_obj() { return *this; };
  // const T &get_obj() { return *this; };
  // T &get_obj() const { return *this; };
  const T &get_obj() const { return *this; };

  T &get_other(T &other) const { return other; }
};

int main()
{
  const T t1;
  T t2;

  t1.get_val();
  t1.set_other(t2);
  t1.get_obj();
  t1.get_other(t2);
}
```


要注意的是如果有 return `*this` 的，那在回傳的型態上也需要加上 `const`。

我們可以利用這個特性來實作 overloading：

```cpp
#include <iostream>

class T {
  int val;

public:
  T()
      : val() {}

  T(int i)
      : val(i) {}

  const T &check_version() const
  {
    std::cout << "const version\n";
    return *this;
  };

  T &check_version()
  {
    std::cout << "without const version\n";
    return *this;
  };
};

int main()
{
  const T t1;
  T t2;

  t1.check_version();    // const version
  t2.check_version();    // without const version
}
```

這個 overload 是利用作用在 member function 的 `const` 來判斷的，也就是說上例中的第二個 `check_version` 的 return type 就算加上 `const` 也是沒問題的：
```cpp
// can still complete the overloading
const T &check_version()
{
  std::cout << "without const version\n";
  return *this;
};
```

然而有些特殊狀況我們會需要在有 `const` 修飾的 member function 內修改某些特殊的變數，此時我們需要在這個特殊的變數上加上 `mutable` 修飾，這種變數不能有 `const` 修飾，因為他的值一定要可以被改：

```cpp
#include <iostream>

class T {
  mutable int val;

  // error
  // const mutable int val2;
  
public:
  void set_val(int i) const { val = i; }
};

int main()
{
  T t1;
  t1.set_val(5);
}
```

# 自定義的型態轉換 (user-defined conversion)

有時我們自己定義的 class 可以與其它的型態互相轉換，此時我們就要自定義轉型的函式，語法是這樣：

> explicit(opt) operator conversion-type-id() {}

會有一個 `operator` 關鍵字，後面接上我們要轉換的型態。

假設我們的 `T` 可以轉換為 `int`，那麼我們可以這樣寫：

```cpp
#include <iostream>

class T {
  int val;

public:
  T()
      : val(20) {}

  operator int() const { return val; }
};

int main()
{
  T t1;
  std::cout << t1 + 5;    // 25
}
```

可以看見在第 16 行的地方 `t1` 被隱式轉換為 `int` 了，因此輸出 25。

如果我們不想要有隱式轉換，則需要加上 `explicit` 關鍵字在轉換函式的前方：
```cpp
#include <iostream>

class T {
  int val;

public:
  T()
      : val(20) {}

  explicit operator int() const { return val; }
};

int main()
{
  T t1;
  // error
  // std::cout << t1 + 5;

  std::cout << static_cast<int>(t1) + 5;    // 25
}
```

加上 `explicit` 關鍵字的轉換函式，compiler 不會自動套用它，需要我們加上 cast operator 時才會去套用轉換，像 19 行那樣。

# static

`static` 有主要會出現在三個地方：

1. global variable/function 前
2. function 內的 varaible 前
3. class 的 member variable/function 前

這三個地方的 `static` 所代表的意義會不太一樣，這邊就由上往下來一一解釋

## global variable/function 前

在前面我們有提到 TU(.o 檔) 的概念，一個 TU 會有一組自己的 symbol table 來記錄哪些 symbol 需要從外部取得，這個 table、symbol 我們稱它為 ABI(Application Binary interface)。

在 global variable/function 前加上 `static` 的意義代表接下來的這個變數/函式只能在這個 TU 內使用，舉個例子：

test.cpp：

```cpp
static int i = 0;

void fn();

int main()
{
  fn();
}
```

test2.cpp：

```cpp
extern int i;

void fn()
{
  ++i;
}
```

第一個檔案中有個 `static` 關鍵字修飾的整數 `i`，而我們在第二個檔案中想要使用這個 `i`，我們先把兩個 `.o` 檔編譯出來：

```bash
g++ -c test1.cpp
g++ -c test2.cpp
```

然後去 link 這兩個檔案，生成執行檔：
```
gcc -o output test.o test2.o
```

此時 linker 就會報錯，告訴你 undefined reference to `i`：

![](https://i.imgur.com/codc4xj.png)

這是由於當 `test2.o` 要去找 `i` 的定義時他找不到，因為我們把 `i` 限定在 `test.o` 內了。

這邊可以看一下 ABI 實際上長的樣子，你會發現有些差別，指令是
```
g++ -S -o output.asm input.cpp
```

這邊我把輸出結果丟到 github 上了：[普通版本](https://github.com/Mes0903/Cpp-Miner/blob/main/Miner_Tutorial/Class/normal.asm)、[extern 版本](https://github.com/Mes0903/Cpp-Miner/blob/main/Miner_Tutorial/Class/extern.asm)、[static 版本](https://github.com/Mes0903/Cpp-Miner/blob/main/Miner_Tutorial/Class/static.asm)。

對比一下你會發現在 `extern` 版本，多了一行 `def	_Z2fnv;	.scl	2;	.type	32;	.endef`，表示他要去找定義，我們之前也提過 `extern` 修飾的變數會只有宣告，並且要從其他 TU 裡面去找定義；

而 `static` 版本則是在最一開始的地方少了 `.globl i`，表示 `i` 這個變數不再是所有 TU 都可以使用的了，後面也少了一行 `.globl _Z2fnv`。

## function 內的 varaible 前

如果 `static` 出現在 function 內的變數前，表示這個變數為<span class = "yellow">靜態變數</span>，生命週期要到程式結束才會跟著結束：

```cpp
#include <iostream>

void fn()
{
  static int i = 0;
  std::cout << ++i << '\n';
};

int main()
{
  fn();    // 1
  fn();    // 2
  fn();    // 3
}
```

因為 `fn` 中的 `i` 變為一個靜態變數，因此離開 `fn` 時 `i` 並不會解構，所以上例輸出的結果為 `1 2 3`。

## class 的 member variable/function 前

利用 `static` 修飾的 member variable/function 所有權並不屬於任何一個 instance，所有的 class instance 會共用這個 member，因此有 `static` 修飾的 member variable 不能利用建構子來初始化，需要在外部定義它：

```cpp
#include <iostream>

class T {
public:
  static int i;

  void fn()
  {
    std::cout << ++i << '\n';
  }

  static void s_fn()
  {
    std::cout << "s_fn\n";
  }
};

int T::i = 5;    // definition of T::i

int main()
{
  T::s_fn();    // okay
  T::i = 0;    // okay

  T t1, t2, t3;
  t1.fn();    // 1
  t2.fn();    // 2
  t3.fn();    // 3
}
```

由於 static member 不屬於任何一個 class instance，因此我們可以直接使用它，這是一個特例，如上例中的 22、23 行。

static data member 的生命週期也是靜態的，它被定義後會一直存在到程式結束。另外，static data member 可以做為 default-argument 來使用，但 non-static data member 不行：

```cpp
class T {
public:
  static int i;

  void fn(int j = i)    // okay, static data member as default argument.
  {
    std::cout << ++i << '\n';
  }
};
```

> [6.7.2](https://eel.is/c++draft/intro.object#2)：Objects can contain other objects, called subobjects. A subobject can be a member subobject ([class.mem]), a base class subobject ([class.derived]), or an array element. 

> [11.4.9](https://eel.is/c++draft/class.static#data-1)：A static data member is not part of the subobjects of a class.

# default member initializer

在 C++11 後，我們可以使用 default member initializer 來初始化 non-static data member，也有人稱這方法為 in-class initializer 或 non-static data member initializer，寫法很簡單，直接在 data member 的後方加上初始化器就可以了：

```cpp
#include <iostream>

class T {
public:
  int i = 0;
};

int main()
{
  T t1;
  std::cout << t1.i;    // 0
}
```

如果 static data member 要使用 in-class initializer，則它需要是被 const 修飾的：

```cpp
class T {
public:
  static const int i = 0;    // okay
};
```

如果有建構子也去初始化了這個變數，那麼初始化器會被跳過：
```cpp
class T {
public:
  int i = 0;

  T()
      : i(2) {}    // initializer i by initialization list,
                   // default member initializer of i was skip
};

int main() {
  T t;    // t.i is 2
}
```

然而這有時會讓邏輯變得有些複雜，尤其是你有多個建構子的時候，因為你會需要不停的判斷它是使用建構子的清單初始化還是預設初始化器的初始化，如果少寫或少看就會造成杯具，所以使用的時候自己要注一下。

> [11.9.3-9](http://www.eel.is/c++draft/class#base.init-9)：
>
> In a non-delegating constructor other than an implicitly-defined copy/move constructor ([class.copy.ctor]), **if a given potentially constructed subobject is not designated by a mem-initializer-id** (including the case where there is no mem-initializer-list because the constructor has no ctor-initializer), then
> 
> (9.1)
>
> if the entity is a non-static data member that has a default member initializer ([class.mem]) and either
>
>> (9.1.1)
>> the constructor's class is a union ([class.union]), and no other variant member of that union is designated by a mem-initializer-id or
>
>> (9.1.2)
> the constructor's class is not a union, and, if the entity is a member of an anonymous union, no other member of that union is designated by a mem-initializer-id,
> the entity is initialized from its default member initializer as specified in [dcl.init];
    
<center>(感謝群裡的 DXPower 幫找 ><)</center><br>

額外閱讀：[C++11 member initializer list vs in-class initializer?](https://stackoverflow.com/questions/27352021/c11-member-initializer-list-vs-in-class-initializer)