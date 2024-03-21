---
title: C++ 教學系列 ── Class & 物件導向
date: 2022-05-19
abstract: 物件導向有三個很重要的特性：資料抽象化、繼承與動態連結。資料抽象化上章有提到，就是將介面與實作分開；繼承的話則可以幫助我們建立相似模型之間的關係；動態連結則可以讓我們使用某些型態的物件時不用在意其內部細節
tags: C++ Miner_tutorial
categories:
- C++ Miner
---
<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 ── Class & 物件導向 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>  

點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>  

# 前言  

物件導向有三個很重要的特性：資料抽象化、繼承與動態連結。資料抽象化上章有提到，就是將介面與實作分開；繼承的話則可以幫助我們建立相似模型之間的關係；動態連結則可以讓我們使用某些型態的物件時不用在意其內部細節。  

# 繼承(inheritance)  

使用繼承的 Class 會構成一個階層架構(hierachy)，通常這個架構會有一個基類(base class)，其他類別再繼承自此基類，這些其他類別稱為衍生類(derived class)。基類會定義大家都需要的共通成員，而衍生類則會額外定義專屬於自己的成員。  

舉個例子，今天要實作船、汽車、飛機，這時候可能就會有個基類叫做「交通工具」，並且內部會有大家都需要的共通成員，如座位、衛星定位等等，而這三個衍生類內部則會再自定義自己需要的成員，如汽車需要輪子，船需要螺旋槳，飛機需要機翼等等。  

利用繼承可以避免持續撰寫重複的行為，但這不是主要的目的，濫用繼承會增加 code 之間的相依性，導致程式維護上變得更加困難，因此繼承最主要的使用時機是當兩個 class 擁有 <span class = "yellow">is-a</span> 的關係時，以上例來說，汽車「是一種」交通工具，船也「是一種」交通工具，飛機也是。  

但不是只有 is-a 的關係會用繼承，在某些時候，如 mixins 或 policy-based 設計，甚至是某些 has-a 關係也會用到繼承，也就是說還是要看你的實作細節來決定，但如果有用到多態繼承(polymorphic inheritance)，也就是有 `virtual` function 的，那就應該永遠都是 is-a 的關係。  

# Base Class and derived class  

要定義一個基類很簡單，基本上跟上章的 Class 定義方法差不多，差別就是基類可能會使用到 `protected` 關鍵字，表示這個成員可讓衍生類使用：  

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

如果沒有寫 access specifier，則會有預設的訪問許可權，若使用的關鍵字是 `struct`，則預設的形式為 public 繼承，如果是 `class`，則為 private 繼承。  

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

繼承時基類會是衍生類的 subobject，因此衍生類可以直接使用基類的成員，但<span class = "yellow">無法直接使用基類的 private 成員</span>，需要基類提供 api 才可以使用基類的 private 成員  

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

可以看見外部無法直接使用 `protected` 與 `private` 的成員，而衍生類內部無法直接使用基類的 `private` 成員  

另外，access specifier 如果寫 public，那基類內的成員訪問許可權不會變，如果寫 protected，則原先是 public 的會變為 protected，寫 private 的話 public 與 protected 都會變為 private，因此第二層繼承的 class 將無法使用基類的 member：  

<center><img src="https://hackmd.io/_uploads/H1-XJmxLT.png"></center><br>  

```cpp
class B {  
public:  
  int i1 = 1;  

protected:  
  int i2 = 2;  

private:  
  int i3 = 1;  
};  

class pub_D : public B {  
public:  
  void print_member()  
  {  
    std::cout << i1 << '\n';    // ok  
    std::cout << i2 << '\n';    // ok  
    std::cout << i3 << '\n';    // error: 'int B::i3' is private within this context  
  }  
};  

class pro_D : protected B {  
public:  
  void print_member()  
  {  
    std::cout << i1 << '\n';    // ok  
    std::cout << i2 << '\n';    // ok  
    std::cout << i3 << '\n';    // error: 'int B::i3' is private within this context  
  }  
};  

class pri_D : private B {  
public:  
  void print_member()  
  {  
    std::cout << i1 << '\n';    // ok  
    std::cout << i2 << '\n';    // ok  
    std::cout << i3 << '\n';    // error: 'int B::i3' is private within this context  
  }  
};  

class pri_D2 : private pri_D {  
public:  
  void print_member()  
  {  
    std::cout << i1 << '\n';    // error: 'int B::i1' is private within this context  
    std::cout << i2 << '\n';    // error: 'int B::i2' is private within this context  
    std::cout << i3 << '\n';    // error: 'int B::i3' is private within this context  
  }  
};  

void print_member()  
{  
  pub_D pubD;  
  pro_D proD;  
  pri_D priD;  
  pri_D2 priD2;  

  std::cout << pubD.i1 << '\n'; // ok  
  std::cout << pubD.i2 << '\n'; // error: 'int B::i2' is protected within this context  

  std::cout << proD.i1 << '\n'; // error: 'int B::i1' is inaccessible within this context  
  std::cout << proD.i2 << '\n'; // error: 'int B::i2' is protected within this context  

  std::cout << priD.i1 << '\n'; // error: 'int B::i1' is inaccessible within this context  
  std::cout << priD.i2 << '\n'; // error: 'int B::i2' is protected with  
int main()  
{  
}  
```

# Operator of Derived Class  

如果衍生類有 operator overloading 的需求，那他必須自己寫出來，無法調用基類的 operator overloading：  

```cpp
#include <iostream>  

class B {  
public:  
  int i;  
  B& operator=(int input) {  
    i = input;  
    return *this;  
  }  
};  

class D : public B {  
};  

int main() {  
    D d;  
    d = 5; // error  
}  
```

正確的作法是在衍生類內明確的呼叫基類的 `operator=`：  

```cpp
#include <iostream>  

class B {  
public:  
  int i;  
  B& operator=(int input) {  
    i = input;  
    return *this;  
  }  
};  

class D : public B {  
public:  
  D& operator=(int input) {  
    B::operator=(input);  
    return *this;  
  }  
};  

int main() {  
  D d;  
  d = 5; // ok  
  std::cout << d.i; // 5  
}  
```

同理，copy 與 move constructor 也是，而 copy 與 move assignment operator 也是：  

