<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 ── 四個你不該用 "const" 的時機 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 四個你不該用 "const" 的時機

source：[C++ Weekly - Ep 75 - Why You Cannot Move From Const](https://www.youtube.com/watch?v=ZKaoR3dP9uM)

source：[C++ Weekly - Ep 322 - Top 4 Places To Never Use "const"](https://www.youtube.com/watch?v=dGCxMmGvocE&t=929s)

主要原因是因為 `const` 會破壞移動，看看這個例子：
```cpp
#include <iostream>

struct S
{
  S() { puts("S()"); }
  S(const S &) { puts("S(const S &)");}
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S &operator=(const S &) { puts("operator=(const S &)"); return *this; }
  S &operator=(S &&) noexcept { puts("operator=(S &&)"); return *this; }
};

int main()
{
  S s1;
  S t1 = std::move(s1);    // rvalue reference, called move

  const S s2;
  S t2 = std::move(s2);    // const rvalue reference, called S(const S&), which is copy

  S s3;
  S &&r1 = std::move(s3);    // ok

  const S s4;
  // S&& r2 = std::move(s4); // error
  const S &&r2 = std::move(s4);
}
```

由於 `std::move()` 基本上是在做轉型，這會保留 const 的修飾，因此若原物件有 const 修飾，就需要使用 const rvalue reference 來做繫結，也因此他會無法去呼叫移動建構子，破壞了移動。

由此可知 `const` 也會破壞隱式移動(implicit move)，有關隱式移動，可以去看之前寫得值類別篇。這邊 Jason Turner 給了四個要注意不該使用 `const` 的例子：

## 當 function 回傳 non-reference type 時，return type 不應該用 const 修飾
    
寫在 return type 的 `const` 大部分的時候都會被忽略，有時候甚至會破壞效能，如這邊提到的它會破壞隱式移動，看看這段 code：

```cpp
#include <iostream>

struct S
{
  S() { puts("S()"); }
  S(const S &) { puts("S(const S &)");}
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S &operator=(const S &) { puts("operator=(const S &)"); return *this; }
  S &operator=(S &&) noexcept { puts("operator=(S &&)"); return *this; }
};

// will called copy
const S make_value()
{
  return S{};
}

// will called move
S make_value2()
{
  return S{};
}

int main()
{
  puts("s");
  S s;
  s = make_value();

  puts("\ns2");
  S s2;
  s2 = make_value2();

  puts("\nend");
}
```
輸出：
```
s
S()
S()
operator=(const S &)
~S()
	
s2
S()
S()
operator=(S &&)
~S()
	
end
~S()
~S()
```
    
這邊有兩個 function，上面那個在 return type 上有 `const` 修飾，下面的沒有。你可以看見上面那個使用的是 copy，而下面那個使用的是 move。
    
原因就如前面所述，一旦加上了 `const`，reference 要連結時就變成要使用 const rvalue reference，但通常 const rvalue reference 並不會有用處(幾乎沒意義，很多餘)，因此通常我們函式不會實作 const rvalue reference 的版本，也因此會去呼叫複製，破壞了隱式移動。
    
當然，const rvalue reference 偶爾會有用就是了，真的很偶爾。
    
額外閱讀：[Do rvalue references to const have any use?](https://stackoverflow.com/questions/4938875/do-rvalue-references-to-const-have-any-use)

## 需要隱式移動時，回傳的變數不該用 const 修飾

原因一樣是因為會破壞隱式移動，看看這個例子：
    
```cpp
#include <iostream>

struct S
{
  S() { puts("S()"); }
  S(const S &) { puts("S(const S &)");}
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S &operator=(const S &) { puts("operator=(const S &)"); return *this; }
  S &operator=(S &&) noexcept { puts("operator=(S &&)"); return *this; }
};

S make_value(bool flag)
{
  if (flag) {
    const S s;    // bad use of const
    return s;
  }
  else {
    const S s2;    // bad use of const
    return s2;    // 直接回傳 S{} 會更好，因為有 Copy Elision
  }
}

int main(int argc, const char *[])
{
  S s = make_value(argc == 1);
}
```
    
輸出：
```
S()
S(const S &)
~S()
~S()
```
    
function 內回傳的是有名物件，因此套用的是 NRVO，但因為有 branch，因此編譯器會嘗試去做隱式移動，然而一樣由於無法利用 rvalue reference 去做連結，因此無法套用移動，導致去呼叫了 copy。

## 在你可能需要直接回傳的 non-trivial 參數上，不應該用 const 修飾
    
原因跟前面都一樣，因為會破壞隱式移動，看這個例子：

```cpp
#include <iostream>

struct S
{
  S() { puts("S()"); }
  S(const S &) { puts("S(const S &)");}
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S &operator=(const S &) { puts("operator=(const S &)"); return *this; }
  S &operator=(S &&) noexcept { puts("operator=(S &&)"); return *this; }
};

S make_value(const S s)    // return statement make this bad use of `const`
{
  return s;    // because we return it, const is bad in function definition!
}
	
int main(int argc, const char *[])
{
  S s = make_value(S{});
}
```

輸出：
```
S()
S(const S &)
~S()
~S()
```

當然我們可能會使用 const reference，那就沒關係，但這邊討論的是一般的 pass by value 的狀況。

額外閱讀：[What is a non-trivial constructor in C++?](https://stackoverflow.com/questions/3899223/what-is-a-non-trivial-constructor-in-c) 

## Data member 不應該用 const 修飾
    
這點就比較有趣了，如果你的 data member 有 `const` 修飾，那這不只會破壞掉隱式移動，也有可能會破壞掉 assignment 的語意，看看這個例子：

```cpp
 #include <iostream>
struct S
{
  S() { puts("S()"); }
  S(const S &) { puts("S(const S &)");}
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S &operator=(const S &) { puts("operator=(const S &)"); return *this; }
  S &operator=(S &&) noexcept { puts("operator=(S &&)"); return *this; }
};
struct Data {
  const S s;
};

int main(int argc, const char *[])
{
  Data d;
  Data d2 = std::move(d);    // the member `s` will be copied
}
```

輸出：
```
S()
S(const S &)
~S()
~S()
```

上例中你可以看見 s 使用的並不是 move 而是 copy，這跟我們大部分時候預期的不太一樣，除非你有特殊用途。

這在我們要使用 STL 容器時會有些問題：

```cpp
#include <iostream>
#include <vector>

struct S
{
  S() { puts("S()"); }
  S(const S &) { puts("S(const S &)");}
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S &operator=(const S &) { puts("operator=(const S &)"); return *this; }
  S &operator=(S &&) noexcept { puts("operator=(S &&)"); return *this; }
};

struct Data {
  const S s;
};

int main(int argc, const char *[])
{
  std::vector<Data> data;
  data.emplace_back();
  data.emplace_back();
  data.emplace_back();
}
```
輸出：
```
S()
S()
S(const S &)
~S()
S()
S(const S &)
S(const S &)
~S()
~S()
~S()
~S()
~S()
```
因為我們把隱式移動給破壞掉了，導致在 resize 容器時會需要整個複製，如果 `s` 沒有加上 const，輸出會是：
```
S()
S()
S(S&&)
~S()
S()
S(S&&)
~S()
S(S&&)
~S()
~S()
~S()
~S()
```