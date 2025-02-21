---
title: 礦坑系列 ── C++03 ~ C++17 主要變化介紹
date: 2022-05-23
tag: C++ Miner
category: C++ Miner
---

# 礦坑系列 ── C++03 ~ C++17 主要變化介紹 

點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>  

# Modern Cpp 在追求什麼?  

來到 Modern Cpp，Cpp 已經不僅僅是個有物件導向的 C 了，其還追求 type-safe, metaprograming 與 static/compile time error 等議題，並包含了如 functional language 的特性進來，已經與以往的 Cpp 長的完全不一樣了。  

如果要我講個我認為 Modern C\+\+ 最重要的東西出來，我會選 RAII 與 move，在 modern C\+\+ 前你會在 code 裡面發現許多的 new/delete 與複製，這代表我們時常需要手動確保我們資源的生命週期，而 RAII 與 move 幫助我們做了這件事，而且做得很好，但也因此 C\+\+ 的核心觀念有些改動。  

這種例子在近代 C\+\+ 內很多，然後在眾多改動中我覺得這個是最重要的改動，且其他大部分的改動其實都是語法糖，不過 spec 會對這個語法糖進行類似「語意」上的定義，或說某些條件，有點像在自己定義出新的概念，並繼續在此概念上疊上新的概念，但本質上整個都是語法糖，也因此語意在 C\+\+ 內是個很重要的因素，但我有看見許多人並不清楚他在使用的語意為何，又或說「味道」更為精確。  