```cpp
#include <iostream>  

class B {  
public:  
  int i;  
  B& operator=(const B& other) {  
    i = other.i;  
    return *this;  
  }  
};  

class D : public B {  
public:  
  D& operator=(D& other) {  
    B::operator=(other);  
    return *this;  
  }  
};  

int main() {  
    D d1, d2;  
    d1.i = 5;  
    d2 = d1; // ok  
    std::cout << d2.i; // 5  
}  
```

# friend  

friend 的關係並沒有辦法被繼承，基類的 friend 並不會有衍生類的存取權，衍生類的 friend 也不會有基類的存取權：  

```cpp
class B {  
  int i;  
  friend void BaseFriend(B b);  

public:  
  int get_i() {  
    return i;  
  }  
};  

class D {  
  int i2;  
  friend void DerivedFriend(D d);  
public:  
  int get_i2() {  
    return i2;  
  }  
};  

void BaseFriend(B b)   
{  
  b.i = 5;  
  b.i2 = 100; // error  
}  

void DerivedFriend(D d)  
{  
  d.i2 = 10;   
  d.i = 100; // error  
}  
```

# 建構子(Constructor)  

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

# Hiding Base Class Member  

有時候我們會有禁用基類的 member 需求，然而在 C++ 中我們無法將基類的 member function 刪掉，但是我們能夠透過更改存取權將「單一個」 member 「隱藏」起來，我們有幾種方式可以更改存取權，首先是使用 `using` 關鍵字：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B{  
private:  
  using B::i1;  
};  

int main() {  
  D d;  
  std::cout << d.i1 << '\n'; // error  
  std::cout << d.i2 << '\n'; // ok  
}  
```

member function 同理：  

```cpp
#include <iostream>  

class B {  
public:  
  void fn() { std::cout << "B::fn()\n"; }  
};  

class D : public B {  
private:  
  using B::fn;  
};  

int main()  
{  
  D d;  
  d.fn();    // error: ‘void B::fn()’ is inaccessible within this context  
}  
```

但要注意，如果有 function overloading，那麼使用 `using` 更改存取權時所有同名的 function 都會被一併改到：  

```cpp
#include <iostream>  

class B {  
public:  
  void fn() { std::cout << "B::fn()\n"; }  
  void fn(int) { std::cout << "B::fn(int)\n"; }  
};  

class D : public B {  
private:  
  using B::fn;    // make all B::fn private  
};  

int main()  
{  
  D d;  
  d.fn();    // error: ‘void B::fn()’ is inaccessible within this context  
  d.fn(1);    // error: ‘void B::fn(int)’ is inaccessible within this context  
}  
```

可以看見兩個 `fn` 都被設為 private 了，我們也可以反過來將 `protected` 的成員設為 `public`：  

```cpp
#include <iostream>  

class B {  
protected:  
  void fn() { std::cout << "B::fn()\n"; }  
  void fn(int) { std::cout << "B::fn(int)\n"; }  
};  

class D : public B {  
public:  
  using B::fn;    // make all B::fn public  
};  

int main()  
{  
  D d;  
  d.fn();    // ok  
  d.fn(1);    // ok  
}  
```

如果你想要的不是更改存取權，而是明確表示不能使用該成員，那可以使用 `delete` 關鍵字：  

```cpp
#include <iostream>  

class B {  
public:  
  void fn() { std::cout << "B::fn()\n"; }  
};  

class D : public B {  
public:  
  void fn() = delete;    // make D::fn2() inaccessible  
};  

int main()  
{  
  D d;  
  d.fn();    // error: use of deleted function ‘void D::fn()’  
}  
```

而如果你是想要寫一個同名的 member function，可以透過 function overloading 來將基類的 member function 隱藏起來：  

```cpp
#include <iostream>  

class B {  
public:  
  void fn() { std::cout << "B::fn()\n"; }  
};  

class D : public B {  
public:  
  void fn() { std::cout << "D::fn()\n"; }  
};  

int main()  
{  
  D d;  
  d.fn();    // calls D::fn()  
}  
```

要注意的是利用這些方法來隱藏成員時，我們仍可以透過轉型來存取被隱藏的成員：  

```cpp
#include <iostream>  

class B {  
public:  
  void fn() { std::cout << "B::fn()\n"; }  
  void fn2() { std::cout << "B::fn2()\n"; }  
  void fn3() { std::cout << "B::fn3()\n"; }  
};  

class D : public B {  
private:  
  using B::fn;    // make all B::fn private  
public:  
  void fn2() = delete;    // make D::fn2() inaccessible  
  void fn3() { std::cout << "D::fn3()\n"; }  
};  

int main()  
{  
  D d;  
  static_cast<B&>(d).fn();    // ok  
  static_cast<B&>(d).fn2();    // ok  
  static_cast<B&>(d).fn3();    // calls B::fn3()  
}  
```

# Derived-to-Based Conversion  

上例中我們透過 `static_cast` 將衍生類轉型為基類，藉此呼叫基類的 member function，這邊我們要講一下衍生類與基類的轉型  

雖然標準並沒有講述衍生類實例在記憶體中的布局該長怎樣，但通常衍生類的實例由三個部份組成：virtual table pointer、基類成員與衍生類成員  

撇除 virtual table pointer 不談，假設一個衍生類 `D` 的定義如下：  

```cpp
class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  
```

則其 memory layout 通常會如下圖：  

<center>  

![image](https://hackmd.io/_uploads/Hk1OE4e8T.png)  
(順序不一定會一樣，要看電腦的架構與編譯器，但一定會有個排列的規則)      

</center>  
我們可以使用 `reinterpret_cast` 來做簡單的驗證：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  

int main()  
{  
  D *ptr1 = new D();  
  std::cout << reinterpret_cast<int *>(ptr1) << ' ' << *reinterpret_cast<int *>(ptr1) << '\n';    // 1  
  std::cout << (reinterpret_cast<int *>(ptr1) + 1) << ' ' << *(reinterpret_cast<int *>(ptr1) + 1) << '\n';    // 2  
  std::cout << (reinterpret_cast<int *>(ptr1) + 2) << ' ' << *(reinterpret_cast<int *>(ptr1) + 2) << '\n';    // 3  
  std::cout << (reinterpret_cast<int *>(ptr1) + 3) << ' ' << *(reinterpret_cast<int *>(ptr1) + 3) << '\n';    // 4  
}  
```

