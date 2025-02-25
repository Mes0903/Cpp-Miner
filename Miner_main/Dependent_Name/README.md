---
title: (WIP) 礦坑系列 ── Dependent Name 與 Name lookup
date: 2025-02-15
tag: C++ Miner-main
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?" height = 50><br>Dependent Name & Name lookup</center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 前言

在對 C\+\+ 做語法解析的時候，編譯器需要知道某個名稱是不是一個型態，像是這個例子：

```cpp
t * f;
```

在 C\+\+ 中，根據 `t` 的含意，這個 statement 可能會產生不同的解釋。 如果 `t` 是一個型態，那這個會是指標 `f` 的宣告；但如果 `t` 不是類型，那就會是一個乘法操作

看一下 spec 怎麼解釋這個行為：

> [n4861(6.1.8)](https://timsong-cpp.github.io/cppwp/n4861/basic.pre#8): Some names denote types or templates. In general, whenever a name is encountered it is necessary to determine whether that name denotes one of these entities before continuing to parse the program that contains it. The process that determines this is called *name lookup*.

意思大概就是名字可能會是型態或是模板，因此遇到一個名字的時候需要確認先確定它是不是型態或模板，確認完才能再繼續做語法的解析。 這個過程就稱為名稱查找(name lookup)

那如果 `T` 是個模板參數，編譯器要如何確認 `T::x` 是什麼東西呢? `x` 可以是個 `int` 成員，這樣乘法會是個合法行為；`x` 也可以是 `T` 內部的 `typedef`

因此在模板（類模板和函式模板）的定義裡面，某些建構的意義可能會因實例化而異。類型和表達式的推導可能會取決於「<span class = "yellow">template parameters 的型態</span>」和「<span class = "yellow">non-type template parameters 的值</span>」。這種東西我們就將其稱為待決名(Dependent Name)，在後面我們會講得更詳細一點

而對於待決名和非待決名的名稱查找與綁定，規則是不一樣的

在編譯器的實作上，可能會等到使用者實例化模板後，才去真正的解析 `T::x * f` 這樣的語句。 編譯器基本上會將模板的內容複製到內部的 buffer，在需要實例化時才開始解析模板，並檢測定義中的錯誤

然而，對於不是待決名的名稱，則不會等待到實例化時才開始解析，而是在 template definition 的階段進行名稱查找與綁定，讓我們看個例子：

```cpp
#include <iostream>
 
void g(double) { std::cout << "g(double)\n"; }
 
template <class T>
struct S {
  void f() const
  {
    g(1);    // "g" is a non-dependent name, bound now
  }
};
 
void g(int) { std::cout << "g(int)\n"; }
 
int main()
{
  g(1);    // calls g(int)

  S<int> s;
  s.f();    // calls g(double)
}
```

輸出：

```bash
g(int)
g(double)
```

這個例子中，我們不需要確定 `T` 是什麼後才能知道 `g` 是個型態還是模板之類的，因此 `g` 不是待決名，因此在 `S` 這個類模板完成時就會進行名稱查找與綁定了

也因此，即使在定義完成後，發現外面有一個 overload resolution rank 更優的 `g(int)`，它呼叫的函式也還是當下能綁定的 `g(double)`

接下來我們就來詳細看一下 C\+\+ 中的名稱查找與待決名吧~

# Unqualified Name & Qualified Name 

Unqualified Name 的中文叫做「非限定名稱」，而 Qualified Name 叫做「限定名稱」，這兩個名詞我比較習慣講英文，所以本文會用英文來來表達

其實 cppreference 給的解釋就差不多了：

> [Unqualified name lookup](https://en.cppreference.com/w/cpp/language/unqualified_lookup): For an unqualified name, that is a name that does not appear to the right of a scope resolution operator `::`
> [Qualified name lookup](https://en.cppreference.com/w/cpp/language/qualified_lookup): A qualified name is a name that appears on the right hand side of the scope resolution operator `::` (see also qualified identifiers)

所以判斷方法很直覺，前面有 `::` 的是 qualified name，沒有的就是 unqualified name

這個 `::` 叫做 scope resolution operator，中文叫做範圍解析運算子，基本上就是幫忙指定名稱域的運算子。 如果 `::` 的左邊沒接上名字，那代表全域(global scope)，例如 `::x` 表示全域中的 `x`

標準內對於 qualified name 與 unqualifed name 的定義其實也差不多：

[n4861(7.5.4.1)](https://timsong-cpp.github.io/cppwp/n4861/expr.prim.id#unqual)：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Dependent_Name/image/unqualified_id.png?raw=true">

</center>

[n4861(7.5.4.2)](https://timsong-cpp.github.io/cppwp/n4861/expr.prim.id#qual)：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Dependent_Name/image/qualified_id.png?raw=true">

</center>

接下來標準內就是講講每項的細節，但重點主要放在 declaration 和 expression，跟我們在意的東西不一樣，所以有興趣的再點進去看ㄅ

# 名稱查找

那在 C\+\+ 中，名稱查找有兩種分類方式，第一種比較正式，以 qualified 與 non qualified 來分：

- Unqualified name lookup
    - 對於函式的名稱而言還會有 Argument-dependent lookup(ADL)
- Qualified name lookup

第二種分法則以是不是 ADL 來分，畢竟 ADL 比較特殊(不直覺)：

- ordinary lookup (普通查找)
- Argument-dependent lookup(ADL)

在 C\+\+ 中，首先會進行 ordinary lookup，然後才會是 ADL。 但由於第一種分類比較正式，所以接下來還是以第一種分法為例，帶大家看一下這三種名稱查找

這邊會以 cppreference 和標準為主來做翻譯，並看情況補充一些東西：

- [Unqualified name lookup](https://en.cppreference.com/w/cpp/language/unqualified_lookup)
- [Qualified name lookup](https://en.cppreference.com/w/cpp/language/qualified_lookup)
- [n4868(6.5)](https://timsong-cpp.github.io/cppwp/n4861/basic.lookup)

## Unqualified Name lookup

接下來基本上就是一堆規則，名稱查找會按照每個對應規則中列出的順序在 scope 中尋找宣告，直到找到至少一個宣告就會停止名稱查找，如果沒有找到對應的宣告，則 program 為 ill-formed(簡單來說就是錯的)

### 1. 全域/文件作用域 (Global scope/File scope)

在全域(top-level namespace) 範圍內使用的名稱，在任何函數、類別或使用者聲明的命名空間之外，應在其在全域範圍內使用之前進行宣告

```cpp
int n = 1;    // declaration of n
int x = n + 1;    // OK: lookup finds ::n

int z = y - 1;    // Error: lookup fails
int y = 2;    // declaration of y
```

### 2. 命名空間作用域 (Namespace scope)

對於在使用者宣告的命名空間內，且在任何函式或類之外所使用的名字，首先會查找該命名空間中，該次使用之前的部分，然後查找外圍命名空間在宣告該命名空間之前的部分，以此類推，直到抵達全域

簡單來說就是由當下那行往上找宣告：

```cpp
int n = 1;    // declaration

namespace N
{
  int m = 2;

  namespace Y 
  {
    int x = n;    // OK, lookup finds ::n
    int y = m;    // OK, lookup finds ::N::m
    int z = k;    // Error: lookup fails
  }    // namespace Y

  int k = 3;
}    // namespace N
```

例子中的 `z` 會找不到 `k`，因為 `k` 出現在 `Y` 的宣告後面

如果變數的定義在 namespace 外面，則會從變數宣告的 scope 開始查找：

```cpp
namespace X
{
  extern int x;    // declaration, not definition
  extern int y;    // declaration, not definition
  extern int z;    // declaration, not definition
  int n = 1;    // found 1st
}    // namespace X

int n = 2;    // found 2nd
int X::x = n;    // finds X::n, sets X::x to 1

int m = 3;
int X::y = m;    // finds ::m, sets X::y to 3

int X::z = i;    // error
int i = 4;
```

這裡的 `int X::x = n;` 與 `int X::y = m;` 為 `X::x` 與 `X::y` 的定義，因此會先從 `X` 內開始尋找名稱，之後一樣往外面找，在全域時一樣從定義處往前找

對於這三個變數：

- `X::n` 存在，因此 `X::x` 為 `1`
- `X::m` 不存在，但 `::m` 存在且出現在 `X::y` 的定義之前，因此 `X::y` 為 `3`
- `X::i` 不存在，而 `::i` 雖然存在但出現在 `X::z` 之後，因此 `X::i = i` 處名稱查找失敗

### 3. 非成員函式定義 (Non-member function definition)

```cpp
namespace A
{
    namespace N
    {
        void f();
        int i = 3; // found 3rd (if 2nd is not present)
    }
 
    int i = 4;     // found 4th (if 3rd is not present)
}
 
int i = 5;         // found 5th (if 4th is not present)
 
void A::N::f()
{
    int i = 2;     // found 2nd (if 1st is not present)
 
    while (true)
    {
       int i = 1;  // found 1st: lookup is done
       std::cout << i;
    }
}
 
// int i;          // not found
 
namespace A
{
    namespace N
    {
        // int i;  // not found
    }
}
```

## Qualified Name lookup

## Argument-dependent lookup(ADL)

# Dependent Name (待決名)

如同前言中所說的，在模板（類模板和函式模板）的定義裡面，某些建構的意義可能會因實例化而異。類型和表達式的推導可能會取決於「<span class = "yellow">template parameters 的型態</span>」和「<span class = "yellow">non-type template parameters 的值</span>」。這種東西我們就將其稱為待決名(Dependent Name)，舉個例子：

```cpp
template<typename T>
struct X : B<T> // “B<T>” is dependent on T
{
    typename T::A* pa; // “T::A” is dependent on T
                       // (see below for the meaning of this use of “typename”)
 
    void f(B<T>* pb)
    {
        static int i = B<T>::i; // “B<T>::i” is dependent on T
        pb->j++; // “pb->j” is dependent on T
    }
};
```

這裡 `B<T>` 就依賴於 `T`，`T::A` 也依賴於 `T`，`B<T>::i` 也依賴於 `T`，甚至 `pb->j` 也依賴於 `T` 因為 `pb` 的型態為 `B<T>`

再看個更具體的例子：

```cpp
T::x * f;
```

在這裡，如果 `T::x` 這個 name 是一個值，則這整個 expression 會是一個乘法，但如果它是一個 type，例如在 `T` 裡面使用 `typedef` 將 `x` 設為 `int`，則這會變成一個 pointer declaration

如果一個名字有這類的問題，需要到知道 template argument 實際的型態後才能確定 name 的性質的話，則這個 name 就被稱為 dependent name

如同前言所述，在編譯器的實作上，可能會等到使用者實例化模板後，才去真正的解析 `T::x * f` 這樣的語句。 編譯器基本上會將模板的內容複製到內部的 buffer，在需要實例化時才開始解析模板，並檢測定義中的錯誤

## 利用 typename 消除待決名的歧義

在模板的宣告或定義中，不是當前實例化成員且依賴模板參數的名稱不會被視為類型，除非它已經有建立了類型名稱，例如使用 `typedef` 宣告，或用於命名基底類別

在不被視為類型的情況下，就需要使用 `typename` 來告訴編譯器如何解析它。 換句話說，為了解決前面提到的 `T::x` 的歧義，我們需要手動告訴編譯器如何解析這個語句

做法也很簡單，只需要在待決名的前方加上 `typename` 的前綴就好

> [n4861(13.8.2)](https://timsong-cpp.github.io/cppwp/n4861/temp.res#2): A name used in a template declaration or definition and that is dependent on a *template-parameter* is assumed not to name a type unless the applicable name lookup finds a type name or the name is qualified by the keyword `typename`.

但要注意，`typename` 只能放在 qualified names 前面，也就是 `T::x` 這種有 `::` 運算子的形式，不過無論名稱是否為待決名都可以加

更確切的說，對於 Unqualified name，透過名稱查找，編譯器都<span class = "yellow">「一定」</span>能正確的解析該語句，例如 `T * f`，這只會是一個 pointer declaration，因為 `T` 是 template argument，所以它是型態，但對於 `T::x` 就不一定了，因此必須寫成 `typename T::x`

看下面這個例子：

```cpp
// no B declared here

class X;

template <typename T>
class Y {
  class Z;    // forward declaration of member class

  void f()
  {
    X *a1;    // declare pointer to X
    T *a2;    // declare pointer to T
    Y *a3;    // declare pointer to Y<T>
    Z *a4;    // declare pointer to Z

    typename ::X *a5;    // typename before qualified non-dependent name, ok

    typedef typename T::A TA;
    TA *a6;    // declare pointer to T's A

    typename T::A *a7;    // declare pointer to T's A

    T::A *a8;    // error: no visible declaration of a7
                 // T​::​A is not a type name; multiplication of T​::​A by a7
    B *a9;    // error: no visible declarations of B and a8
              // B is not a type name; multiplication of B by a8
    typename B *a10;    // error: typename before non-qualified name
  }
};
```

輸出：

```bash
<source>: In member function 'void Y<T>::f()':
<source>:22:11: error: 'a8' was not declared in this scope; did you mean 'a7'?
   22 |     T::A *a8;    // error: no visible declaration of a7
      |           ^~
      |           a7
<source>:25:5: error: 'B' was not declared in this scope
   25 |     B *a9;
      |     ^
<source>:25:8: error: 'a9' was not declared in this scope; did you mean 'a7'?
   25 |     B *a9;
      |        ^~
      |        a7
<source>:27:14: error: expected nested-name-specifier before 'B'
   27 |     typename B *a10;    // error: typename before non-qualified name
      |              ^
<source>:27:14: error: expected '(' before 'B'
Compiler returned: 1
```

可以雖然有許多名稱不需要 `typename` 關鍵字，編譯器可以透過模板定義中的名稱查找來知道如何建構，例如 `T *a2;`，但對於 `T::A * a7;` 這種形式，就必須寫成 `typename T::A *a7;`

對於 `T::A *a8;` 與 `B *a9` 這兩個例子，編譯器都把他們當乘法了，因此報了錯說找不到 `a8`、`B` 和 `a9` 的宣告。 


## 利用 template 消除歧義

再來看個例子：

```cpp
boost::function< int() > f;
```

這對我們來說看起來可能很直覺的就會認為這是一個 template class 的 instance，但是對編譯器來說這也是有歧義的，假設實際情況如下：

```cpp
namespace boost { int function = 0; }

int main() { 
    int f = 0;
    boost::function< int() > f; 
}
```

則當中的 `<` 為 less-than operator，`>` 為 greater-than operator，因此這仍是合法的語句。 不過我們都知道在該 lib 中 `boost::function` 實際上是個 template，因此 compiler 進行 name lookup 是有辦法判別的：

> [n4861(13.3-3)](https://timsong-cpp.github.io/cppwp/n4861/temp#names-3)：When a name is considered to be a *template-name*, and it is followed by a <, the < is always taken as the delimiter of a *template-argument-list* and never as the less-than operator. When parsing a *template-argument-list*, the first non-nested > is taken as the ending delimiter rather than a greater-than operator. Similarly, the first non-nested >> is treated as two consecutive but distinct > tokens, the first of which is taken as the end of the *template-argument-list* and completes the *template-id*. [ Note: The second > token produced by this replacement rule may terminate an enclosing *template-id* construct or it may be part of a different construct (e.g., a cast). — end note ]

這邊標準說如果一個 name 是 template 的名字，則 `<` 與<span class = "yellow">第一個</span> `>` 會被解析為 `template<>` 的一部分，而非 less-than operator，看個例子：

```cpp
template<int i> class X { /* ... */ };

X< 1>2 > x1;                            // syntax error
X<(1>2)> x2;                            // OK

template<class T> class Y { /* ... */ };
Y<X<1>> x3;                             // OK, same as Y<X<1> > x3;
Y<X<6>>1>> x4;                          // syntax error
Y<X<(6>>1)>> x5;                        // OK
```

應該一目了然，這個例子就先不多做解釋

現在回來想個情境，如果我們還不確定一個 name 是不是 template 怎麼辦? 這時就與 `typename` 的解法一樣，我們需要加上 `template` 以消除歧義

> [n4861(13.3-4)](https://timsong-cpp.github.io/cppwp/n4861/temp#names-4)：The keyword template is said to appear at the top level in a *qualified-id* if it appears outside of a *template-argument-list* or *decltype-specifier*. In a *qualified-id* of a *declarator-id* or in a *qualified-id* formed by a *class-head-name* or *enum-head-name*, the keyword `template` shall not appear at the top level. In a *qualified-id* used as the name in a *typename-specifier*, *elaborated-type-specifier*, *using-declaration*, or *class-or-decltype*, an optional keyword `template` appearing at the top level is ignored. In these contexts, a < token is always assumed to introduce a *template-argument-list*. In all other contexts,<span class = "yellow">when naming a template specialization of a member of an unknown specialization, the member template name shall be prefixed by the keyword template</span>.

看個例子：

```cpp
struct X {
  template<std::size_t> X* alloc();
  template<std::size_t> static X* adjust();
};

template<class T> 
void f(T* p) {
  T* p1 = p->alloc<200>();              // error: < means less than
  T* p2 = p->template alloc<200>();     // OK: < starts template argument list
  T::adjust<100>();                     // error: < means less than
  T::template adjust<100>();            // OK: < starts template argument list
}
```

這裡的 `alloc` 與 `adjust` 為兩個 template specialization，但在函式 `f` 內，我們無從得知  
`p->alloc` 與 `T::adjust` 是不是 template specialization，因此需要在前方加上 `template` 的關鍵字以消除歧義

# Template 的建構

## Dependencies

講完了如何使用，接下來來談談標準，在標準中隨著 template arguments 的不同，template declarations 的建構也會有不同的意義。 具體來說，由於 types 和 expressions 會依賴於 template parameters 的 type 或 value，會導致 name lookup 的行為不同

標準依照 types 與 expressions 的依賴對象進行了分類，其中 types 依賴於 type，而 expressions 則可依賴於 type 與 value，因此可以簡單的將分類畫成下圖：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Dependent_Name/image/dependencies.png?raw=true">

</center>

- Dependent types (e.g: a type template parameter `T`)
- Value-dependent expressions (e.g: a non-type template parameter `N`)
- Type-dependent expressions (e.g: a cast to a type template parameter `(T)0`)

大部分的規則都是比較值觀的，而且是可以 recursively 套用的，舉個例子，對於 `T[N]` 這個 type，如果 `N` 是個 Value-dependent expression，或者 `T` 是個 Dependent type，則 `T[N]` 也是一個 Dependent type

詳細的內容留到後面的章節，基本上就是把 cppreference 和標準的內容拿出來翻譯和補充



## Dependent types
## Type-dependent expressions
## Value-dependent expressions

# 參考資料

- [Where and why do I have to put the "template" and "typename" keywords?](https://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-keywords)








