---
title: 礦坑系列 ── Concept と SFINAE と Detection Idiom
date: 2022-12-01
tag: C++ Miner
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?" height = 50> 礦坑系列 ── Concept と SFINAE と Detection Idiom <img src = "https://i.imgur.com/thmVmX6.png?" height = 50></center></h1>

點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 前言

ぼっち・ざ・ろっく！ 實在太棒了，標題致敬一下ギターと孤独と蒼い惑星 XD

![](https://i.imgur.com/KGmaSgl.png)

Detection Idiom 中文翻譯為偵測語意，其他的名字還有 Member Detector 和 Template type constraint 等等。

主要的目的是偵測一個傳進 function template 的 argument 是否符合我們對 template parameter 設定的條件，之後看你要給 error 還是利用這個條件去做模板特化或分支，在對物件或函式做封裝的時候蠻好用的。

從這邊可以預見，這篇基本上都是在講模板的東西，對模板不熟的可以先去讀讀模板怎麼用，這篇會先簡單介紹一下 C\+\+20 的 Concept，再接著講 C\+\+20 前利用 SFINAE 的方法。

# Concept

C\+\+20 時新增了一個語言特性叫 Concept，它是一種 requires expression，可以對 template argument 設定需要符合的條件。 要做這件事，首先需要先定義一個 Concept，然後定義我們的條件。

舉個例子，我們想要偵測 template argument 可以進行加法運算，並回傳原本的型態要是一樣的，像是 `5+5` 會回傳 int，那我們可以這樣寫：

```cpp
#include <iostream>
#include <concepts>

// 定義一個 Concept，表示可以進行加法運算的 type
template <typename T>
concept Addable = requires(T a, T b)
{
  { a + b } -> std::convertible_to<T>; // 限制要有 a + b 的操作，且回傳型態要可以轉換為 T
};

// 一個可以進行加法運算的 class X
struct X {
  int data;
  X operator+(const X &other) const { return X(data + other.data); }

  X() = default;
  X(int i) : data(i) {}
};

// 限制 template argument 要符合 Addable 的條件
template <Addable T>
T add(T a, T b)
{
  return a + b;
}

int main()
{
  int n1 = 1, n2 = 2;
  X x1(1), x2(2);

  int n3 = add(n1, n2);
  X x3 = add(x1, x2);

  std::cout << n3 << ' ' << x3.data; // 3 3
}
```

簡單看一下語法：
```cpp
template <typename T>
concept concept_name = constraint_expression;
```

後面的 constraint_expression 通常會使用 `requires-clause` 來幫忙完成語法，所以會長：
```cpp
template <typename T>
concept concept_name = requires(T a, T b) {
  // 你的條件
}
```

條件的寫法也很簡單，直接把你想要限制的條件寫上去就好了，像是剛剛的可以相加：
```cpp
template <typename T>
concept Addable = requires(T a, T b) {
  a + b;
};
```

這樣代表 a 和 b 需要可以相加，如果還要額外限制回傳型態的話就加個大括號上去：
```cpp
template <typename T>
concept Addable = requires(T a, T b) {
  { a + b } -> std::convertible_to<T>;
};
```

`std::convertible_to<T>` 代表回傳的型態可以轉換為 `T`，不直接使用 `T` 是因為編譯器會不知道 `T&` 也可以嗎? 那 `const T`、`const T&` 或 `T&&` 呢? 所以通常我們會加上 `std::convertible_to<T>` 來表示可轉換 `T` 的都可以接受。

`std::convertible_to<T>` 是一個 Concept，我們這邊的意思等於是說 `a + b` 的回傳型態要滿足這個 concept，這樣的東西我們稱它為 return type requirement。

如果想要限制只有 `T` 才可以，那我們會使用 `std::same_as<T>` 來當回傳型態：
```cpp
template <typename T>
concept Addable = requires(T a, T b) {
  { a + b } -> std::same_as<T>;
};
```

有興趣的朋友可以看看 [P1452R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1452r2.html) 的討論。

我們也可以限制 template argument 有沒有某個特定的 member 或 member function，舉個例子，我們正在實作一個 ML Model，那麼它會需要有 `fit` 和 `predict` 這兩個 member function，和一個 confusion tabel，所以會有 4 個 `int`：
```cpp
#include <iostream>
#include <concepts>

template <typename T>
concept valid_model = requires(T a)
{
  a.TN, a.TP, a.FN, a.FP;
  { a.fit() } -> std::same_as<void>;
  { a.predict() } -> std::same_as<void>;
};

struct M1 {
  int TN, TP, FN, FP;
  void fit();
  void predict();
};

struct M2 {};

struct M3 {
  int TN, TP, FN, FP;
  [[noreturn]] int fit();
  [[noreturn]] int predict();
};

template <valid_model T>
void check(T a)
{
  puts("ok!");
}

int main()
{
  M1 m1;
  M2 m2;
  M3 m3;

  check(m1);
  // check(m2); // error: no matching function for call to 'check(M2&)'
  // check(m3); // error: no matching function for call to 'check(M3&)'
}
```
![](https://i.imgur.com/ED73ecV.png)

可以看見錯誤訊息變得十分乾淨，debug 變的很方便。

## Conjunction 與 Disjunction

Concept 可以進行組合，所謂的 Conjunction 指的就是邏輯上的 and，而 Disjunction 指的則是邏輯上的 or，以前面 `valid_model` 的例子來說，可以將它細分為 `has_fit`、`has_predict` 與 `has_confusion` 三種 concept，再由 `valid_model` 對這三個進行組合：
```cpp
template <typename T>
concept has_fit = requires(T t) 
{
  { t.fit() } -> std::same_as<void>;
};

template <typename T>
concept has_predict = requires(T t) 
{
  { t.predict() } -> std::same_as<void>;
};

template <typename T>
concept has_confusion = requires(T t) 
{
  t.TN, t.TP, t.FN, t.FP;
};

template <typename T>
concept valid_model = has_fit<T> && has_predict<T> && has_confusion<T>;
```

當然因為這個例子很簡單，所以看起來會有點多此一舉，但對於比較複雜的例子就會需要它了。

再舉個例子，今天有個 function template 只想要接受 `int` 和 `char` 兩種參數，那麼我們可以這樣做：

```cpp
#include <iostream>
#include <concepts>

template <typename T>
concept valid_t = std::same_as<int, T> || std::same_as<char, T>;

template <valid_t T>
void check(T a)
{
  puts("ok!");
}

int main()
{
  int i;
  char c;
  bool b;
  check(i);
  check(c);
  // check(b); // error: error: no matching function for call to 'check(bool&)'
}
```

cppreference 上有個 `println` 的例子，也可以看看：
```cpp
#include <concepts>
#include <iostream>

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T>
concept IsPrintable = std::integral<T> || std::floating_point<T> ||
                      IsAnyOf < std::remove_cvref_t<std::remove_pointer_t<std::decay_t<T>>>,
                      char, wchar_t > ;

void println(IsPrintable auto const... arguments)
{
  (std::wcout << ... << arguments) << '\n';
}

int main()
{
  println("Example: ", 3.14, " : ", 42, " : [", 'a', L'-', L"Z]");
}
```

到這裡我們把 concept 的基本操作講了一遍，可以看見概念和語法其實都很簡單，但畢竟並不是每個人環境都有 C\+\+20，因此接下來就繼續講 C\+\+20 前要怎麼做 Detection Idiom

# 背景知識

## 前言

C++20 前要做 Detection Idiom 需要使用 function overloading，而 SFINAE 則是針對 function template overloading 做的~~億些規則~~，因此我們要先搞懂 SFINAE 的概念與 function overloading 的大致流程

SFINAE 全名為 Substitution failure is not an error，中文就直翻叫替換失敗並非錯誤，直接看名字有點難懂，我們先複習一下 function overloading。

有讀過 C++ 的朋友可能知道 Compiler 會利用 function parameter type 的不同來進行 function overloading，看個例子：

```cpp
#include <iostream>

void fn(int i)
{
  puts("int");
}

void fn(char c)
{
  puts("char");
}

int main()
{
  int i = 0;
  char c = 'c';

  fn(i);
  fn(c);
  fn(0);
  fn('c');
}
```

對於同樣 function name 的 function call，Compiler 會透過 return type 與 argument type 來判別要呼叫哪個 function，不熟的朋友可以看看之前的文章：[函式重載 Function Overloading](https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FMinerT_function_and_memory#%E5%87%BD%E5%BC%8F%E9%87%8D%E8%BC%89-Function-Overloading)。

在這裡，你大概就可以感受到那個味道了，`fn(c)` 在嘗試 `fn(int i)` 時失敗了，但因為還有 `fn(char c)`，因此並沒有給出 compile error，只有當完全沒有可以使用的 function 時才會給出 compile error。

換句話說你可以把他理解為類似 switch case 的存在，像是這樣：
```cpp
switch (argument type) {
  case void (char) :
    call void fn(char c);
  case void (int) :
    call void fn(int i);
  default :
    give compile error;
}
```

但實際上當然沒這麼簡單，所以我們在這邊要提一下 Overload Resolution，因為我們要做的事基本上就是 Overload Resolution

## Overload Resolution

Overload Resolution 中文叫多載解析，它還有另一個名字叫做 Function Matching，中文為函式匹配，從名字可以看出來他就是在解析函式多載的過程，目的是在有多個同名的函式時決定要呼叫哪個函式。

標準裡面花了很大篇幅來講 Overload Resolution，因為其包含的面向很多，大致上有 name lookup、ADL、types、value categories、declarations、conversions、template argument deduction、SFINAE 等，頁數真的非常多，我也只是大概讀了幾個重點章節和 committee paper 而已，實際上等真的需要確認結果的時候才會去詳細翻如何推斷。

但那些篇幅有很大一部分都是在講解詳細的推斷細節，實際上推斷是有個大致流程的，主要分四個步驟：

1. 利用 function name lookup 建立 overload set
    這會把所有 visible 的 function declaration 找出來，建立出一個 overload set，過程中它可能會需要 ADL，對於 function template 可能還會需要做 template argument deduction。

    這個階段只要 function name 一樣就好，argument 沒有符合並沒有關係，舉個例子：
    ```cpp
    void fn(int i) {}
    void fn(char i) {}
    void fn() {}
    void fn(int, int) {}

    int main()
    {
      fn(1);
    }
    ```
    這裡四個 `fn` 都會被加到 overload set 內。

2. 建立候選人清單 (candidate set)
    此時 compiler 會判斷 overload set 內的 function 是否為 viable 的，viable 的[條件](https://eel.is/c++draft/over.match#viable)如下：
    1. argument 數量正確
    2. 如果呼叫的 argument 數量多於 function parameter 數量，那 function parameter 需要有 ellipsis parameter，就是 C 裡面也有的那個 `...`，像是 `int printz(...);`。
    3. 如果呼叫的 argument 數量小於 function parameter 數量，那 function parameter 需要有 default parameter，像是 `int fn(int = 0);`。
    4. 如果 function 有額外的 constraint，那需要符合其 constraint
    5. argument 的型態要對，armument 可能有 [implicit conversion sequence](https://eel.is/c++draft/over.match#def:conversion_sequence,implicit) 存在，也就是說如果 argument 轉型可以傳進 function 那也算對

    沿用前面的例子：
    ```cpp
    void fn(int i) {}
    void fn(char i) {}
    void fn() {}
    void fn(int, int) {}

    int main()
    {
      fn(1);
    }
    ```

    這裡的 candidate set 為 `void fn(int i) {}` 與 `void fn(char i) {}` 兩個。
    :::info
	overload set 是一般的用語，candidate set 也是一般的用語，並非 standard 的用語，儘管 standard 有出現這些字眼，但並沒有詳細的定義，不是 C++ 內的專有名詞。
	:::

3. ranking，找 best overload
  	如果 candidate set 只有一個 function，那麼就會直接呼叫它，如果有多個，則進行 ranking，選出一個最佳的 function 來呼叫。

  	若沒有 best overload，則會給出 compiled error。

    要注意的是這個「最佳」對你來說不一定是最佳的 function，它只不過是 compiler 依照標準訂好的選出的 function。

    到這裡主要的目的是從 candidate set 裡面挑一個最好的，換句話說這裡的每一個 function 都是 viable 的，所以主要用轉型來決定。

    function argument 的轉型分三種，依照 ranking 高至低分別為：[Standard conversion sequences](https://timsong-cpp.github.io/cppwp/n4868/over.match#over.ics.scs)、[User-defined conversion sequences](https://timsong-cpp.github.io/cppwp/n4868/over.match#over.ics.user)、[Ellipsis conversion sequences](https://timsong-cpp.github.io/cppwp/n4868/over.match#over.ics.ellipsis)

    而 Standard conversion sequences 自己還有分 [ranking](https://timsong-cpp.github.io/cppwp/n4868/over#ics.scs-3)：
    1. Exact Match
        1. Identity
            不需要 Conversion
        2. Lvalue Transformation
            + [Lvalue-to-rvalue conversion](https://timsong-cpp.github.io/cppwp/n4868/conv.lval)
            + [Array-to-pointer conversion](https://timsong-cpp.github.io/cppwp/n4868/conv.array)
            + [Function-to-pointer conversion](https://timsong-cpp.github.io/cppwp/n4868/conv.func)
        3. Qualification Adjustent
            + [Qualification conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.qual)
            + [Function pointer conversion](https://timsong-cpp.github.io/cppwp/n4868/conv.fctptr)
    2. Promotion
        1. [Integral promotions](https://timsong-cpp.github.io/cppwp/n4868/conv.prom)
        2. [Floating-point promotion](https://timsong-cpp.github.io/cppwp/n4868/conv.fpprom)
    3. Conversion
        1. [Integral conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.integral)
        2. [Floating-point conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.double)
        3. [Floating-integral conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.fpint)
        4. [Pointer conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.ptr)
        5. [Pointer-to-member conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.mem)
        6. [Boolean conversions](https://timsong-cpp.github.io/cppwp/n4868/conv.bool)

  	如果此 sequenced 有 Conversion Rank argument，那麼 sequenced rank 即為 Conversion Rank；如果沒有，但有 Promotion Rank argument，那麼 sequenced rank 為 Promotion Rank；否則為 sequenced rank 為 Exact Match Rank

    簡單來說，<span class = "yellow">以 argument 有的最低 Rank 當作 sequenced rank</span>。

  	如果 sequenced 的 rank 一樣，則依照 [ranking 規則](https://timsong-cpp.github.io/cppwp/n4868/over.ics.rank)(基本上是照上面順序下來)，依序從 sequenced 的第一個至最後一個 parameter 開始比較 rank，從頭到尾<span class = "yellow">都沒輸過</span>，且有贏最多次的為 function 為 best overload。

    舉個例子：		
	```cpp
    #include <iostream>

    void fn(int, double, int, int) { puts("A"); } // candidate A
    void fn(int, double, double, int) { puts("B"); } // candidate B

    int main()
    {
      fn(1, 2, 3, 4); // A
    }
    ```

    這裡 A 與 B 的 sequenced rank 都是 Conversion，平手，因此開始依序比叫參數的 rank：
	1. 平手
		A：1->int
		B：1->int
	2. 平手
		A：1->double
		B：1->double
	3. A 贏
		A：1->int
		B：1->double
	4. 平手
		A：1->int
		B：1->int

	A 從來沒有輸過，且 A 贏比較多次，所以 A 為 best overload。

	但我們換一下參數的順序，看另一個例子：
	```cpp
	#include <iostream>

	void fn(int, int, int, double) { puts("A"); } // candidate A
	void fn(int, double, double, int) { puts("B"); } // candidate B

	int main()
	{
	  fn(1, 2, 3, 4); // compile error: ambiguous
	}
	```

	這裡 A 與 B 的 sequenced rank 一樣都是 Conversion，平手，因此開始依序比叫參數的 rank：
	1. 平手
		A：1->int
		B：1->int
	2. A 贏
		A：1->int
		B：1->double
	3. A 贏
		A：1->int
		B：1->double
	4. <span class = "yellow">B 贏</span>
		A：1->double
		B：1->int

	A 有輸過一次，所以不能為 best overload，但 B 也輸過兩次，也不能為 best overload，所以這邊 Compiler 會給一個 error: call of overloaded is ambiguous，表示它找不到 best overload，哪怕 A 贏了比較多次

建議寫到一些比較特別的例子時要去翻一下 [ranking 規則](https://timsong-cpp.github.io/cppwp/n4868/over.ics.rank)，像是扯到 template 的時候：
```cpp
#include <iostream>

template <typename T>
void foo(T) { puts("A"); }

template <>
void foo<int *>(int *) { puts("B"); }    // specialisation of foo(T)

template <typename T>
void foo(T *) { puts("C"); }    // overload of foo(T)

int main()
{
  foo(new int);    // C;
}
```

這裡會去呼叫 `void foo(T *)` 而不是 `void foo<int *>(int *)`，因為標準訂了

> [12.4.4.1 - 2.4](https://timsong-cpp.github.io/cppwp/n4868/over.match.best#general-2.4): F1 is not a function template specialization and F2 is a function template specialization, or, if not that

意即 non-specialization 的版本優於 specialization 的版本，其實一開始寫起來還挺違反直覺的。

我們可以透過刻意加上 ambiguous function 來查看 candidate set，以上例為例：
```cpp
#include <iostream>

template <typename T>
void foo(T, int, double, double) { puts("A"); }

template <>
void foo<int *>(int *, int, double, double) { puts("B"); }    // specialisation of foo(T)

template <typename T>
void foo(T *, int, double, double) { puts("C"); }    // overload of foo(T)

template <typename T>
void foo(T *, double, int, int) { puts("D"); }    // overload of foo(T)

int main()
{
  foo(new int, 0, 0, 0);    // C;
}
```

如此一來編譯器便會給予錯誤訊息，並把 candidate set 給印出來：

![](https://i.imgur.com/0ENmOeD.png)

可以看見 candidate set 的確有這三個函式。

## SFINAE

而 SFINAE 的概念基本上就是對於模板的 overload 解析，在 overload resolution 第一個 name lookup 的階段，compiler 會把所有的 function template 包含進 overload set，這也包括 template specialization。但並不是所有的 function template instance 都是有意義的，有些 function 可能含有無法被 evaluated 的 expression，這時候就會需要 SFINAE。

SFINAE 發生的階段在 Overload 的第二個建立 candidate set 的階段，核心的概念是 template 參數在替換 explicitly specified 或 deduced type 的時候，如果失敗的並不會給出 error，而是不將其從 overload set 移除(不加入 candidate set)

而所謂的替換(Substitution)，簡單來說就是把函式模板裡面的 parameter，替換成 argument 的過程。

我們看個例子：

```cpp
struct X {
  typedef int type;
};

struct Y {
  typedef int type2;
};

template <typename T> void foo(typename T::type);    // Foo0
template <typename T> void foo(typename T::type2);   // Foo1
template <typename T> void foo(T);                   // Foo2

void callFoo() {
   foo<X>(5);    // Foo0: Succeed, Foo1: Failed,  Foo2: Failed
   foo<Y>(10);   // Foo0: Failed,  Foo1: Succeed, Foo2: Failed
   foo<int>(15); // Foo0: Failed,  Foo1: Failed,  Foo2: Succeed
}
```

當我們呼叫 `foo<X>(5)` 的時候，Foo0 成功被替換為 `void foo(int)`，加到 candidate set 內；Foo1 替換失敗，因為 `X` 裡面沒有 `type2`；Foo2 替換失敗，因為 `5` 無法被轉換為 `X`

同樣的，對於 `foo<Y>(10)`，Foo0 替換失敗，因為 `Y` 裡面沒有 `type`；Foo1 成功替換為 `void foo(int)`，加到 candidate set 內；Foo2 替換失敗，因為 `10` 無法被轉換為 `Y`

## std::enable_if

`std::enable_if` 是個 template struct，可能的定義如下(一種實作方式)：
```cpp
template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> { typedef T type; };
```

可以看見它是個 non-type template class，當第一個 template argument 為 `true` 時，`enable_if` 會有一個成員 `typedef` 叫 `type`，定義為 `T`，預設是 `void`；如果第一個 template argument 為 `false`，那就不會有這個成員。

也就是說 `std::enable_if<true, T>::type` 的結果為 `T`，`std::enable_if<false, T>::type` 則無法被 evaluated。

C\+\+11 時可以利用 `std::enable_if<CONDITION>::type` 拿到這個 `typedef` member。C\+\+14 時為了讓大家少打點自，多了一個 type alias 叫 `std::enable_if_t<CONDITION>` 可以直接拿到這個 member，也就是說它長得像這樣：

```cpp
template<bool B, typename T>
using std::enable_if_t<B, T> = std::enable_if<B, T>::type;
```

`std::enable_if` 可以幫助我們在 SFINAE 判斷的過程中，根據我們傳進的型別去 enable 或 disable 一個 function overloading，這邊 TJSW 舉了個很好的例子：
```cpp
template<typename T>
void func(T t) { puts("General"); } // (1)

template<>
void func<double>(double) { puts("Hello"); } // (2)

template<>
void func<int>(int) { puts("Hello"); } // (3)
```

這個例子中 `(1)` 是個通用的版本，而 `(2)` 是對 `double` 的特化， `(3)` 是對 `int` 的特化，依照我們呼叫 `func()` 給的參數會執行這三個 function 的其中一種，目的在於給予 `double` 或 `int` 當參數時我們想要印出 `Hello`，其他想印出 `General`

有了 `std::enable_if` 我們可以把 `(2)` 和 `(3)` 寫在一起：
```cpp
#include <iostream>

template <typename T>
constexpr bool is_int_or_double_v =
  std::is_same_v<T, int> || std::is_same_v<T, double>;

template <typename T>
void func(T t,
          std::enable_if_t<!is_int_or_double_v<T>> * = nullptr >) // A
{
  puts("General");
}

template <typename T>
void func(T t,
          std::enable_if_t<is_int_or_double_v<T>> * = nullptr >) // B
{
  puts("Hello");
}

int main()
{
  func('a');
  func(1.1);
  func(3);
}
```

第一段的 `is_int_or_double_v<T>` 是一個樣版變數，根據給定的型別是否是 `int` 或 `double`，他就是 `true` 或 `false`。

然後兩個 `func()` 其實就只是長相比較複雜的函式重載 (function overloading)，差別在 `is_int_or_double<T>` 前面有沒有 `!` 做 not 的反向邏輯。當我們在 `main` 寫下呼叫 `func('a')`，此時編譯器看到的兩個 `func()` 是：

```cpp
void func(char t, void* = nullptr); // A
void func(char t, /* ill-formed expression */); // B
```

根據 SFINAE，第一個 `func` 會被加入 candidate set，第二個由於 expression 無法被 evaluated(struct 內部沒有 type 成員)，所以不會被加入 candidate set。因此編譯器會去呼叫 A，印出 `General`。

在這裡的例子我們使用的是 `std::is_same_v` 來幫助我們製作 `std::enable_if` 的參數，若想要處理更複雜的狀況，往往需要 `decltype` 與 `std::declval` 來幫助我們

## decltype 與 std::declval

`decltype` 可以幫助我們獲得某個 entity 的 type，或是某個 expression 的 type 與 value category，用法大致上如下：
```cpp
#include <type_traits>
#include <iostream>

struct S {};

int main()
{
  int i = 0;
  int &ri = i;
  int &&rri = 0;

  S s;
  S &rs = s;
  S &&rrs = {};

  std::cout << std::boolalpha;
  std::cout << std::is_same_v<int, decltype(i)> << ' ' << std::is_same_v<int &, decltype((i))> << '\n'    // true true
            << std::is_same_v<int &, decltype(ri)> << ' ' << std::is_same_v<int &, decltype((ri))> << '\n'    // true true
            << std::is_same_v<int &&, decltype(rri)> << ' ' << std::is_same_v<int &, decltype((rri))> << '\n'    // true true
            << std::is_same_v<S, decltype(s)> << ' ' << std::is_same_v<S &, decltype((s))> << '\n'    // true true
            << std::is_same_v<S &, decltype(rs)> << ' ' << std::is_same_v<S &, decltype((rs))> << '\n'    // true true
            << std::is_same_v<S &&, decltype(rrs)> << ' ' << std::is_same_v<S &, decltype((rrs))> << '\n';    // true true
}
```

`decltype(i)` 會回傳 `i` 的 type，`decltype((i))` 會對 type 加上一個 lvalue reference，若原型態為 reference type 則會進行 reference-collapsing，其[規則](https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers)如下：

+ An rvalue reference to an rvalue reference becomes (“collapses into”) an rvalue reference.
+ All other references to references (i.e., all combinations involving an lvalue reference) collapse into an lvalue reference.

而這邊加的是 lvalue reference，因此基本上回傳的只會有 lvalue reference。

這可以幫助我們獲得一個 class 的 member type：
```cpp
#include <type_traits>
#include <iostream>

struct S {
  int i;
  int fn1();
};

int main()
{
  std::cout << std::boolalpha;
  std::cout << std::is_same_v<int, decltype(S().i)> << '\n'    // true
            << std::is_same_v<int, decltype(S().fn1())> << '\n';    // true
}
```

`decltype` 並不會真的去執行 `()` 內的 expression，因此 `S` 的建構子是不會被執行的，也不會有臨時對象出來。

但如果一個 class 沒有預設的建構子，那麼 `decltype` 就沒有辦法用：
```cpp
#include <type_traits>
#include <iostream>

struct S {
  S() = delete;
  int i;
  int fn1();
};

int main()
{
  std::cout << std::boolalpha;
  std::cout << std::is_same_v<int, decltype(S().i)> << '\n'    // error
            << std::is_same_v<int, decltype(S().fn1())> << '\n';    // error
}
```

這時就需要使用 `std::declval<T>()`，它可以允許在沒有預設建構子的情況下，不需用傳參數就回傳一個 `T` 的 rvalue reference `T&&`，換句話說它就是個偽造實例的工具，套用到上面的例子：
```cpp
#include <type_traits>
#include <iostream>

struct S {
  S() = delete;
  int i;
  int fn1();
};

int main()
{
  std::cout << std::boolalpha;
  std::cout << std::is_same_v<int, decltype(std::declval<S>().i)> << '\n'    // true
            << std::is_same_v<int, decltype(std::declval<S>().fn1())> << '\n';    // true
}
```

## std::false_type 與 std::true_type

這兩個東西基本上就是 `false` 和 `true`，定義大概長這樣:
```cpp
template <bool _Val>
using bool_constant = integral_constant<bool, _Val>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;

// STRUCT TEMPLATE integral_constant
template <class _Ty, _Ty _Val>
struct integral_constant {
    static constexpr _Ty value = _Val;

    using value_type = _Ty;
    using type       = integral_constant;
    constexpr operator value_type() const noexcept {
        return value;
    }
    _NODISCARD constexpr value_type operator()() const noexcept {
        return value;
    }
};
```

你可以簡單理解為這樣：
```cpp
struct false_type {
    static constexpr bool value = false;
    constexpr operator bool() const noexcept { return value; }
    // There is more here, but it doesn't really matter for your question
};

struct true_type {
    static constexpr bool value = true;
    constexpr operator bool() const noexcept { return value; }
    // There is more here, but it doesn't really matter for your question
};
```

因為它是個 struct，因此可以被繼承，這讓我們的 class 可以擁有個 true 與 false 來當作符不符合條件的判斷條件，舉個例子，學到這裡我們基本上可以組出一個簡單的判斷了，通常我們會用一個 class 來封裝這個判斷，這個 class 被稱為 traits class

假設我們想判斷一個型態是不是 `void`，可以用最簡單的特化來做：
```cpp
#include <iostream>

// primirary class
template <typename T>
struct Is_void {
  static constexpr bool value = false;
};

template <>
struct Is_void<void> {
  static constexpr bool value = true;
};

int main()
{
  std::cout << std::boolalpha;
  std::cout << Is_void<int>::value << '\n'    // false
            << Is_void<float>::value << '\n'    // false
            << Is_void<void>::value << '\n';    // true

  return 0;
}
```

我們可以把 `std::false_type` 和 `std::true_type` 加進來：
```cpp
#include <iostream>
#include <type_traits>

// primirary class
template <typename T>
struct Is_void : std::false_type {};

template <>
struct Is_void<void> : std::true_type {};

int main()
{
  std::cout << std::boolalpha;
  std::cout << Is_void<int>::value << '\n'    // false
            << Is_void<float>::value << '\n'    // false
            << Is_void<void>::value << '\n';    // true

  return 0;
}
```

# 簡單的例子

## 例 1 檢查型態是否為 double 或 int

讀到這裡我們已經可以寫一些例子了，像是我們現在想要檢查一個型態是不是 `doble` 或 `float`，那我們可以簡單的利用 constexpr bool 來做：
```cpp
#include <iostream>
#include <type_traits>

template <typename T>
constexpr bool is_double_or_float = std::is_same_v<T, double> || std::is_same_v<T, float>;

int main()
{
  std::cout << std::boolalpha;
  std::cout << is_double_or_float<int> << '\n'
            << is_double_or_float<double> << '\n'
            << is_double_or_float<long> << '\n'
            << is_double_or_float<char> << '\n'
            << is_double_or_float<bool> << '\n'
            << is_double_or_float<float>;
}
```

非常簡單，不需要用到太多東西，但隨著東西的複雜度上升，需要的東西就越來越多了，讓我們看看下個例子。

## 例 2：ToString 與 `std::to_string`

TJSW 舉了一個很棒的[例子](https://tjsw.medium.com/%E6%BD%AE-c-detection-idiom-%E5%81%B5%E6%B8%AC%E8%AA%9E%E7%BE%A9-%E7%89%A9%E4%BB%B6%E8%83%BD%E4%B8%8D%E8%83%BD%E8%BD%89%E5%9E%8B%E6%88%90%E5%AD%97%E4%B8%B2-expression-sfinae-1f1d05e72cc7)，我這邊引用一下並補充一些東西，假設今天我們想針對擁有 `ToString()` 成員函式的實例自動呼叫 `ToString()`；其他型別則是呼叫 `std::to_string()`，這樣根據編譯期傳入型別的不同，去特化不同 function template，這便是 SFINAE 可以進來幫助我們的地方。

我們用一個 `ToString` 來封裝，而邏輯大概是這樣的：

+ 當傳入 ToString(t) 且呼叫 `t.ToString()` 合法，那便利用這個型別去特化一個 function template，在裡面呼叫 `t.ToString()`
+ 否則檢查 `std::to_string(t)` 是否合法，合法的話便呼叫 `std::to_string(t)`

重點是要怎麼知道 `t.ToString()` 合法呢? 方法也很簡單，嘗試利用 `std::declval` 去呼叫就可以了：

```cpp
// example from: https://tjsw.medium.com/%E6%BD%AE-c-detection-idiom-%E5%81%B5%E6%B8%AC%E8%AA%9E%E7%BE%A9-%E7%89%A9%E4%BB%B6%E8%83%BD%E4%B8%8D%E8%83%BD%E8%BD%89%E5%9E%8B%E6%88%90%E5%AD%97%E4%B8%B2-expression-sfinae-1f1d05e72cc7
#include <iostream>
#include <type_traits>
#include <string>

template <typename T>
auto ToString(T t) -> decltype(std::to_string(t))
{
  return std::to_string(t);
}

template <typename T>
auto ToString(T t) -> decltype(std::declval<T>().ToString())
{
  return t.ToString();
}

struct S {
  S(std::string a) : a(std::move(a)) {}
  std::string ToString() const { return a + "!!!!"; }
  std::string a;
};

int main()
{
  int a = 10;
  double d = 1.67;
  S s{ "Bambi" };
  std::cout
    << ToString(a) << '\n'    // 10
    << ToString(d) << '\n'    // 1.67000
    << ToString(s) << '\n';    // Bambi!!!!
}
```

若我們還想要檢查回傳的型態是 `std::string` 的話，可以把 `std::enable_if` 加進來：
```cpp
// example from: https://tjsw.medium.com/%E6%BD%AE-c-detection-idiom-%E5%81%B5%E6%B8%AC%E8%AA%9E%E7%BE%A9-%E7%89%A9%E4%BB%B6%E8%83%BD%E4%B8%8D%E8%83%BD%E8%BD%89%E5%9E%8B%E6%88%90%E5%AD%97%E4%B8%B2-expression-sfinae-1f1d05e72cc7
#include <iostream>
#include <type_traits>
#include <string>

template <typename T>
auto ToString(T t) -> decltype(std::to_string(t))    // std::to_string 一定可以轉，就不檢查了
{
  return std::to_string(t);
}

template <typename T>
std::enable_if_t<std::is_convertible_v<decltype(std::declval<T>().ToString()), std::string>,
                 std::string>
ToString(T t)
{
  return t.ToString();
}

struct S {
  S(std::string a) : a(std::move(a)) {}
  std::string ToString() { return a + "!!!!"; }
  std::string a;
};

int main()
{
  int a = 10;
  double d = 1.67;
  S s{ "Bambi" };
  std::cout
    << ToString(a) << '\n'    // 10
    << ToString(d) << '\n'    // 1.67000
    << ToString(s) << '\n';    // Bambi!!!!
}
```

如此一來，當 `ToString` 的回傳型別可以轉換為 `std::string` 時，`std::is_convertible_v` 的結果會為 `true`，因此 `std::enable_if_t` 的 CONDITION 為 `true`，所以 type 會是我們設定的 `std::string`。

我們可以利用 `std::enable_if` 的特性，寫個 traits class，對它進行封裝：
```cpp
// example from: https://tjsw.medium.com/%E6%BD%AE-c-detection-idiom-%E5%81%B5%E6%B8%AC%E8%AA%9E%E7%BE%A9-%E7%89%A9%E4%BB%B6%E8%83%BD%E4%B8%8D%E8%83%BD%E8%BD%89%E5%9E%8B%E6%88%90%E5%AD%97%E4%B8%B2-expression-sfinae-1f1d05e72cc7
#include <iostream>
#include <type_traits>
#include <string>

// primary
template <typename, typename = void>
struct has_ToString : std::false_type {};

// template partial specialization
template <typename T>
struct has_ToString<T, decltype(T::ToString, void())>
    : std::is_convertible<decltype(std::declval<T>().ToString()),
                          std::string> {
};

template <typename T>
constexpr bool has_ToString_v = has_ToString<T>::value;

template <typename T,
          typename std::enable_if_t<has_ToString_v<T>> * = nullptr>
std::string ToString(T t)
{
  return t.ToString();
}

template <typename T,
          typename std::enable_if_t<!has_ToString_v<T>> * = nullptr>
std::string ToString(T t)
{
  return std::to_string(t);
}

struct S {
  S() = default;
  S(std::string a) : a(std::move(a)) {}
  std::string ToString() { return a + "!!!!"; }
  std::string a;
};

int main()
{
  int a = 10;
  double d = 1.67;
  S s{ "Bambi" };
  std::cout
    << ToString(a) << '\n'    // 10
    << ToString(d) << '\n'    // 1.67000
    << ToString(s) << '\n';    // Bambi!!!!
}
```

這邊我們一樣利用了模板偏特化的技巧來包裝這個 traits class，首先我們有個基本模板類(primary template class) 繼承 `std::false_type`，之後有個偏特化類繼承 `std::is_convertible<decltype(std::declval<T>().ToString()), std::string>`，當 SFINAE 在看這段時，若 `T` 沒有 member `ToString()`，推斷便會失敗，因此不會實例化此特化，而是實例化基本類，也因此 `has_ToString::value` 為 `false`；若有 `ToString()`，但回傳型態不能轉換為 `std::string`，那雖然會實例化此特化類，但 `has_ToString::value` 仍為 `false`。

這裡的 `decltype(T::ToString, void())` 是為了滿足偏特化條件，因為我們基本模板類的模板參數是 `<typename, typename = void>`，為了進行偏特化，我們的第二個模板參數也必須為 `void`，因此 `decltype` 內加上了 `void()`。

在 C\+\+17 後，我們可以使用 `std::void_t` 來幫助我們做這件事，它是一個模板別名，定義為：
```cpp
template <class...>
using void_t = void;
```

換句話說，`std::void_t<任何東西>` 都是 `void`，因此我們上面的 `decltype(T::ToString, void())` 可以寫成 `std::void_t<decltype(T::ToString)>`。

回到原本的例子，看完了 traits class，我們來看 function，兩個 function 都有個 `std::enable_if_t<>`，差別在於 `has_ToString_v<T>` 前面有沒有 `!` 做 not 的反向邏輯，`has_ToString_v<T>` 是個 `constexpr bool`，儲存 `has_ToString::value` 的值。

跟前面介紹 `std::enable_if` 的用法一樣，若 CONDITION 為 `false`，則 `std::enable_if_t<COND> * = nullptr` 會被推斷為 `/*ill-formed*/ * = nullptr`，由於無法被 evaluated，因此 SFINAE 不會選擇實例化這個 function。

如此一來我們就做到了當 `has_ToString_v` 為 `true` 時，使用類別物件的 `ToString`，其他狀況使用 `std::to_string`。

核心概念就是 <span class = "yellow">`void_t` 檢查 member 是否存在，`std::is_same` 檢查 type 是否一樣</span>。

## 例 3：ML Model

最後回到最一開始講 `concept` 的例子，讓大家多看幾個例子熟悉一下，我們有一個函式 `check`，它的 template parameter 是一個 ML Model，一個有效的 ML Model 需要有 `fit` 和 `predict` 這兩個 function，同時需要有個 confusion table，那在 C\+\+20 前我們可以這樣寫：
```cpp
#include <iostream>
#include <type_traits>

template <typename, typename = void>
struct has_fit : std::false_type {};

template <typename Model>
struct has_fit<Model, std::void_t<decltype(Model::fit)>>
    : std::is_same<void,
                   decltype(std::declval<Model>().fit())> {
};

template <typename, typename = void>
struct has_predict : std::false_type {};

template <typename Model>
struct has_predict<Model, std::void_t<decltype(Model::predict)>>
    : std::is_same<void,
                   decltype(std::declval<Model>().predict())> {
};

template <typename, typename = void>
struct has_confusion : std::false_type {};

template <typename Model>
struct has_confusion<Model, std::void_t<decltype(Model::FN), decltype(Model::FP), decltype(Model::TN), decltype(Model::TP)>>
    : std::conjunction<std::is_same<int, decltype(Model::FN)>,
                       std::is_same<int, decltype(Model::FP)>,
                       std::is_same<int, decltype(Model::TN)>,
                       std::is_same<int, decltype(Model::TP)>> {
};

template <typename Model>
struct valid_Model 
  : std::conjunction<has_fit<Model>, has_predict<Model>, has_confusion<Model>> {};

template <typename Model>
constexpr bool valid_Model_v = valid_Model<Model>::value;

struct M1 {
  int TN, TP, FN, FP;
  void fit();
  void predict();
};

struct M2 {};

struct M3 {
  int TN, TP, FN, FP;
  [[noreturn]] int fit();
  [[noreturn]] int predict();
};

template <typename T, std::enable_if_t<!valid_Model_v<T>> * = nullptr>
void check(T a)
{
  puts("no~~");
}

template <typename T, std::enable_if_t<valid_Model_v<T>> * = nullptr>
void check(T a)
{
  puts("ok!");
}

int main()
{
  M1 m1;
  M2 m2;
  M3 m3;

  check(m1);    // ok
  check(m2);    // no
  check(m3);    // no
}
```

## 例 4: 檢查是否含有指定的 member function

這是一個 stackoverflow 上的[例子](https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature)，寫得很漂亮，所以這邊也拿過來講解一下：
```cpp
#include <iostream>
#include <string>

#include <type_traits>

// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.

template <typename, typename T>
struct has_serialize {
  static_assert(
    std::integral_constant<T, false>::value,
    "Second template parameter needs to be of function type.");
};

// specialization that does the checking

template <typename C, typename Ret, typename... Args>
struct has_serialize<C, Ret(Args...)> {
private:
  template <typename T>
  static constexpr auto check(T *)
    -> typename std::is_same<
      decltype(std::declval<T>().serialize(std::declval<Args>()...)),
      Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      >::type;    // attempt to call it and see if the return type is correct

  template <typename>
  static constexpr std::false_type check(...);

  typedef decltype(check<C>(0)) type;

public:
  static constexpr bool value = type::value;
};

struct X {
  int serialize(const std::string &, int, double) { return 42; }
};

struct Y : X {};

int main()
{
  std::cout << has_serialize<Y, int(const std::string &, int, double)>::value;    // will print 1
}
```

這個例子只使用了 `std::declval` 來實作，且結合了 variadic template，讓我們可以使用不定參數來檢查。

這個例子會有幾種情況：

1. template argument 數量不對
  那麼會進到上面的 primary template，觸發 assertion 或變成我們不 care 的狀況。
2. type 沒有 `serialize` 這個 method
  此時 `std::declval<T>().serialize(std::declval<Args>()...)` 嘗試呼叫失敗，SFINAE 並不會進行實例化，因此會進到 traits class 內的 `static constexpr std::false_type check(...)`。
3. type 有 `serialize`，但參數與檢查的不符
  同上。
4. type 有 `serialize`，但回傳型態不對
  雖然可以呼叫，所以實例化的是上面的 `check(T *)`，然而回傳型態不對，因此 `std::is_same` 的 type 為 `false`。
5. type 有 `serialize`，且回傳型態與參數全部符合
  可以呼叫，因此實例化上面的 `check(T *)`，且回傳型態是對的，因此 `std::is_same` 的 type 為 `true`。

從這邊你可以發現其實 Detection Idiom 的寫法很多，像這邊就只用到了 `std::declval`，且彈性很高且寫起來也可以很乾淨。

## constexpr if

前面這種「符合某個條件的時候呼叫 A，不符合的時候呼叫 B」的事情我們是在 function template parameter 加上 `std::enable_if`，利用 SFINAE 來完成的，在 C\+\+17 後我們可以使用 
`constexpr if` 來做這件事情，使用方法很簡單，直接把原本塞在 `std::enable_if` 裡面的 CONDITION 放到 `constexpr if` 的圓括號裡面就好，寫起來像這樣：
```cpp
// example from: https://tjsw.medium.com/%E6%BD%AE-c-detection-idiom-%E5%81%B5%E6%B8%AC%E8%AA%9E%E7%BE%A9-%E7%89%A9%E4%BB%B6%E8%83%BD%E4%B8%8D%E8%83%BD%E8%BD%89%E5%9E%8B%E6%88%90%E5%AD%97%E4%B8%B2-expression-sfinae-1f1d05e72cc7
#include <iostream>
#include <type_traits>
#include <string>

template <typename, typename = void>
struct has_ToString : std::false_type {
};

template <typename T>
struct has_ToString<T, std::void_t<decltype(T::ToString)>>
    : std::is_convertible<decltype(std::declval<T>().ToString()),
                          std::string> {
};

template <typename T>
constexpr bool has_ToString_v = has_ToString<T>::value;

template <typename T>
std::string ToString(T t)
{
  if constexpr (has_ToString_v<T>)
    return t.ToString();
  else
    return std::to_string(t);
}

struct S {
  S() = default;
  S(std::string a) : a(std::move(a)) {}
  std::string ToString() { return a + "!!!!"; }
  std::string a;
};

int main()
{
  int a = 10;
  double d = 1.67;
  S s{ "Bambi" };
  std::cout
    << ToString(a) << '\n'    // 10
    << ToString(d) << '\n'    // 1.67000
    << ToString(s) << '\n';    // Bambi!!!!
}
```

可以發現 `ToString` 變得簡潔許多，可以避免我們寫很多 `std::enable_if_t` 去做樣板特化 (雖然我還是比較習慣用 `std::enable_if` 啦)

# 自己做一個簡單的 concept

## 實作例子

看了這麼多例子，大家應該比較熟悉這些工具的運用了，現在我們可以試著做一個自己的 concept 看看，這邊我寫了一個例子，先看看沒有包裝的版本：
```cpp
#include <iostream>
#include <type_traits>

/**
 * @brief The checker, which will check if the concept structure has a function called REQUIRES,
 *        then tried to called it for checking if the Type satisfy the requirement by SFINAE.
 * @tparam CONCEPT The Concept structure
 * @tparam SUBJECT The class will be checked if it satisfied the requirement.
 */
template <typename CONCEPT, typename SUBJECT>
struct RequiresChecker {
private:
  // if the the return type of REQUIRES is not valid, SFINAE goes here.
  template <typename T, typename = void>
  struct Checker : std::false_type {};

  // otherwise it will be here, since return type is valid.
  template <typename T>
  struct Checker<T, std::void_t<decltype(std::declval<T>().template REQUIRES<SUBJECT>(std::declval<SUBJECT>()))>>    // Try to call the REQUIRE function, this would check if the return type of the fuction is valid
      : std::true_type {};

  using type = Checker<CONCEPT>;

public:
  static constexpr bool value = type::value;    // true if the requirement of concept class passed, otherwise false.
};

template <typename T, typename TARGET>
constexpr std::enable_if_t<std::is_same_v<T, TARGET>, bool> same_return = true;    // if the type is not same, the type `valid_ret` is non valid.

template <class CONCEPT, class... SUBJECT>
constexpr bool satisfied = std::conjunction_v<RequiresChecker<CONCEPT, std::remove_reference_t<SUBJECT>>...>;

struct valid_model {
  template <class T>
  auto REQUIRES(T o) 
    -> std::void_t<decltype(same_return<decltype(o.fit()), void>, 
                            same_return<decltype(o.predict()), void>, 
                            same_return<decltype(o.TN), int>)>;    // use decltype to check if the enable_if is valid,
                                                                   // if the member is not exist or the type is not same,
                                                                   // the `same_return` would not exist, which cause an Substitution failed in Checker
};

struct normal_model {
  template <class T>
  auto REQUIRES(T o) 
    -> std::void_t<decltype(o.fit(), 
                            o.predict(), 
                            o.TN)>;    // use decltype to check if the enable_if is valid,
                                       // if the member is not exist, it will cause an Substitution failed in Checker.
};

struct M1 {
  int TN, TP, FN, FP;
  void fit();
  void predict();
};

struct M2 {};

struct M3 {
  int TN, TP, FN, FP;
  [[noreturn]] int fit();
  [[noreturn]] int predict();
};

template <typename... T, typename std::enable_if_t<satisfied<valid_model, T...>> * = nullptr>
bool check_valid(T... t) { return true; }

template <typename... T, typename std::enable_if_t<!satisfied<valid_model, T...>> * = nullptr>
bool check_valid(T... t) { return false; }

template <typename... T, typename std::enable_if_t<satisfied<normal_model, T...>> * = nullptr>
bool check_normal(T... t) { return true; }

template <typename... T, typename std::enable_if_t<!satisfied<normal_model, T...>> * = nullptr>
bool check_normal(T... t) { return false; }

int main()
{
  M1 m1;
  M2 m2;
  M3 m3;
  std::cout << std::boolalpha;
  std::cout << check_valid(m1) << ' ' << check_valid(m2) << ' ' << check_valid(m3) << '\n';    // true false false
  std::cout << check_normal(m1) << ' ' << check_normal(m2) << ' ' << check_normal(m3) << '\n';    // true false true
  std::cout << check_normal(m1, m2) << ' ' << check_normal(m1, m3) << ' ' << check_normal(m3, m3);    // false true true
}
```

## Concept 部分

我們分幾個部分來看，首先是 concept 的部分，我用 struct 來實作：
```cpp
struct normal_model {
  template <class T>
  auto REQUIRES(T o) 
    -> std::void_t<decltype(o.fit(), 
                            o.predict(), 
                            o.TN)>;    // use decltype to check if the enable_if is valid,
                                       // if the member is not exist, it will cause an Substitution failed in Checker.
};
```

這個 concept 叫做 `normal_model`，裡面有個 `REQUIRES` 模擬 concept 中的 `requires` 關鍵字，拿來放我們對 template parameter 的限制，這裡我是用function template declaration 來實作，回傳型態為 `std::void_t<decltype(o.fit(), o.predict(), o.TN)>`，用意是在 `o` 沒有任一成員時造成替換失敗。

你可能會想說奇怪，這裡只有一個 function template，那一旦替換失敗，candidate set 為空，那不就會造成 compile error 了嗎?

但我們這邊希望做到的事是製作另一個統一的 interface 去嘗試使用 `REQUIRES`，這個 interface 會被一個 `Checker` 的 class 包裝起來，所以邏輯是在 Checker 內嘗試去檢查：
1. 如果 `REQUIRES` 的回傳型態為 valid，那麼 `Checker::value` 為 `true`
2. 否則為 `false`

這就跟我們前面的例子一樣了

這是另一個 concept 的例子：
```cpp
struct valid_model {
  template <class T>
  auto REQUIRES(T o) 
    -> std::void_t<decltype(same_return<decltype(o.fit()), void>, 
                            same_return<decltype(o.predict()), void>, 
                            same_return<decltype(o.TN), int>)>;    // use decltype to check if the enable_if is valid,
                                                                   // if the member is not exist or the type is not same,
                                                                   // the `same_return` would not exist, which cause an Substitution failed in Checker
};
```

用意一樣，如果我們傳進 `same_return` 的兩個 template argument 型態不一樣，那就要導致 `Checker` 的替換失敗，至於如何達成則要看看 `same_return` 的定義：
```cpp
template <typename T, typename TARGET>
constexpr std::enable_if_t<std::is_same_v<T, TARGET>, bool> same_return = true;    // if the type is not same, the type `valid_ret` is non valid.
```

可以看見 `same_return` 的型態用到了 `std::enable_if_t`，因此當 `std::is_same_v<T, TARGET>` 為 `false` 時 `same_return` 的 return type 為 non-valid 的，而這將會觸發替換失敗。

## Checker

Checker 用來幫助我們使用 SFINAE，也就是我們前面的例子所用到的技巧，先看看這裡的實作例子：
```cpp
template <typename CONCEPT, typename SUBJECT>
struct RequiresChecker {
private:
  // if the the return type of REQUIRES is not valid, SFINAE goes here.
  template <typename T, typename = void>
  struct Checker : std::false_type {};

  // otherwise it will be here, since return type is valid.
  template <typename T>
  struct Checker<T, std::void_t<decltype(std::declval<T>().template REQUIRES<SUBJECT>(std::declval<SUBJECT>()))>>    // Try to call the REQUIRE function, this would check if the return type of the fuction is valid
      : std::true_type {};

  using type = Checker<CONCEPT>;

public:
  static constexpr bool value = type::value;    // true if the requirement of concept class passed, otherwise false.
};
```

我的 Checker 取名為 `RequiresChecker`，有兩個 template parameter，第一個為 `CONCEPT`，也就是我們的 concept structure，第二個為 `SUBJECT`，也就是要檢查是否符合 concept requirement 的型態，因此假設我們要檢查 `T` 是不是一個 `valid_model`，用起來會長這樣：
`RequiresChecker<valid_model, T>::value`

`RequiresChecker` 裡面有個 traits class 叫 `Checker`，特化的版本使用了
`decltype(std::declval<T>().template REQUIRES<SUBJECT>(std::declval<SUBJECT>()))` 來檢查 `CONCEPT` 裡面的 `REQUIRES` 是否 valid，若 valid，代表 `REQUIRES` 的要求都有被滿足，則 `RequiresChecker::value` 為 `true`，否則為 `false`。

## 如何使用

我為了方便，同時為了支援 variadic template型別，最後使用了一個 `satisfied` 來存 `RequiresChecker::value`，定義如下：

```cpp
template <class CONCEPT, class... SUBJECT>
constexpr bool satisfied = std::conjunction_v<RequiresChecker<CONCEPT, std::remove_reference_t<SUBJECT>>...>;
```

可以看見就只是用 `std::conjubction_v` 把每個 `value` 串起來而已，`std::remove_reference_t` 的目的是為了應付 function parameter 為 universal reference 的狀況，因為 references type 並沒有 function members，舉個例子：

```cpp
#include <type_traits>

template<typename, typename = void>
struct has_mem : std::false_type {};

template <typename T>
struct has_mem<T, std::void_t<decltype(T::mem)>> : std::true_type {};

struct S {
  int mem;
};

template<typename T>
void check(T&&) {
  static_assert(has_mem<T>::value, "type didn't contain 'mem'");
}

int main() {
  S s;
  check(s); // error, type didn't contain 'mem'
}
```

這邊會有 error 是因為根據 reference-collapsing，`T& &&` 為 `T&&`，因此 `T` 被推斷為 `T&`，導致在檢查的時候是檢查 `T&::mem`，但我們實際想要檢查的是 `T::mem`，為了避免這種狀況，我加上了 `remove_reference`。

另外這個實作沒有處理 `const` 的版本，因此對於 `const T` 實際上只做了 `T` 的檢查，若有些 member 應該要是 `const` version 的，這個實作檢查不到。

function template 要對 template argument 進行限制也很簡單，我這邊是用 `std::enable_if` 來做，大家也可以使用 constexpr if 試試看：
```cpp
template <typename... T, typename std::enable_if_t<satisfied<valid_model, T...>> * = nullptr>
bool check_valid(T... t) { return true; }

template <typename... T, typename std::enable_if_t<!satisfied<valid_model, T...>> * = nullptr>
bool check_valid(T... t) { return false; }
```
## 進行包裝

我們可以對它做包裝，讓其使用起來方便一點：
```cpp
#include <iostream>
#include <type_traits>

#define REQUIRES_BLOCK(...) \
  ->std::void_t<decltype(__VA_ARGS__)>;

#define REQUIRES_RET(EXPR, TARGET) \
  detail::same_return<decltype(EXPR), TARGET>

#define CONCEPT_REQUIRES(CONCEPT_CLASS, ...) \
  std::enable_if_t<detail::satisfied<CONCEPT_CLASS, __VA_ARGS__>> * = nullptr

#define CONCEPT_REQUIRES_NOT(CONCEPT_CLASS, ...) \
  std::enable_if_t<!detail::satisfied<CONCEPT_CLASS, __VA_ARGS__>> * = nullptr

namespace detail {
  /**
   * @brief Check if the concept structure has a function called REQUIRES,
   *        Then tried to called it for checking if the Type satisfy the requirement by SFINAE.
   * @tparam CONCEPT__ The Concept structure
   * @tparam SUBJECT__ The type would be checked if satisfied the requirement.
   */
  template <typename CONCEPT__, typename SUBJECT__>
  struct RequiresChecker {
  private:
    // otherwise the the return type of REQUIRES is not valid, SFINAE goes here.
    template <typename T, typename = void>
    struct Checker__ : std::false_type {};

    // if the type satisfied the requirement, it will be here, since return type is valid.
    template <typename T>
    struct Checker__<T, std::void_t<decltype(std::declval<T>().template REQUIRES<SUBJECT__>(std::declval<SUBJECT__>()))>>
        : std::true_type {};

    using type = Checker__<CONCEPT__>;

  public:
    static constexpr bool value = type::value;
  };

  template <typename T, typename TARGET>
  using valid_ret_type = std::enable_if_t<std::is_same_v<T, TARGET>, bool>;    // if the type is same, it will be bool

  template <typename T, typename TARGET>
  constexpr valid_ret_type<T, TARGET> same_return = true;    // if the type is not same, the type of same_return is non valid.

  template <class CONCEPT__, class... SUBJECT__>
  constexpr bool satisfied = std::conjunction_v<RequiresChecker<CONCEPT__, std::remove_reference_t<SUBJECT__>>...>;
}    // namespace detail

struct valid_model {
  template <typename T>
  auto REQUIRES(T o) REQUIRES_BLOCK(
    REQUIRES_RET(o.fit(), void),
    REQUIRES_RET(o.predict(), void),
    REQUIRES_RET(o.TN, int)
  )
};

struct normal_model {
  template <typename T>
  auto REQUIRES(T o) REQUIRES_BLOCK(
    o.fit(),
    o.predict(),
    o.TN
  )
};

struct M1 {
  int TN, TP, FN, FP;
  void fit();
  void predict();
};

struct M2 {};

struct M3 {
  int TN, TP, FN, FP;
  [[noreturn]] int fit();
  [[noreturn]] int predict();
};

template <typename... T, CONCEPT_REQUIRES(valid_model, T...)>
constexpr bool check_valid(T...) { return true; }

template <typename... T, CONCEPT_REQUIRES_NOT(valid_model, T...)>
constexpr bool check_valid(T...) { return false; }

template <typename... T, CONCEPT_REQUIRES(normal_model, T...)>
constexpr bool check_normal(T...) { return true; }

template <typename... T, CONCEPT_REQUIRES_NOT(normal_model, T...)>
constexpr bool check_normal(T...) { return false; }

int main()
{
  M1 m1;
  M2 m2;
  M3 m3;
  std::cout << std::boolalpha;
  std::cout << check_valid(m1) << ' ' << check_valid(m2) << ' ' << check_valid(m3) << '\n';    // true false false
  std::cout << check_normal(m1) << ' ' << check_normal(m2) << ' ' << check_normal(m3) << '\n';    // true false true
  std::cout << check_normal(m1, m2) << ' ' << check_normal(m1, m3) << ' ' << check_normal(m3, m3);    // false true true
}
```

用起來就很像 C\+\+20 的 concept 了

# std::is_detected

`std::is_detected` 實際上應該寫為 `std::experimental::is_detected`，是個還沒正式納入 STL 的東西，它的用法長這樣：

```cpp
template <typename T>
using has_mem_t = decltype(std::declval<T>().mem);

struct S1 {};
struct S2 { int mem; };

int main() {
  static_assert(std::is_detected<has_mem_t, S1>::value); // failed
  static_assert(std::is_detected<has_mem_t, S2>::value); // ok!
}
```

可以看見它能幫忙我們偵測某個 type 裡面有沒有對應的成員，相信看到這裡，大家心裡都大概有個底該怎麼做出來了，實際上跟我們前面做的事情差不多，[cppreference](https://en.cppreference.com/w/cpp/experimental/is_detected) 上面給的實作長這樣：
```cpp
struct nonesuch {
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

namespace detail {
template <class Default, class AlwaysVoid,
          template<class...> class Op, class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <class Default, 
          template<class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

} // namespace detail

template <template<class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template<class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;
```

`template <template<class...> class Op, class... Args>` 代表 template parameter 是一個 template class Op，如果看不懂的同學可以再去複習一下 template。

當我們寫 `using has_mem_t = decltype(std::declval<T>().mem);` 並把它代入為 `std::is_detected<has_mem_t, S1>` 時，`is_detected` 會推斷為
`dector<nonesuch, void, has_mem_t, S1>`，若 `has_mem_t` 為 valid 的 type，則 `value_t` 為 `std::true_type`，`Op<Args...>` 為 `has_mem_t<S1>`。

可以看見它實際上就只是另一種包裝的方法，內部就是個 traits class，只是做了一點調整變得更 general 了。

# 古典 C\+\+ 的做法(C\+\+11 以前)

如果你有嘗試找過 Detected Idiom 相關的東西，像是 google 搜尋 「C\+\+ check if template class has member」之類的，你可能會發現還有另一種神奇的作法，用了兩個 char array，並以 array 大小判斷是否有指定 member，這其實是 C\+\+11 前還沒有 `std::enable_if` 的精典做法，我們通常稱之為 Member Detector

我這邊從 [stack overflow](https://stackoverflow.com/questions/1005476/how-to-detect-whether-there-is-a-specific-member-variable-in-class) 上找到了一個還不錯的例子：
```cpp
template<typename T> 
struct HasX { 
  struct Fallback { int x; }; // introduce member name "x"
  struct Derived : T, Fallback { };

  template<typename C, C>
  struct ChT; 

  template<typename C>
  static char (&f(ChT<int Fallback::*, &C::x>*))[1]; 

  template<typename C>
  static char (&f(...))[2]; 

  static bool const value = sizeof(f<Derived>(0)) == 2;
};
```

使用方法也很簡單，跟之前一模一樣：
```cpp
struct A { int x; };
struct B { int I; };

int main() { 
  std::cout << HasX<A>::value << std::endl; // 1
  std::cout << HasX<B>::value << std::endl; // 0
}
```

所以你可以發現它基本上就是個 traits class，只不過只用了古典 C\+\+ 的東西。

### Fallback 與 Derived

那這邊就來講一下這份東西到底在幹嘛，首先是這兩個 `struct`：

```cpp
struct Fallback { int x; }; // introduce member name "x"
struct Derived : T, Fallback { };
```

`Fallback` 是個 Mixin class，而 `Derived` 繼承了 `T` 和 `Fallback`，這裡有個重點就是 `Fallback` 裡面有個 member 叫 `x`，而 `T` 裡面也有可能有個 member 叫 `x`，這在後面可能會導致 ambiguous，以此來利用 SFINAE 達成目的。

### struct ChT

再來是 `ChT`：
```cpp
template<typename C, C>
struct ChT; 
```

這個 `ChT` 在 `f` 裡面會用到，他的第一個參數為一般的 template parameter，而第二個則是 non-type template parameter，型態為前面引入的 `C`。

### function template `f`

`f` 有兩個：
```cpp
template<typename C>
static char (&f(ChT<int Fallback::*, &C::x>*))[1]; 

template<typename C>
static char (&f(...))[2];
```

不熟語法的朋友可能會連這兩個 `f` 的型態是什麼都不知道，這邊的 `f` 是兩個 function template，上面那個回傳 reference to `char[1]`，也就是 `char(&)[1]`，而下面那個則回傳 reference to `char[2]`，也就是 `char(&)[2]`。

上面那個 function 的參數是 `ChT<int Fallback::*, &C::x>*`，`int Fallback::*` 是個 pointer to `int` data member，也就是指向整數的成員指標，而 `&C::x` 則是 `ChT` 的第二個 template parameter，等等在呼叫時我們會使用 `f<Derived>(0)`，因此這邊才會是 `Fallback` 的 pointer to data member。

### 使用 SFINAE 之處：`f<Derived>(0)`

最後是 `value`：
```cpp
static bool const value = sizeof(f<Derived>(0)) == 2;
```

這段 SFINAE 進行的邏輯是這樣的，因為 `Derived` 繼承了 `T`，若 `T` 裡面也有成員 `x`，那麼在實例化 `f(ChT<int Fallback::*, &C::x>*)` 時，由於 `Derived` 內有兩個 `x` (分別為 `Fallback` 內的 `x` 與 `T` 內的 `x`)，因此會造成 ambiguous，進而導致替換失敗，因此呼叫時使用的是第二個 `f`，換句話說 `sizeof(f<Derived>(0))` 為「2」

而若 `T` 內沒有成員 `x`，則根據 overload resolution，會呼叫到上面的 `f`，換句話說就是 `sizeof(f<Derived>(0))` 為「1」。

### 小結

由此你可以發現它繞了一些圈子去使用 SFINAE 來判斷，而且它並沒有辦法檢查型態，且要檢查的成員，也就是 `Fallback` 內的成員是寫死的，所以我們通常會結合 macro 來使用：
```cpp
#include <iostream>

#define CREATE_TRAITS_MEMBER(TARGET)                             \
  template <typename T>                                          \
  struct Traits_##TARGET {                                       \
    struct Fallback {                                            \
      int TARGET;                                                \
    };                                                           \
    struct Derived : T, Fallback {};                             \
                                                                 \
    template <typename C, C>                                     \
    struct ChT;                                                  \
                                                                 \
    template <typename C>                                        \
    static char (&fn(ChT<int Fallback::*, &C::TARGET> *))[1];    \
                                                                 \
    template <typename C>                                        \
    static char (&fn(...))[2];                                   \
                                                                 \
    static bool const value = sizeof(fn<Derived>(nullptr)) == 2; \
  }

#define HAS_MEMBER_V(SUBJECT, TARGET) \
  Traits_##TARGET<SUBJECT>::value

struct A {
  int x;
};

struct B {
  int x2;
  int i;
};

struct C {
  char x;
  void fn();
};

struct D {
  struct X {};
  X x;
};

CREATE_TRAITS_MEMBER(x);
CREATE_TRAITS_MEMBER(i);
CREATE_TRAITS_MEMBER(fn);

int main()
{
  std::cout << std::boolalpha;
  std::cout << HAS_MEMBER_V(A, x) << '\n';    // true
  std::cout << HAS_MEMBER_V(B, x) << '\n';    // false
  std::cout << HAS_MEMBER_V(C, x) << '\n';    // true
  std::cout << HAS_MEMBER_V(D, x) << '\n';    // true
  std::cout << '\n';

  std::cout << HAS_MEMBER_V(A, i) << '\n';    // false
  std::cout << HAS_MEMBER_V(B, i) << '\n';    // true
  std::cout << HAS_MEMBER_V(C, i) << '\n';    // false
  std::cout << HAS_MEMBER_V(D, i) << '\n';    // false
  std::cout << '\n';

  std::cout << HAS_MEMBER_V(A, fn) << '\n';    // false
  std::cout << HAS_MEMBER_V(B, fn) << '\n';    // false
  std::cout << HAS_MEMBER_V(C, fn) << '\n';    // true
  std::cout << HAS_MEMBER_V(D, fn) << '\n';    // false
}
```

# 總結

恭喜你看完了，幫大家整理一下脈絡，我們一開始講了 C\+\+20 的 concept，接著以在使用 C++11~C++17 的前提下講解了該如何做 Detection Idiom，其中包含了：

1. Overload Resolution
2. SFINAE
3. std::enable_if
4. decltype 與 std::declval
5. std::false_type 與 std::true_type
6. std::void_t

之後舉了幾個例子，並利用這些東西實作出了一個簡化版的 concept。

文末提及了尚未納入標準庫的 `std::is_detected` 的實作，與如何在古典 C++ 中實作 Detection Idiom。

目前實務上常常有在使用 template 但升級不上 C\+\+20 時的狀況 (我最近就遇到了 ROS1 僅支援到 C\+\+17 的問題)，因此很建議大家熟悉各版本的實作，這也可以幫助你在寫 template 的時候更好的理解你自己到底在幹嘛，畢竟這頓操作下來大概也很熟 SFINAE 了，蠻建議大家自己實作一次看看的。

template 真的是個大坑，畢竟~~實作泛型就是為了接受更多的苦難~~，沒有啦是更多型態啦ㄏㄏ

![](https://i.imgur.com/gSIbsYm.png)

# 參考資料

**<a href = "https://zhuanlan.zhihu.com/p/21314708" class = "redlink">1. C++模板进阶指南：SFINAE</a>**

**<a href = "https://tjsw.medium.com/%E6%BD%AE-c-20-concepts-c-%E7%B7%A8%E8%AD%AF%E6%9C%9F%E6%AA%A2%E6%9F%A5%E7%9A%84%E6%AD%A3%E6%B4%BE%E9%81%93%E8%B7%AF-3db8bec914a4" class = "redlink">2. C\+\+20 | Concepts - C\+\+ 編譯期檢查的正派道路</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/constraints" class = "redlink">3. Constraints and concepts (since C++20)</a>**

**<a href = "https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1452r2.html" class = "redlink">4. P1452R2</a>**

**<a href = "https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1084r2.pdf" class = "redlink">5. P1048R2</a>**

**<a href = "https://tjsw.medium.com/%E6%BD%AE-c-17-constexpr-if-part-2-%E5%86%8D%E6%9C%83%E4%BA%86-std-enable-if-%E5%86%8D%E4%B9%9F%E4%B8%8D%E7%94%A8%E6%A8%A3%E7%89%88%E7%89%B9%E5%8C%96%E4%BA%86-86f06ac768da" class = "redlink">6. 潮．C++17 | constexpr if (2) 再會了 enable_if !? 再也不用樣版特化了</a>**

**<a href = "https://www.youtube.com/watch?v=b5Kbzgx1w9A&t=2204s" class = "redlink">7. Back To Basics: Overload Resolution - CppCon 2021</a>**

**<a href = "https://hedzr.com/c++/algorithm/cxx-is_detected-and-detection-idioms/#%E5%85%B3%E4%BA%8E-stdis_detected" class = "redlink">8. 实作中的 std::is_detected 和 Detection Idioms (C++17)</a>**

**<a href = "https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature" class = "redlink">9. Check if a class has a member function of a given signature</a>**

**<a href = "https://stackoverflow.com/questions/1005476/how-to-detect-whether-there-is-a-specific-member-variable-in-class" class = "redlink">10. How to detect whether there is a specific member variable in class?</a>**

**<a href = "https://cpptalk.wordpress.com/2009/09/12/substitution-failure-is-not-an-error-2/" class = "redlink">11. Substitution failure is not an error, part II</a>**