<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++教學系列 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50><br>Class & 物件導向</center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 前言

物件導向有三個很重要的特性：資料抽象化、繼承與動態連結。資料抽象化上章有提到，就是將介面與實作分開；繼承的話則可以幫助我們建立相似模型之間的關係；動態連結則可以讓我們使用某些型別的物件時不用在意其內部細節。

# 繼承(inheritance)

使用繼承的 Class 會構成一個階層架構(hierachy)，通常這個架構會有一個基類(base class)，其他類別再繼承自此基類，這些其他類別稱為衍生類(derived class)。基類會定義大家都需要的共通成員，而衍生類則會額外定義專屬於自己的成員。

舉個例子，今天要實作船、汽車、飛機，這時候可能就會有個基類叫做「交通工具」，並且內部會有大家都需要的共通成員，如座位、衛星定位等等，而這三個衍生類內部則會再自定義自己需要的成員，如汽車需要輪子，船需要螺旋槳，飛機需要機翼等等。

利用繼承可以避免持續撰寫重複的行為，但這不是主要的目的，濫用繼承會增加 code 之間的相依性，導致程式維護上變得更加困難，因此繼承最主要的使用時機是當兩個 class 擁有 <span class = "yellow">is-a</span> 的關係時，以上例來說，汽車「是一種」交通工具，船也「是一種」交通工具，飛機也是。

但不是只有 is-a 的關係會用繼承，在某些時候，如 mixins 或 policy-based 設計，甚至是某些 has-a 關係也會用到繼承，也就是說還是要看你的實作細節來決定，但如果有用到多態繼承(polymorphic inheritance)，也就是有 `virtual` function 的，那就應該永遠都是 is-a 的關係。

## Base Class and derived class

要定義一個基類很簡單，基本上跟上章的 Class 定義方法差不多，差別就是基類可能會使用到 `protected` 關鍵字：

```cpp
#include <iostream>

class Transportation {
public:
  int seat = 4;

protected:
  int position = 0;

private:
  int cnt = 5;
};
```

`Transportation` 是一個基類，其有三個成員，且每一個成員的訪問許可權都不一樣。

而繼承的寫法如下：

> class Name : access-specifier(opt) Base_Class_Name
> access specifier : public、protected、private

access specifier 如果寫 public，那 Base Class 內的成員訪問許可權不會變，如果寫 protected，則原先是 public 的會變為 protected，填 private 則原先是 public 與 protected 的都會變為 private。

另外，如果沒有寫 access specifier，則會有預設的訪問許可權，若使用的關鍵字是 `struct`，則預設的形式為 public 繼承，如果是 `class`，則為 private 繼承。

接下來我們要寫一個 `Car` 繼承 `Transportation`：

```cpp
class Car : public Transportation {
public:
  int wheal = 4;

  int get_pos()
  {
    return position;
  }

  int get_cnt()
  {
    // return cnt;    // error.
  }
};
```

繼承時基類會是衍生類的 subobject，因此衍生類可以直接使用基類的成員，但<span class = "yellow">無法直接使用基類的 private 成員</span>，需要基類提供 api 才可以使用基類的 private 成員。

整段 code 長這樣：

```cpp
#include <iostream>

class Transportation {
public:
  int seat = 4;

protected:
  int position = 0;

private:
  int cnt = 5;
};

class Car : public Transportation {
public:
  int wheal = 4;

  int get_pos()
  {
    return position;
  }

  int get_cnt()
  {
    // return cnt;    // error.
  }
};

int main()
{
  Car c;
  std::cout << c.seat << '\n';
  // std::cout << c.position << '\n';    // error
  std::cout << c.get_pos() << '\n';    // okay
}
```

可以看見外部無法直接使用 `protected` 與 `private` 的成員，而衍生類內部無法直接使用基類的 `private` 成員。

## 建構子(Constructor)

雖然衍生類內含有基類的成員，但衍生類不應該直接初始化那些成員(除非你有特殊設計需求)，需要透過基類的建構子來初始化他們，因此建構時會先呼叫基類的建構子，再呼叫衍生類的建構子：
```cpp
#include <iostream>

class B {
public:
  B() { std::cout << "Base Class Constructor\n"; }
};

class D : public B {
public:
  D() { std::cout << "Derived Class Constructor\n"; }
};

int main()
{
  D d;
}
```

上例會先呼叫 `B()` 再呼叫 `D()`。

我們通常會透過委派建構子來初始化基類的成員，直接在初始化清單內呼叫基類的建構子即可：

```cpp
#include <iostream>

class B {
public:
  int i;
  B() { std::cout << "Base Class Constructor\n"; }
  B(int i)
      : i(i) {}
};

class D : public B {
public:
  int j;
  D() { std::cout << "Derived Class Constructor\n"; }
  D(int i, int j)
      : B(i), j(j) {}
};

int main()
{
  D d(1, 2);
  std::cout << d.i << " " << d.j;    // 1 2
}
```

這樣比較好的原因是因為每個 Class 都會有自己的 interface，我們應該透過這些 interface 來跟 Class 互動，即使它是你的基類也是。另外一點就是 Class 有自己的 scope，在繼承底下衍生類的 scope 為巢狀的範疇(nested scope)，如果你使用衍生類的建構子來初始化基類的成員，可能讓 code 變得較為複雜。