對於這個 memory layout，有興趣的話可以到 [malloc、new 與 POD Type](https://hackmd.io/@Mes/Miner_malloc_new_pod) 看更多  

而我們之前在 [Object、Expression、Statement](https://hackmd.io/@Mes/MinerT_Object_Expression_Statement#Type) 內，有提到 Type 的本質是對物件、reference 和函式的一種「解讀方式」，你可以理解為它是一種讓你知道「如何讀取一塊記憶體」的標籤，例如 `0x41` 這段 binary，如果我們使用 `char` 來解讀，那他的表現方式就是英文字母 `A`，但如果我們用 `int` 來解讀，他的表現方式則是 `65`  

而在講 Class 時我們也有提到，Class 的本質是讓我們自訂義一個型態，也就是能夠自己定義一個物件的「解讀方式」，且在 C++ 中，<span class = "yellow">衍生類物件含有其對應基類的子物件</span>  

因此我們可以使用基類型態來解讀一個衍生類型態的物件，當我們用 `B` 來解讀 `D` 的實例時，他自然就會用 `B` 的解讀方式來讀，也因此前面才能夠找到被隱藏的函式  

在做這件事情時，我們通常會使用指標或 reference 來幫助我們操作，利用其將一個基類連結到一個衍生物件的基類部份，由於型態不一樣，因此就有轉型，這個轉型被稱為 derived-to-base conversion  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  

int main()  
{  
  D d;  
  B *ptrB = &d;    // ok: derived-to-base conversion  
  B &refB = d;    // ok: derived-to-base conversion  
}  
```

要注意由於型態不同，因此對物件的解讀方式也就不同，由於 `B` 內並沒有 `i3` 和 `i4` 這兩個成員，因此以 `B` 這個型態來解讀 `d` 這個物件時，就會無法使用 `i3` 和 `i4` 這兩個成員：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  

int main()  
{  
  D d;  
  B *ptrB = &d;    // ok: derived-to-base conversion  
  B &refB = d;    // ok: derived-to-base conversion  

  std::cout << ptrB->i1 << '\n';    // ok  
  std::cout << refB.i1 << '\n';    // ok  

  std::cout << ptrB->i3 << '\n';    // error: B has no member named i3  
  std::cout << refB.i3 << '\n';    // error: B has no member named i3  
}  
```

而很直觀的，這兩個型態的實例大小也不一樣：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  

int main()  
{  
  D d;  
  D &refD = d;  
  B &refB = d;    // ok: derived-to-base conversion  

  std::cout << sizeof(refD) << '\n'    // 16  
            << sizeof(refB) << '\n';    // 8  
}  
```

這都是因為物件的「解讀方式」不同  

## Object Slicing  

讀到這裡你應該知道基類與衍生類是不同的型態，但有轉型可以幫助它們做轉換  

也因為有這個特性，當我們使用一個衍生類物件賦值給基類物件時，會發生一個問題稱為物件切片(Object Slicing)  

這個問題是這樣的，由於衍生類可以轉行為基類，自然就可以賦值給基類物件，但是衍生類有額外的成員，如上面的 `i3` 與 `i4`，兩個型態的實例大小也不一樣  

因此，使用衍生類物件賦值給基類物件時，<span class = "yellow">衍生類自身的成員會被捨棄掉</span>，導致資料遺失，類似於將 `double` 賦值給 `float`，因為 `double` 比 `float` 大，因此精度會有所損失：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1, i2;  

  B() : i1(), i2() {}  
  B(int i1, int i2) : i1(i1), i2(i2) {}  
};  

class D : public B {  
public:  
  int i3, i4;  

  D() = default;  
  D(int i3, int i4) : B(1, 2), i3(i3), i4(i4) {}  
};  

int main()  
{  
  D d(3, 4);  
  B b = d;    // Object Slicing here, i3 and i4 was deprecated  

  std::cout << b.i1 << '\n';    // 1  
  std::cout << b.i3 << '\n';    // error  
}  
```

> 題外話：這種從大的 type 轉型到小的 type 的轉換被稱為 narrowing conversion  

# Override Base Class Method  

上面提到了我們可以透過 function overloading 來隱藏基類的 member function，從而讓透過衍生類呼叫時使用的會是衍生類自己定義的版本；在這樣做的情況下，我們要的效果通常會是想以一般化的方式來操作實例，無論該實例是基類或衍生類實例  

舉個例子，在定義交通工具這個基類的時候我們可能會認為交通工具都會需要能夠「前進」，因此我們會在基類宣告一個「前進」的 member function，然而對於汽車、飛機與船，它們「前進」的運作原理可能不一樣，所以它們會需要自己定義這個 member function 的內容：  

```cpp
#include <iostream>  

class Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Base foward\n";  
  }  
};  

class Car : public Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Car foward\n";  
  }  
};  

class Boat : public Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Boat foward\n";  
  }  
};  

class Airplane : public Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Airplane foward\n";  
  }  
};  
```

但現在問題來了，我們在設計一個函式界面時可能希望參數只要是個「交通工具」就可以傳進來，例如展示會上我們要展示各式各樣交通工具的運作(我想不到其他例子ㄌXD)：  

```cpp
#include <iostream>  

class Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Base foward\n";  
  }  
};  

class Car : public Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Car foward\n";  
  }  
};  

class Boat : public Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Boat foward\n";  
  }  
};  

class Airplane : public Transportation {  
public:  
  void foward()  
  {  
    std::cout << "Airplane foward\n";  
  }  
};  

void show_foward(Transportation *t)  
{  
  t->foward();    // trying to call corresponding foward function  
}  

int main()  
{  
  Car car;  
  Boat boat;  
  Airplane airplane;  

  show_foward(&car);    // Base foward  
  show_foward(&boat);    // Base foward  
  show_foward(&airplane);    // Base foward  

  return 0;  
}  
```

與前面轉型可以呼叫被隱藏的函式同理，由於型態的關係，呼叫到的會是基類的 `foward` 函式  

因此我們這裡需要一個機制，讓我們在執行期呼叫時可以透過傳入的物件的型態來決定要呼叫哪個 function  

## static binding 與 dynamic binding  

之前有提到 function 有所謂的「宣告」與「定義」，宣告可以有很多個，但定義只能有一個，最終宣告會連結到其對應的單一個的定義，在呼叫 function 的時候會利用 function 的 signature 尋找對應的定義，進而執行 function 的內容  

「呼叫 function 的時候會利用 function 的 signature 尋找對應的定義」這件事我們稱之為 binding，會發生在兩個時機點：「執行期」與「編譯期」  

編譯期的 binding 被稱為 static binding，因為比較早連結所以又稱 early binding，編譯器會在 compile-time 時就把函式呼叫與函式定義連結在一起  

而執行期的 binding 被稱為 dynamic binding，又稱 late binding，函式呼叫與其對應的定義會一直等到值 run-time 才會發生  

由於 static binding 在編譯期完成，因此「利用 function 的 signature 尋找對應的定義」這件是在編譯期就已經完成了，換句話說就是呼叫函式需要的資訊都已經先提前值到了，因此執行起來的速度會比較快  

而 dynamic binding 由於在執行期才完成，因此在執行的時候才會去找對應的定義，但也因為這樣所以寫法可以更有彈性  

在 C++ 中，一般的 function 都是 static binding，而若要使用 dynamic binding，則要利用 `virtual` 關鍵字來實作  

我們的需求是在執行期呼叫時可以透過傳入的物件的型態來決定要呼叫哪個 function，因此要使用的是跟以前不一樣的 dynamic binding 的方式，所以接下來就要開始講 virtual function 了  

## virtual function  

在基類中，我們可以在「預期會被衍生類覆寫的函式」定義為 `virtual`，這種成員函式被稱為虛擬函式(virtual function)，擁有虛擬函式的 class 被稱為 polymorphic class  

> [n4659(13.3-1)](https://timsong-cpp.github.io/cppwp/n4659/class.virtual#def:class,polymorphic)：[ Note: Virtual functions support dynamic binding and object-oriented programming.  — end note ] A class that declares or inherits a virtual function is called a polymorphic class.  

當我們透過指標或 reference 來呼叫虛擬函式時，這個呼叫會是使用 dynamic binding 的方式在尋找函式定義，根據物件的型態，可能會執行到基類的成員函式，也有可能式其中一個衍生類中覆寫的版本：  

```cpp
#include <iostream>  

class B {  
public:  
  virtual void f() { std::cout << "B::f()" << std::endl; }  
};  

class D : public B {  
public:  
  void f() { std::cout << "D::f()" << std::endl; }  
};  

int main()  
{  
  D d;  

  B *bptr = &d;  
  bptr->f();    // calls D::f()  

  B &bref = d;  
  bref.f();    // calls D::f()  
}  
```

在這裡，我們在 `B` 內的 `void f()` 前方寫上了 `virtual`，因此 `f` 是一個虛擬函式，而在 `D` 中，我們覆寫了 `f`，此時 `D` 中的 `f` 也會是一個虛擬函式，就算前方沒寫 `virtual` 關鍵字  

任何非 static 的成員函式，除了建構子以外，都可以是虛擬的，在 [Bjarne Stroustrup's C++ Style and Technique FAQ](https://www.stroustrup.com/bs_faq2.html#virtual-ctor) 中有解釋：  

> A virtual call is a mechanism to get work done given partial information. In particular, "virtual" allows us to call a function knowing only an interfaces and not the exact type of the object. To create an object you need complete information. In particular, you need to know the exact type of what you want to create. Consequently, a "call to a constructor" cannot be virtual.  

翻成中文的意思是：  

> 虛擬函式的呼叫是一種「在給定部分資訊的情況下完成工作」的機制，因為「virtual」允許我們呼叫一個只知道介面，而不知道物件確切型態的函式。  
>   
> 要實例化物件，你就得知道其完整的資訊，尤其是你需要知道要實例化的物件的確切類型。 因此，「對建構子的呼叫」不能是 virtual 的  

虛擬函式不一定要被覆寫，如果衍生類沒有覆寫它，那麼這個虛擬函式就跟其他的成員函式一樣，繼承基類中的定義  

> [n4659(footnote-111)](https://timsong-cpp.github.io/cppwp/n4659/class.virtual#footnote-111)：A function with the same name but a different parameter list (Clause [over]) as a virtual function is not necessarily virtual and does not override. The use of the virtual specifier in the declaration of an overriding function is legal but redundant (has empty semantics). Access control is not considered in determining overriding.  

## override  

在我們要覆寫基類的虛擬函式時，最好可以顯式的將 `override` 寫上去，這會避免我們沒有覆寫到基類的虛擬函式，考慮以下狀況：  

```cpp
#include <iostream>  

class B {  
public:  
  virtual void f() const { std::cout << "B::f()\n"; }  
  void callf() { f(); }  
};  

class D : public B {  
public:  
  void f() { std::cout << "D::f()\n"; }  
};  

int main() {  
    B *b = new D();  
    b->callf();    // B::f()  
}  
```

在基類中有一個虛擬函式 `f` 與一個會去呼叫 `f` 的函式 `callf`，在這麼寫的情況下，我們通常希望 `callf` 會去呼叫到衍生類自己的 `f`，因此在 `D` 裡面，我們嘗試去覆寫了 `f`  

然而在上面輸出的結果顯示，我們呼叫到的 `f` 是基類的 `f`，這是因為基類中的 `f` 擁有 `const` 標示符，但 `D` 內的 `f` 忘記寫上去了  

為了避免這類意外，我們可以將 `override` 加上去，加在 function body 的前方就可以了：  

```cpp
#include <iostream>  

class B {  
public:  
  virtual void f() const { std::cout << "B::f()\n"; }  
  void callf() { f(); }  
};  

class D : public B {  
public:  
  void f() override { std::cout << "D::f()\n"; }    // error: 'void D::f()' marked 'override', but does not override  
};  

int main() {  
    B *b = new D();  
    b->callf();  
}  
```

可以看見在第 11 行處跳了 error，成功的幫助我們找出了這個意外，此時我們將 `const` 加上去就可以完成覆寫了：  

```cpp
#include <iostream>  

class B {  
public:  
  virtual void f() const { std::cout << "B::f()\n"; }  
  void callf() { f(); }  
};  

class D : public B {  
public:  
  void f() const override { std::cout << "D::f()\n"; }    // error: 'void D::f()' marked 'override', but does not override  
};  

int main() {  
    B *b = new D();  
    b->callf();    // D::f()  
}  
```

> 額外閱讀： [What are the differences between overriding virtual functions and hiding non-virtual functions?](https://stackoverflow.com/questions/19736281/what-are-the-differences-between-overriding-virtual-functions-and-hiding-non-vir)  

## final  

在我們不希望、或不確定一個 Class 會不會被其他人繼承的時候，我們可以利用 `final` 這個 keyword 來防止繼承：  

```cpp
class B final{};  
class D : public B {}; // error  
```

如果只有針對特定的 virtual function 不想被 override 的話，可以加在那個 function 的後面：  

```cpp
class B {  
public:  
  virtual void fn() {};  
};  

class D1 : public B {  
public:  
  void fn() final {}  
};  

class D2 : public D1 {  
  void fn() {} // error: 'virtual void D2::fn()' overriding final function  
};  
```

# Static Type and Dynamic Type  

在使用有繼承關係的型態時，我們要特別注意變數，或說其 expression 的靜態型態(static type) 與動態型態(dynamic type)  

所謂的靜態型態指的是在編譯時期就已經知道的型態，在 C++ 中這會是一個變數所宣告的型態，或是編譯期運算式所回傳的型態  

而動態型態則是指「記憶體」中那個變數或運算式所代表的物件的型態，動態型態可要到執行期才會確定  

回來看交通工具的例子：  

```cpp
#include <iostream>  

class Transportation {  
public:  
  virtual void foward()  
  {  
    std::cout << "Base foward\n";  
  }  
};  

class Car : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Car foward\n";  
  }  
};  

class Boat : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Boat foward\n";  
  }  
};  

class Airplane : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Airplane foward\n";  
  }  
};  

void show_foward(Transportation *t)  
{  
  t->foward();    // trying to call corresponding foward function  
}  

int main()  
{  
  Car *car = new Car();  
  Boat *boat = new Boat();  
  Airplane *airplane = new Airplane();  

  show_foward(car);    // Car foward  
  show_foward(boat);    // Boat foward  
  show_foward(airplane);    // Airplane foward  

  return 0;  
}  
```

我們專注看 `show_foward` 這個函式，在執行這個函式前，對於 `t`，我們能確定的是 `t` 的靜態型態為 `Transportation*`，但由於還沒到執行期，所以我們並不知道 `t` 的動態型態為何  

而當我們執行到第 46 行的 `show_foward(car);` 時，我們才能確認在這個函式呼叫中，`t` 的動態型態為 `Car*`，因此才會去呼叫 `Car` 內的 `foward` 函式  

因此對於一個指標或 reference 的變數/expression 而言，他們的靜態型態與動態型態不一定會一樣；但相反的，一個不是指標或 reference 的變數/expression 就沒有這種問題，他們的靜態型態與動態型態就永遠都一樣  

## Dynamic Polymorphism  

首先先看看 wiki：  

> 在程式語言和類型論中，多型（英語：polymorphism）指為不同資料類型的實體提供統一的介面，或使用一個單一的符號來表示多個不同的類型。  

而物件導向背後的核心概念是多型(Polymorphism)，由上方的敘述我們可以得知多型的核心概念就是「統一介面」，已經讀過前面的你，應該可以理解這四個字的含意  

簡單來說就是同一個 function，同一個靜態型態的參數，但依照傳入的參數，可以有不同的行為；這樣的好處是易於擴充，同時可以降低程式碼的耦合性  

以上面交通工具的例子來說，所有的交通工具都應該要有「前進」這個功能，因此當我們在一些應用到交通工具的場景時，就不用去煩惱這個交通工具到底是車，是船，還是飛機了，因為我們「確定」只要是交通工具，它就有實作「前進」這個功能  

我們可以小改一下前面的例子來體會一下多型的好處，假設我們今天有不確定數量的交通工具要進行展示，一樣是展示往前這個功能，此時我們可以結合 `std::vector` 與多型來實作：  

```cpp
#include <iostream>  
#include <vector>  

class Transportation {  
public:  
  virtual void foward()  
  {  
    std::cout << "Base foward\n";  
  }  
};  

class Car : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Car foward\n";  
  }  
};  

class Boat : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Boat foward\n";  
  }  
};  

class Airplane : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Airplane foward\n";  
  }  
};  

void show_foward(std::vector<Transportation*> &vec)  
{  
  for(Transportation* t : vec)  
    t->foward();  
}  

int main()  
{  
  std::vector<Transportation*> vec;  

  vec.push_back(new Car());  
  vec.push_back(new Boat());  
  vec.push_back(new Airplane());  
  vec.push_back(new Airplane());  
  vec.push_back(new Boat());  
  vec.push_back(new Car());  

  show_foward(vec);  

  return 0;  
}  
```

在 `show_foward` 這個函式內我們遍歷了 `vec`，並且呼叫了 `vec` 中每一個元素的 `foward`，如此一來我們便可以在不知道實際上這個元素是車、船還是飛機的情況下完成我們的目的了  

另外，根據多型的實現方法，也就是根據函式的使用的是 static binding 還是 dynamic binding，可以將多型分為靜態多型與動態多型  

在 C++ 中，靜態多型的寫法比較麻煩，需要使用 CRTP，會有一些限制，但好處是比較快；而動態多型使用的就是我們這篇一直在提的 virtual function，好處是彈性較高，也比較好寫，但就比較慢了  

對 CRTP 有興趣的可以讀讀 TJSW 的文章：[潮．C++ | CRTP 和靜態多型](https://tjsw.medium.com/%E6%BD%AE-c-crtp-%E5%92%8C%E9%9D%9C%E6%85%8B%E5%A4%9A%E5%9E%8B-96a91b9e4db6)  

另外，如果你有仔細去閱讀多型的 wiki，你會發現 wiki 內寫了三種，分別是：特設多型、參數多型與子類型，我們這邊講的都是子類型的範疇，因為這與物件導向有直接的關係  

如果你還想理解更多，可以去看看林信良老師之前寫的文章：  

+ [多型的本質一](https://www.ithome.com.tw/voice/74892)  
+ [多型的本質二](https://www.ithome.com.tw/voice/75186)  
+ [多型的本質三](https://www.ithome.com.tw/voice/75352)  

# Pure Virtual Function  

如果我們仔細思考了一下上面交通工具的例子，你可能會想到一件事：Transportation 可以被實例化  

這是一件不太合理的事情，Transportation 的目的是定義一個交通工具「必須」要有的行為，它是一個抽象的「概念」，或我們可以說是一種「原則」  

因此它並不如我們上一章的 class，它不是「某一個東西的設計圖」，也因此，它不應該可以被實例化，不應該要有 Transportation 物件產生  

再更嚴謹的說，我們甚至不該定義「前進」這個函式，因為我們只能確定 Transportation 這個概念所衍生出來的產品必須能夠前進，但各個衍生類的「前進」卻大相逕庭，這種情況下，「前進」這個函式不會有一個預設的行為  

這種時候，我們就會需要使用純虛擬函式(pure virtual function)，純虛擬函式代表「一定會有這個功能，但這個功能並沒有預設的行為」  

與虛擬函式不同，純虛擬函式並不需要被定義(但可以被定義)，只需要在 function body 的部分寫上 `= 0` 就可以了，衍生類的部分則不會差太多  

而擁有純虛擬函式的 class 被稱為「抽象類別(abstract class)」，不能被實例化，一樣用交通工具的例子來看：  

```cpp
#include <iostream>  

class Transportation {  
public:  
  virtual void foward() = 0;  
};  

class Car : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Car foward\n";  
  }  
};  

class Boat : public Transportation {  
public:  
  void foward() override  
  {  
    std::cout << "Boat foward\n";  
  }  
};  

void show_foward(Transportation *t)  
{  
  t->foward();    // trying to call corresponding foward function  
}  

int main()  
{  
  Car *car = new Car();  
  Boat *boat = new Boat();  

  show_foward(car);    // Car foward  
  show_foward(boat);    // Boat foward  

  Transportation t;    // error: cannot declare variable 't' to be of abstract type 'Transportation'  

  return 0;  
}  
```

可以看見我們在 `Transportation` 內的 `foward` 的 function body 部分寫上了 `= 0`  

而在第 37 行我們嘗試去實例化一個 `Transportation` 的物件，因此得到了一個 error，告訴我們 `t` 是一個抽象類別，不能被實例化  

至於何時會有定義純虛擬函式的需求，可以參考 Effective C++ 的 Item 34，這邊不鼓勵盜版，推薦大家可以買書來看  

# Virtual Destructor  

當一個物件的 static type 與 dynamic type 不一樣的時候，也就是有在使用多型的時候，如果我們「需要對物件做 `delete`」，則基類的解構子必須要是 virtual 的，否則對此物件的 `delete` 行為被定義為 UB；但再提醒一下，建構子是沒有 virtual 的  

考慮一下下面的例子：  

```cpp
#include <iostream>  

class B {  
public:  
  int *ptr;  
  B() : ptr(new int) {}  
  ~B()  
  {  
    delete ptr;  
    std::cout << "B::~B()\n";   
  }  
};  

class D : public B {  
public:  
  int *ptr2;  
  D() : B(), ptr2(new int) {}  
  ~D()  
  {  
    delete ptr2;  
    std::cout << "D::~D()\n";  
  }  
};  

int main() {  
  B *d = new D();  
  delete d;    // only call B::~B()  
}  
```

在這個例子中，我們使用 Base Class 的來存一個 `D` 的實例，如同前面講的，這麼做的目的是利用多型來達到統一介面的效果  

而在 `B` 與 `D` 兩個 class 內，都擁有動態配置出來的記憶體空間，因此需要做對應的 delete 操作，上方的例子中，我們在解構子內釋放了這些記憶體空間  

然而，在第 27 行的 delete 中，我們發現它僅僅呼叫了 `B` 的解構子，並沒有呼叫 `D` 的解構子，你可以預見這會導致 memory leak  

依照前面的理解，這個行為也很正常，因為解構子並不是虛擬的，因此使用基類的指標或 reference 呼叫時並不會呼叫到衍生類內對應的解構子  

因此我們需要將解構子加上 virtual 關鍵字：  

```cpp
#include <iostream>  

class B {  
public:  
  int *ptr;  
  B() : ptr(new int) {}  
  virtual ~B()  
  {  
    delete ptr;  
    std::cout << "B::~B()\n";   
  }  
};  

class D : public B {  
public:  
  int *ptr2;  
  D() : B(), ptr2(new int) {}  
  ~D()  
  {  
    delete ptr2;  
    std::cout << "D::~D()\n";  
  }  
};  

int main() {  
  B *d = new D();  
  delete d;    // call D::~D(), then call B::~B()  
}  
```

可以看見呼叫完 `D` 的解構子後其也會再去呼叫 `B` 的解構子  

不過這只是個幫助理解的例子，有 `delete` 的情況下，並不代表你如果保證沒有 memory leak 就可以不用寫，解構子需要寫 virtual 這件事是<span class = "yellow">強制的</span>！<span class = "yellow">標準內很明確地講述了沒寫的話是 UB</span>：  

> [n4659(8.3.5-3)](https://timsong-cpp.github.io/cppwp/n4659/expr.delete#3)：In the first alternative (delete object), if the static type of the object to be deleted is different from its dynamic type, the static type shall be a base class of the dynamic type of the object to be deleted and the static type <span class = "yellow">shall have a virtual destructor or the behavior is undefined.</span> In the second alternative (delete array) if the dynamic type of the object to be deleted differs from its static type, the behavior is undefined.  

而只要我們要使用多型的特性，我們就會需要用基類的指標或 reference 來存取對應的衍生類物件，也因此高機率會有需要 delete 的狀況，哪怕 class 內並沒有使用動態記憶體配置成員  

<span class = "yellow">所以簡單來說，大部分的情況我們都會需要寫虛擬解構子</span>  

不過，如果你很清楚你不會需要 delete 衍生類的物件，那麼也可以不用寫  

根據 Herb Sutter 的文章 [Virtuality](http://www.gotw.ca/publications/mill18.htm)，我們可以整理成兩種情況：  

1. 你<span class = "yellow">有</span>利用 Base Class Pointer 去 delete 衍生類的需求  
    這種情境下，虛擬解構子便是必要的，而且解構子需要是 public 的  
2. 你<span class = "yellow">沒有</span>利用 Base Class Pointer 去 delete 衍生類的需求  
    這種情境下，你的解構子可以不用是虛擬的，但最好是 protected 的，以防意外呼叫到它  

# RTTI  

RTTI 的全名叫做 Run-Time Type Information，意思是執行階段類型辨識，一開始是為了讓程式在運行時能根據基類的指標或 reference 來獲得該指標或 reference 所指的物件的實際類型  

C\+\+ 透過兩個運算子來支援 RTTI：  

1. `typeid` operator，它會傳回其給定 expression 或 type 的型態  
2. `dynamic_cast` operator，它能夠安全的將 Base Class 的指標或 reference 轉換為 Derived Class 的指標或 reference  

套用到有虛擬函式的型態的指標或 reference 時，這些 operator 會使用對應物件的動態型態(dynamic type)，首先看 `typeid`：  

```cpp
#include <iostream>  

class B {  
public:  
    virtual ~B() = default;  
};  
class D : public B {};  
class D2 : public B {};  

void print_type(B *ptr)  
{  
  std::cout << typeid(*ptr).name() << '\n';  
}  

int main() {  
  D *d1 = new D();  
  D2 *d2 = new D2();  

  print_type(d1);    // 1D  
  print_type(d2);    // 2D2  
}  
```

但前面有提到，C++ 的動態型態需要依賴 `virtual` 來驅動，所以如果沒有虛擬函式，輸出就會不一樣了：  

```cpp
#include <iostream>  

class B {};  
class D : public B {};  
class D2 : public B {};  

void print_type(B *ptr)  
{  
  std::cout << typeid(*ptr).name() << '\n';  
}  

int main() {  
  D *d1 = new D();  
  D2 *d2 = new D2();  

  print_type(d1);    // 1B  
  print_type(d2);    // 1B  
}  
```

標準內也有提到這個事情，所以這不是實作上的差異，而是有標準定義的：  

> [n4659(8.2.8-3)](https://timsong-cpp.github.io/cppwp/n4659/expr.typeid#3)：When typeid is applied to an expression <span class = "yellow">other than</span> a glvalue of a <span class = "yellow">polymorphic class type</span>, the result refers to a std​::​type_­info object representing the <span class = "yellow">static type</span> of the expression. Lvalue-to-rvalue, array-to-pointer, and function-to-pointer conversions are not applied to the expression. If the expression is a prvalue, the temporary materialization conversion is applied. The expression is an unevaluated operand.  

> 註：擁有虛擬函式的 class 被稱為 polymorphic class  

而 `dynamic_cast` 運算子則是可以幫助安全的我們將基類指標或 reference 轉型為衍生類的指標或 reference，這種轉型被稱為 downcasting  

downcasting 是有危險性的，因為你基類的指標可以指向其任何衍生類的實例，但這個實例和你轉換目標的衍生類很有可能是不同型態的物件，如果你直接做 C-style casting，很有可能會導致程式 crash 掉  

為了要保證可以安全地做轉換，`dynamic_cast` 會在執行期去做轉換的檢查，常見的實作方法是把繼承的關係用樹狀結構畫出來，透過查找這棵樹來確保兩個型態是有 is-a 關係的  

對於指標，在轉換失敗時，產生的值會是 `nullptr`，因此可以將其放在 if-else 的判斷式內偵測轉換結果：  

```cpp
#include <iostream>  

class B {  
public:  
  virtual void whoami() { std::cout << "B\n"; }  
};  

class D1 : public B {  
public:  
  void whoami() override { std::cout << "D1\n"; }  
};  

class D2 : public B {  
public:  
  void whoami() override { std::cout << "D2\n"; }  
};  

void do_casting(B *base) {  
  if(D2 *d2 = dynamic_cast<D2*>(base))  
    d2->whoami();  
  else  
    std::cout << "cannot do the dynamic cast!\n";  
}  

int main() {  
  B *b = new B();  
  D1 *d1 = new D1();  
  D2 *d2 = new D2();  

  do_casting(b);    // cannot do the dynamic cast!  
  do_casting(d1);    // cannot do the dynamic cast!  
  do_casting(d2);    // D2  
}  
```

對於 reference，則是會丟出 `std::bad_cast`，需要透過 try-catch 來去捕捉這個例外：  

```cpp
#include <iostream>  

class B {  
public:  
  virtual void whoami() { std::cout << "B\n"; }  
};  

class D1 : public B {  
public:  
  void whoami() override { std::cout << "D1\n"; }  
};  

class D2 : public B {  
public:  
  void whoami() override { std::cout << "D2\n"; }  
};  

void do_casting(B &base) {  
  try {  
    D2 d2 = dynamic_cast<D2&>(base);  
    d2.whoami();  
  }  
  catch (std::bad_cast){  
    std::cout << "cannot do the dynamic cast!\n";  
  }  
}  

int main() {  
  B b;  
  D1 d1;  
  D2 d2;  

  do_casting(b);    // cannot do the dynamic cast!  
  do_casting(d1);    // cannot do the dynamic cast!  
  do_casting(d2);    // D2  
}  
```

> [n4659(8.2.7-9)](https://timsong-cpp.github.io/cppwp/n4659/expr.dynamic.cast#9)：The value of a failed cast to pointer type is the null pointer value of the required result type. A failed cast to reference type throws an exception of a type that would match a handler of type std​::​bad_­cast.  

downcasting 需要被顯式的寫出來，而且基類要是 polymorphic class，否則會報錯：  

```cpp
#include <iostream>  

class B {};  
class D1 : public B {};  

class B2 {  
public:  
  virtual ~B2() = default;  
};  
class D2 : public B2 {};  

int main()  
{  
  B *b = new D1();  
  D1 *d1 = b;    // error: conversion from 'B' to non-scalar type 'D' requested  
  D1 *d2 = dynamic_cast<D1 *>(b);    // error: cannot 'dynamic_cast' 'b' (of type 'class B*') to type 'class D*' (source type is not polymorphic)  

  B2 *b2 = new D2();  
  D2 *vd1 = b2;    // error: invalid conversion from 'B2*' to 'D2*' [-fpermissive]  
  if (D2 *vd2 = dynamic_cast<D2 *>(b2); vd2 != 0)  
    std::cout << "suceffuly do the downcasting\n";    // ok  
}  
```

在上例中：  

+ 第 14 行：由於是 downcasting，因此無法直接轉換  
+ 第 15 行：由於 `B` 不是 polymorphic class(沒有虛擬函式)，因此無法執行 downcasting  
+ 第 19 行：由於是 downcasting，因此無法直接轉換  
+ 第 20 行：由於 `B2` 是 polymorphic class，因此可以使用 `dynamic_cast` 來做 downcasting，由於 `b2` 的動態型態為 `D2`，所以可以成功地執行轉型  

# Virtual Table  

標準裡面只定義了整個動態型態的外顯行為，也就是這個系統動起來應該要符合哪些規範，該長怎樣，但是實際上底層是怎麼實作這整個動態型態的系統的呢?  

現在主流的實作方法是使用 virtual table 這種方式實作，每一個 polymorphic class 都會有自己的一張表，用來確認執行期應該要去呼叫哪一個對應的函式，以此來達到 dynamic binding 的效果  

polymorphic class 實例的 memory layout 中，會存放一個 virtual table pointer，指向自己的 virtual table，我們可以簡單使用 `reinterpret_cast` 來檢驗這件事  

我們前方有一個講述衍生類實例的 memory layout 的例子，畫了一張圖來講解沒有虛擬函式的 class 的 memory layout：  

```cpp
class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  
```
<center>  

![image](https://hackmd.io/_uploads/Hk1OE4e8T.png)  

</center>  

透過下面這個 code 可以看見 `ptr` 與 `&ptr->i1` 的位址是一樣的：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  

int main()  
{  
  D *ptr = new D();  
  std::cout << reinterpret_cast<int *>(ptr) << ' ' << *reinterpret_cast<int *>(ptr) << '\n';    // 0xb902b0 1  
  std::cout << reinterpret_cast<int *>(&ptr->i1) << ' ' << *reinterpret_cast<int *>(&ptr->i1) << '\n';    // 0xb902b0 1  
}  
```

這裡在做的事情是將 `ptr` 與 `&ptr->i1` 這兩段記憶體上面的值以 `int` 的形式讀出來，因為我們在 `B` 內將 `i1` 初始化為 `1` 了，因此兩個顯示出來的結果都會是 `1`：  

<center>  

![image](https://hackmd.io/_uploads/B1fFWq7IT.png)  

</center>  

當我們讓這個 class 變為 polymorphic class 後，由於多了一個 vtable pointer，結果就會不一樣了：  

```cpp
#include <iostream>  

class B {  
public:  
  int i1 = 1, i2 = 2;  

  virtual ~B() = default;  
};  

class D : public B {  
public:  
  int i3 = 3, i4 = 4;  
};  

int main()  
{  
  D *ptr = new D();  
  std::cout << reinterpret_cast<int *>(ptr) << ' ' << *reinterpret_cast<int *>(ptr) << '\n';    // 0x20f42b0 4202520  
  std::cout << reinterpret_cast<int *>(&ptr->i1) << ' ' << *reinterpret_cast<int *>(&ptr->i1) << '\n';    // 0x20f42b8 1  
}  
```

這是因為多了一個 vtable pointer，導致 `ptr` 與 `&ptr->i1` 指向的位址不一樣了：  

<center>  

![image](https://hackmd.io/_uploads/rk1P-cXIT.png)  

</center>  

而現在我們來看一下 virtual table 的運作模式，考慮以下範例：  

```cpp
#include <iostream>  

class Base {  
public:  
  virtual void func1() { std::cout << "Base::func1()\n"; }  
  virtual void func2() { std::cout << "Base::func2()\n"; }  
  void nonVirtualFunc() { std::cout << "Base::nonVirtualFunc()\n"; }  
};  

class Derived : public Base {  
public:  
  virtual void func2() override { std::cout << "Derived::func2()\n"; }  
  void nonVirtualFunc() { std::cout << "Derived::nonVirtualFunc()\n"; }  
};  

int main()  
{  
  Base *b = new Base();  
  Base *d1 = new Derived();  
  Base *d2 = new Derived();  

  b->func1();    // Base::func1()  
  b->func2();    // Base::func2()  
  b->nonVirtualFunc();    // Base::nonVirtualFunc()  

  d1->func1();    // Base::func1()  
  d1->func2();    // Derived::func2()  
  d1->nonVirtualFunc();    // Base::nonVirtualFunc()  

  d2->func1();    // Base::func1()  
  d2->func2();    // Derived::func2()  
  d2->nonVirtualFunc();    // Base::nonVirtualFunc()  
}  
```

上例中 `Base` 有兩個虛擬函式 `func1` 與 `func2`，並且有一個一般的成員函式 `nonVirtualFunc`；而 `Derived` 內止覆寫了 `func2`，並有對基類的一般成員函式做了 function overloading  

因此當我們透過基類指標呼叫時，第 26 行會是呼叫 `Base::func1`，因為 `Derived` 並沒有覆寫它；第 27 行則是會呼叫 `Derived::func2`，因為有對其做覆寫；而第 28 行會呼叫 `Base::nonVirtualFunc`，因為是透過基類指標呼叫，因此可以找到這個函式  

此例中的 virtual table 樣貌如下：  

<center>  

![image](https://hackmd.io/_uploads/rJHc9qm8a.png)  

</center>  

每一個實例都擁有自己的 vtable pointer，但同一種 class type 會共用同一張 virtual table，在進行虛擬函式的呼叫時會利用自己的這張表去找對應的函式定義  

對於 `d1` 與 `d2` 的 virtual table，由於 `Derived` 沒有覆寫 `func1`，因此 dynamic binding 的結果會連結到 `Base::func1`；而 `Derived` 有覆寫 `func2`，因此 dynamic binding 的結果會連結到 `Derived::func2`  

而由於 `nonVirtualFunc` 不是虛擬函式，因此就 virtual table 內就不會有它的資訊  

另外 virtual table 內並不是只存了 virtual function 的定義，其他像是動態型態的資訊、與多重繼承時用來定位 `this` 的指標也都會存在裡面，有興趣的話可以看看這篇：[c++ vtable 深入解析](https://zhuanlan.zhihu.com/p/268324735)  

以 virtual table 當作關鍵字的話去查應該可以查到很多利用 gdb 工具來去把整個 memory layout 印出來做驗證的文章，所以我暫時就先不寫了(好累)，之後有空的話可能會額外拉一篇出來寫，這邊我有看到一篇還不錯的，大家可以閱讀一下：[C/C++ 修道院第17篇：C++繼承中虛表的記憶體佈局](https://zhuanlan.zhihu.com/p/190169823)