這邊提供一個工具叫做 C\+\+ Insights [(連結)](https://cppinsights.io/)，這個網站可以看見 Compiler 實際上看到的 code 「大概」長怎樣，這解析出來的結果基本上都很接近了，不過因為這還是一個 interpretation 的東西，所以可能還是有小差別，但很好用了，舉個例子，大家常說的 lambda 其實是語法糖，本質上是個匿名 class：  

輸入：  

```cpp
int main()  
{  
  [](){};  
}  
```

輸出：  

```cpp
int main()  
{  
  class __lambda_3_2 {  
  public:  
    inline /*constexpr */ void operator()() const  
    {  
    }  

    using retType_3_2 = void (*)();  
    inline /*constexpr */ operator retType_3_2() const noexcept  
    {  
      return __invoke;  
    };  

  private:  
    static inline void __invoke()  
    {  
    }  
  } __lambda_3_2{};  

  ;  
  return 0;  
}  
```

非常好用~~  

接下來我就介紹一下 Jason Turner 整理的例子，看一下 Modern C\+\+ 內 C\+\+03 到 C\+\+17 有什麼重要的東西 & 概念，不會講太深，簡單介紹一下而已。  

# C++ 98 & C++03  

source：[C++ Weekly - Ep 173 - The Important Parts of C++98 in 13 Minutes](https://www.youtube.com/watch?v=78Y_LRZPVRg)  

## 建構子與解構子  

最初許多人稱 C\+\+ 為 C with Object Oriented 的原因就是因為 C\+\+ 引入了對物件導向的支援，我們能夠自己定義一個型態，這個型態的物件被建構出來時會去呼叫建構子，當物件被解構時會去呼叫解構子：  

```cpp
class T {  
public:  
  T() { puts("T()"); }  
  ~T() { puts("~T()"); }  
};  
```

這裡 `T()` 就是一種建構子，而 `~T()` 就是一種解構子。  

## RAII  

RAII 全名為 Resource Acquisition Is Initialization，意思為資源取得即初始化，是由 C++ 之父 Bjarne Stroustrup 提出的，這裡的 Resource 指的主要是指系統中有限的東西，像是記憶體、socket 等等，利用 RAII 可以很好的自動管理這類資源。  

```cpp
#include <iostream>  

class T {  
public:  
  T() { puts("T()"); }  
  ~T() { puts("~T()"); }  
};  

int main()  
{  
  puts("before scope");  

  {  
    T t;  
  }    // use RAII to destruct object  

  puts("after scope");  
  // t = {};    // error  
}  
```

輸出：  

```bash  
before scope  
T()          
~T()         
after scope  
```

在物件建構時，就會自動去呼叫建構子進行建構，在離開物件對應的 scope 時，會自動進行資源的回收，因此呼叫解構子，這就是 RAII 最主要的概念，這用在 `fstream`、`mutex` 等物件上十分好用。  

讀更多：[c++经验之谈一：RAII原理介绍](https://zhuanlan.zhihu.com/p/34660259)  

## 模板 (Template)  

模板是 metaprogramming 的基礎之一，他可以幫助我們生成 code，看下面這個例子：  

```cpp
template<typename First, typename Second>  
struct T{  
  First i;  
  Second j;  
};  
```

當我們像這樣 `T<int, bool> t;` 建構這個物件時，第一個型態 `First` 就會被替換為 `int`，第二個型態就會變成 `bool`，相當於 compiler 幫你「寫」了一段 code 長：  

```cpp
struct T {  
  int i;  
  bool j;  
};  
```

template 是個大坑，越後面的版本可以玩的越花俏，另外 template 是圖靈完備的，十分有趣。  

額外閱讀：[C++ templates Turing-complete?](https://stackoverflow.com/questions/189172/c-templates-turing-complete)  

順代一提，`printf` 也是圖靈完備的，有個東西甚至叫 printf-oriented programming，有興趣的可以看看：[printf-tac-toe](https://github.com/carlini/printf-tac-toe)，他只用 `printf` 就寫了一個圈圈叉叉的遊戲，黑魔法。  

## algorithm  

在 C++98 後我們可以使用 standard algorithm：  

```cpp
#include <algorithm>  
#include <vector>  

int main()  
{  
  std::vector<int> vec = { 3, 1, 2 };  
  std::sort(vec.begin(), vec.end());    // The elements of vec will be 1 2 3.  
}  
```

有沒有在 standard lib 裡面是差很多的，這影響到程式的一致性。  

# C\+\+11 & C\+\+14  

C\+\+11 是一個大更新，而 C\+\+14 則完善了 C\+\+11，做了一些延伸，並沒有特別新增什麼東西，因此我兩個擺在一起講。  

source：[C++ Weekly - Ep 176 - Important Parts of C++11 in 12 Minutes](https://www.youtube.com/watch?v=D5n6xMUKU3A)  
source：[C++ Weekly - Ep 178 - The Important Parts of C++14 In 9 Minutes](https://www.youtube.com/watch?v=mXxNvaEdNHI)  

## auto  

在某些情況下我們可能並不是特別在意某個物件的型態，在 C\+\+11 後我們可以使用 auto，舉個例子：  

```cpp
#include <algorithm>  
#include <vector>  

void count_things(const std::vector<int> &vec, int value)  
{  
  const auto count = std::count(std::begin(vec), std::end(vec), value);  
}  
```

在這邊 `count` 是什麼型態我們可能不是那麼在意，這時候直接使用 `auto` 就好，`auto` 會自動幫我們推斷出他的型態，這在 generic programming 時非常有用：  

```cpp
#include <algorithm>  
#include <vector>  

template <typename T>  
void count_things(const T &vec, int value)  
{  
  const auto count = std::count(std::begin(vec), std::end(vec), value);  
}  
```

在 C\+\+14 後我們可以使用 `auto` 來當作 return type，他會幫我們推斷 function 回傳的型態：  

```cpp
#include <algorithm>  
#include <vector>  

template <typename T>  
auto count_things(const T &vec, int value)  
{  
  const auto count = std::count(std::begin(vec), std::end(vec), value);  
  return count;  
}  
```

當然他還是有一些的規則和限制，不過這邊就只先簡單介紹。  

## ranged-for loop  

ranged-for loop 可以幫助我們很簡單的遍歷一個可以使用 `std::begin` 與 `std::end` 的容器，在 C\+\+98 時我們會這樣寫：  

```cpp
#include <vector>  

void travel_thing(const std::vector<int> &vec)  
{  
  for (std::vector<int>::const_iterator it = vec.begin();  
       it != vec.end();  
       ++it)   
  {  
    // do thing with *itr  
  }  
}  
```

而在 C\+\+11 後可以使用 ranged-for，這常搭配 `auto` 使用，因為元素的型態在這裡常常不是這麼的重要：  

```cpp
#include <vector>  

void travel_thing(const std::vector<int> &vec)  
{  
  for (const auto &elem : vec) {  
    // do thing with elem  
  }  
}  
```

如此一來可讀性便增加了許多，非常簡潔，不過速度並不會差太多。  
## lambda  

lambda 可以幫助我們增加可讀性與便利性，看一下這個例子：  

```cpp
#include <algorithm>  
#include <vector>  

template <typename T>  
void count_things_less_than_3(const T &vec)  
{  
  const auto count = std::count_if(std::begin(vec), std::end(vec),  
    [](int i) { return i < 3; }    // element need to be converted to int  
  );  
}  
```

原先我們可能需要有額外的 function 來當作第三個參數，C\+\+11 後可以直接使用 lambda，這樣可以避免汙染名稱空間，同時別人也比較好找你的比較方法。lambda 對於我們在寫 functional programming 的時候也有一定的幫助。  

在 C\+\+14 後我們可以使用 generic lambda，在上例中，我們可以傳進 lambda 的參數可以不只是 int，這時候我們就可以使用 generic lambda，也就是在 lambda 中使用 auto：  

```cpp
#include <algorithm>  
#include <vector>  

template <typename T>  
void count_things_less_than_3(const T &vec)  
{  
  const auto count = std::count_if(std::begin(vec), std::end(vec),  
    [](const auto i) { return i < 3; }  
  );  
}  
```

另外在 lambda 的捕捉清單內也可以使用 generalized capture expression，長的會像這樣：  

```cpp
#include <algorithm>  
#include <vector>  

template <typename T>  
void count_value(const T &vec, int value)  
{  
  const auto count = std::count_if(std::begin(vec), std::end(vec),  
    [value = 3](int i) { return i < 3; }    // element need to be converted to int  
  );  
}  
```

也就是說我們可以在前面的 `[]` 內寫初始化了，這是因為 C\+\+11 的時候只提供了 by copy 和 by reference 兩種方式，沒有提供 move，因此在 C\+\+14 的時候被完善了，這讓我們可以寫出很多神奇的東西：  

```cpp
#include <algorithm>  
#include <vector>  

template <typename T>  
void count_value(const T &vec, int value)  
{  
  const auto count = std::count_if(std::begin(vec), std::end(vec),  
    [value = static_cast<int>([]() { return 3.0f; }())](const auto i) { return i < value; }    // element need to be converted to int  
  );  
}  
```

這裡 value 利用一個 double 轉型後的結果初始化，只是告訴大家這樣合法，長的很酷，但不要這樣寫，你的同事可能會把你打死  

## variadic templates  

variadic template 讓模板用起來更加的便利，可以讓 compiler 幫助我們展開一些 code，一個簡單的應用是傳遞不定個數的參數，看起來會像這樣：  
```cpp
template <typename Func, typename... T>  
void caller(const Func &func, const T &...param)  
{  
  func(param...);  
}  
```

如此一來無論我們傳了多少參數進 `caller`，他都會幫我們轉送進我們指定的函式。  

## 智慧指標 (smart pointer)  

智慧指標可以幫助我們控制物件的資源，這邊挑了比較有代表性的 `unique_ptr` 介紹，`unique_ptr` 是一種 move only types，也就是說不能複製只能移動，用起來會像這樣：  

```cpp
#include <memory>  

void allocate_memory()  
{  
  std::unique_ptr<int> ptr(new int(5));  
}    // ptr destory, memory free  
```

當 `ptr` 離開對應的 scope 時，其保管的資源便會被自動釋放。  

在 C\+\+14 後我們可以使用 `make_unique` 來 allocate heap 段空間，這大幅減少了我們在 code 中對 `new` 的依賴性，也讓我們可以搭配 `auto` 使用：  

```cpp
#include <memory>  
void allocate_memory()  
{  
  auto ptr{std::make_unique<int>(5)};    
}  
```

額外閱讀：[Advantages of using std::make_unique over new operator](https://stackoverflow.com/questions/37514509/advantages-of-using-stdmake-unique-over-new-operator)  

## constexpr  

`constexpr` 在 C\+\+11 時限制非常的多，任何跟其有關的東西都需要也是 `constexpr` 的，這在後面的版本有放寬使用條件，用 `constexpr` 修飾表示這個東西可以在編譯期就被確定內容，用法大概會像這樣：  

```cpp
constexpr int get_value()  
{  
  return 5 * 3;  
}  

constexpr auto value = get_value();  
```

上例中在編譯時期 `value` 的值就可以被確定為 15。  

在 C\+\+14 中放寬了對 `constexpr` 的限制，我們可以有分支、for loop 這些東西了，也可以有一般的變數：  

```cpp
constexpr int get_value()  
{  
  int val = 5;  
  int val2 = 3;  
  return val * val2;  
}  
```

這在 C\+\+11 的時候是不合法的，但在 C\+\+14 合法。  

# C\+\+17  

source：[C++ Weekly - Ep 190 - The Important Parts of C++17 in 10 Minutes](https://www.youtube.com/watch?v=QpFjOlzg1r4)  

## Copy / Move Elision 的保證  

在 C\+\+17 時 Copy Elision 被保證了：  

```cpp
#include <iostream>  
class T {  
public:  
  T() { puts("T()"); }  
  ~T() { puts("~T()"); }  
};  

T fn()  
{  
  return T();  
}  

int main()  
{  
  T t = fn();  
}  
```

上面這個例子中只會有一個物件被建構。  

## constexpr 於 Stdlib 中開始被普遍支援  

在 C\+\+17 後一些我們常用的東西開始支援 `constexpr` 的版本了，這可以幫助我們更好的利用 `constexpr`：  

![](https://i.imgur.com/CKnBgQN.png)  

上例中較常使用的 `operator[]` 在 C\+\+17 後開始被支援。  

## constexpr lambda  

在 C\+\+17 後可以用 `constexpr` 來修飾 lambda 了：  

```cpp
constexpr auto l = [](){};  
```

這在 C\+\+17 後是合法的了。  

## string_view  

`string_view` 是 `string` 的一個 「view」，也就是說唯讀，不可修改，以往可能會利用 `const std::string&` 來傳遞，C\+\+17 後可以使用 `string_view` 來更好的符合語意，且更解簡潔：  

```cpp
#include <string_view>  
constexpr std::string_view name = "Hello";  
```

有關 `view` 的概念可以看看 TJSW 寫的這篇：[潮．C++20 | Ranges & Views：使用 STL 的全新姿勢](https://tjsw.medium.com/%E6%BD%AE-c-20-ranges-views-%E4%BD%BF%E7%94%A8-stl-%E7%9A%84%E5%85%A8%E6%96%B0%E5%A7%BF%E5%8B%A2-8007777a8da6)  

額外閱讀：[How exactly is std::string_view faster than const std::string&?](https://stackoverflow.com/questions/40127965/how-exactly-is-stdstring-view-faster-than-const-stdstring)  

## Class Template Argument Deduction  

在 C\+\+17 後可以自動推斷 template argument 了：  

```cpp
#include <array>  
std::array data{1, 2, 3, 4, 5};  
```

這裡 Compiler 會自動幫你推斷出他的 template argument 為 `<int, 5>`。  

## fold expression  

在 C\+\+17 後，類似於之前 vairadic template 的功用，我們可以使用 fold expression 來幫助我們更有彈性的處理多個參數的傳遞：  

```cpp
template<typename ...T>  
auto add(const T & param...)  
{  
  return (param + ...);  
}  
```

這件事在 C\+\+17 前我們需要在寫另外一個函式把參數列的第一項拉出來操作，有了 fold expression 可以更方便的做這件事了。  

## Structured Bindings  

在 C\+\+17 後我們可以使用 structured bindings 來幫助我們使用 tuple-like 的容器，如 `std::pair`、`std::tuple` 等：  

```cpp
std::pair<int, int> values{1, 2};  
auto [first, second] = values;  
```

上例中 `first` 會去連結到 `1`，而 `second` 會去連結到 `2`，類似於 reference 的概念，但內部不是 reference。  

## if-init expressions  

C\+\+17 後我們可以在 `if` statement 的圓括號內進行物件初始化了：  

```cpp
void fn(int i, int t){  
  if(int j = i * t;  
     j < 5)  
  {  
    // do something  
  }  
}  
```

這有個 scope 的優勢，以往我們需要手動在外層加上一個 scope 才可以如此利用 RAII，C\+\+17 後就不用了。  

# 總結  

以上就是 C\+\+98 ~ C\+\+17 的重要概念了，其他東西，由於 C\+\+20 我還沒有太熟，所以就不在這裡亂講了，以免誤人子弟，未來有機會再補上。  

至於要用什麼版本則是跟團隊講好就好，Modern Cpp 有很多工具能讓你方便做事，且效率不會變慢，錯誤也可以盡量在 Compile Time 找到，執行時間也可以往 Compile Time 推，然而這些對你來說也有可能不是那麼重要，此時使用熟悉的 C\+\+03 也是一種選擇。 最近也有人開始在說可以從 Rust 入手，也是一個不錯的選擇。  

語言之間並沒有太多優劣之分，希望大家不用戰語言。  