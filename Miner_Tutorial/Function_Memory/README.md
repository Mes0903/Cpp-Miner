---
title: C++ 教學系列 ── Function 與 Memory
date: 2022-03-22
tags: C++ Miner_tutorial
categories:
- C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 ── Function 與 Memory <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>  

點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>  

# 函式 (Function)  

函式是一個有名稱的程式碼區塊，通常拿來表示一個過程，可以接受參數並產生一個結果，我們可以透過呼叫(calling) 來使用函式。  

## Function Declaration  

Function declarations 可以出現在任何的 scope 內，Function 的型態由回傳型態與 Function 的 Declarator 組成。  

> [n4868(9.3.4.6)](https://timsong-cpp.github.io/cppwp/n4868/dcl.fct#1)：... the type of the declarator-id in D is “derived-declarator-type-list noexcept function of parameter-type-list cv-qualifier-seq ref-qualifier returning T” ...  

Function Declaration 的語法長這樣：  

> T noptr-declarator ( parameter-list ) cv(opt) ref(opt) except(opt) attr(opt)	(1)	  
> auto noptr-declarator ( parameter-list ) cv(opt) ref(opt) except(opt) attr(opt) -> trailing (2)  

T 為回傳的型別，第一個是一般的 Function 宣告形式，第二個是 C\+\+11 後出現的尾端回傳形式。  

+ noptr-declarator  
    跟 array 一樣，可以擺任何合法的 declarator，但如果由 `*`、`&`、`&&` 開始，那麼需要用括號括起來，這會用在 Function 回傳陣列的指標(pointer of array) 時。  

+ 參數列 (parameter-list)  
    參數列可以是空的，每個參數間會用逗號來隔開。  

看起來會長：  

```cpp
int fn1(int par1, int par2);    // (1)  
auto fn2() -> int;    // (2)  

int (*fn3())[3];    // function return pointer of array, which dimension is 3.  
```

上面宣告了三個函式，第一個是正常的形式，第二個是有尾端回傳(trailing type) 的形式，如果要使用尾端回傳，於前方 `T` 的部分需要填上 `auto`，第三個是一個回傳型態為 `int(*)[3]` 的函式。  

就像之前講變數一樣，我們要使用函式的話也需要先宣告它，函式的宣告可以有很多個，但定義只能有一個，我們又稱函式的宣告為<span class = "yellow">函式原型(function prototype)。</span>  

函式的宣告可以省略參數的名字，也就是說這樣是可以的：  
```cpp
int fn1(int, int);    // (1)  
```

另外我們常把函式的宣告稱為函式的<span class = "yellow">介面(interface)</span>。  

額外閱讀：[C++ - Function declarations inside function scopes?](https://stackoverflow.com/questions/28470479/c-function-declarations-inside-function-scopes)  
額外閱讀：[In the standard, what is "derived-declarator-type"?](https://stackoverflow.com/questions/13779273/in-the-standard-what-is-derived-declarator-type)  
額外閱讀：[Why does C++ allow unnamed function parameters?](https://stackoverflow.com/questions/12186698/why-does-c-allow-unnamed-function-parameters)  

## Function Definition  

前面講完宣告了，跟變數不一樣，函式的定義需要另外寫，函式的定義會把函式的名字、型態與 function body 連結起來。<span class = "yellow">函式的定義不能出現在函式裡面</span>。  

在宣告函式時我們需要寫回傳值的型態，如果沒有要回傳東西，那就使用 `void`，如果有定義宣告回傳值型態但沒有回傳東西，那麼編譯器會報錯。另外 <span class = "yellow">Function 的回傳型態不能是 Array type 或 Function type。</span>  

function body 由 compound statement 組成，因此需要用 `{}` 包起來，因此於 function 內宣告的變數生命週期與 function 一樣，每次 function 被呼叫時會去執行這個 compound statement，舉個例子：  

```cpp
#include <iostream>  

void fn()  
{  
  std::cout << "get in fn()\n";  
}  

int main()  
{  
  fn();  
}  
```

上面這個例子中有一個函式 `fn`，參數列是空的，也沒有回傳物件，function body 裡面只有一行字串的輸出。在 main function 裡面我透過 `()` 呼叫了 `fn`，因此這段程式的執行結果會是「get in fn()」。  

## return statement  

`return` 是一個 statement，它會結束目前正在執行的程式，回到剛剛呼叫函式的地方，`return` 的語法如下：  

> return expression(opt) ; (1)  
> return braced-init-list ; (2)  

第一種會去計算我們寫的 expression，並將其結果回傳，如果沒有寫 expression，那麼此 statement 只能出現在回傳型態為 `void` 的函式中。  

舉個例子：  

```cpp
#include <iostream>  

void fn(int i)  
{  
  switch (i) {  
  case 1:  
    std::cout << "case 1\n";  
    return;  
  case 2:  
    std::cout << "case 2\n";  
    return;  
  default:  
    std::cout << "default\n";  
    return;  
  }  

  std::cout << "end of the function\n";  
}  

int main()  
{  
  fn(1);  
  fn(2);  
}  
```

這個例子中 `fn` 內有一個 `switch` statement，其中我用 `return` 代替了 `break`，而 `fn` 的最後有一個字串的輸出，但這個輸出不會被跑到，因為在 `switch` 中執行到了 `return`，此時 `fn` 就結束了。  

而第二種則是會使用 copy list initialization 去建構要回傳的物件並回傳，除非串列為空，若串列為空則是使用 value initialize，舉個例子：  

```cpp
#include <iostream>  

int fn(int n)  
{  
  switch (n) {  
  case 0:  
    return {};  
  default:  
    return { n };  
  }  
}  

int main()  
{  
  int i1 = fn(0);    // 相當於 i1 = {}  
  int i2 = fn(1);    // 相當於 i2 = {n}  
}  
```

注意版本要在 C++11 以上  

### Don't return pointer of local variable  

在回傳物件時有一點要注意的是<span class = "yellow">別回傳指向區域變數的指標</span>，因為在離開 function 時區域變數就離開了他們的 scope，因此其生命週期就結束了，此時的 pointer 為 dangling pointer。  

舉個例子：  

```cpp
#include <iostream>  

int *fn()  
{  
  int i = 2;  
  int *p = &i;  

  return p;    // return 後 i 會被解構  
}  

int main()  
{  
  int *p = fn();    // 此時 p 為 dangling pointer  

  std::cout << *p;    // maybe caused an error  
}  
```

reference 也同理。  

# 記憶體配置過程  

一般的 C/C++ 程式在執行時記憶體的配置會長的像下圖那樣，主要可分為 text、data、bss、stack、heap 與 system 這幾個部分。  

![](https://i.imgur.com/flktcJS.png)  

source：[C 語言程式的記憶體配置概念教學](https://blog.gtwang.org/programming/memory-layout-of-c-program/)  

+ text 段  
    text 也被稱為 code 段，存放可執行的 cpu 指令，這裡的資料是可以共用的，並且是唯讀的。  

+ data 段  
    data 段拿來放有初始化的靜態變數，如全域變數與 static 變數，舉個例子：  

    ```cpp
    #include <iostream>  

    int i = 0;  

    void fn()  
    {  
    static int si = 0;  
    }  

    int main()  
    {  
    }  
    ```

    其中的 `i` 與 `si` 會存在 data 段。  

    data 段還有分可讀寫與唯讀的部分，可讀寫的部分稱為 read-write area，拿來存放一般變數，而唯讀區則稱為 read-only area，負責存放固定的常數。  

+ bss 段  
    bss 段拿來存放沒有初始化的靜態變數，這些變數在程式執行前會被系統初始化為 `0` 或 `null`，舉個例子：  
    ```cpp
    int i;  

    int main()  
    {      
    }  
    ```

    其中的 `i` 會被儲存在 bss 段。  

+ stack 段  
    stack 段拿來儲存函數的區域變數，以及各種函數呼叫時需要儲存的資訊，每一次函數呼叫時就會在 stack 段建立一個 frame，一層一層疊下去，這樣不同函數就不會互相干擾。  

+ heap 段  
    heap 段拿來儲存動態配置的變數，如 `malloc` 與 `new` 所建立出來的變數。  

+ system  
    system 段拿來儲存跟系統有關的環境變數與命令列參數。  

source：[C 語言程式的記憶體配置概念教學](https://blog.gtwang.org/programming/memory-layout-of-c-program/)  

舉個例子：  

```cpp
#include <iostream>  

int fn(int i)  
{  
  int n = i;  
  std::cout << "in fn()\n";  
  return n;  
}  

int main()  
{  
  int i1 = fn(1);  
  int i2;  
}  
```

程式執行的流程會長：  

![](https://i.imgur.com/fBxTjhN.png)  

一開始先分配好 main function 需要的大小，然後將 `fn(1)` 的 frame 推到 stack 內，之後因為要回傳值，所以會將要回傳的值儲存到暫存器或 stack 中，再把 `fn(1)` 的 frame 刪除，並把剛剛儲存的值拿去給 `i1` 初始化。  

# new 與 delete  

### new expression  

因為 `fn` 結束後其相對應的 stack frame 也會消失，因此區域變數也會跟著解構，如果想要自己掌控變數的生命週期，我們就需要利用動態配置，因為動態配置的變數會存在 heap 段，所以當 stack frame 消失時並不會影響到動態配置的變數。  

要動態配置記憶體，需要利用 `new` expression，語法長這樣：  

> ::(opt) new ( type ) initializer(opt)	(1)	  
> ::(opt) new new-type initializer(opt)	(2)	  
> ::(opt) new (placement-params) ( type ) initializer(opt)	(3)	  
> ::(opt) new (placement-params) new-type initializer(opt)	(4)	  

`new` 會試著在 heap 段建構相對應型態的變數，(1) 與 (2) 的差別在於 (2) 的型態沒有包含括號，舉個例子：  

```cpp
new int(*[10])();    // error: parsed as (new int) (*[10]) ()  
new (int (*[10])()); // okay: allocates an array of 10 pointers to functions  
```

這邊型態是 function pointer `int(*[10])()`，所以有 `()` 在裡面，導致需要使用第一種。  

另外，`new-type` 會把所有可以屬於 declarator 的 token 都包含近來：  
```cpp
new int + 1; // okay: parsed as (new int) + 1, increments a pointer returned by new int  
new int * 1; // error: parsed as (new int*) (1)  
```

利用 `new` 所建立的物件是沒有名字的，`new` 在成功建構物件後會回傳它的記憶體位址，因此我們需要使用指標去儲存這個位址，不然之後會沒法使用這個物件。  

`new` expression 代表的東西有兩種，一為 `operator new`，另一種為 `operator new[]`，type 為 array type 時使用的為後者，其他時候使用的 `new` 為前者。  

initializer 如果沒寫，那會使用 default initialized，我們通常會使用 `()` 來當初始化器，此時會透過 Direct initialized 來初始化 heap 上的物件，舉個例子：  

```cpp
#include <iostream>  

int main()  
{  
  int *p = new int(20);  
  std::cout << *p;    // 20  
}  
```

上面這裡用 `new` 在 heap 段建構了一個整數物件，初始化為 20，並將其位址回傳給 p，圖看起來會長這樣：  

<center><img src = "https://i.imgur.com/G1Y3ycj.png"></center><br>  
而如果物件的型態為 array type，我們通常會用空括號 `()` 來進行 value initialized，在 C\+\+11 後，則可以使用 `{}` 來初始化物件，舉個例子：  

```cpp
#include <iostream>  

int main()  
{  
  int *p1 = new int[3];    // default initialized  
  // 亂數  
  for (int i = 0; i < 3; ++i)  
    std::cout << p1[i] << " ";  
  std::cout << '\n';  

  int *p2 = new int[3]();    // value initialized  
  // 0 0 0  
  for (int i = 0; i < 3; ++i)  
    std::cout << p2[i] << " ";  
  std::cout << '\n';  

  // after C++11  
  int *p3 = new int[3]{ 1, 2, 3 };    // aggregated initialized  
  // 1 2 3  
  for (int i = 0; i < 3; ++i)  
    std::cout << p3[i] << " ";  
  std::cout << '\n';  
}  
```

上面有三個 `new` 出來的陣列，第一個沒有初始化器，因此會是預設初始化，第二個的初始化器則為 `()`，使用 value initialized，因此陣列的元素全被初始化為 `0`，第三個的初始化器則為 `{}`，為每個元素都寫好了初始化的值。  

### delete  

由於動態配置的物件我們需要自己掌握其生命週期，因此解構也需要自己來，使用的是 `delete` expression，語法長這樣：  

> ::(opt)    delete    expression	(1)	  
> ::(opt)    delete [] expression	(2)	  

第一個給一般物件用，第二個給 array type 的物件用。  

expression 的型態需要是一個 pointer to object type，或是可以轉型為相對應的 pointer 的 class type，而值可以是 `null` 或 `new` 出來的物件的位址。  

基本上如果是單獨一個物件就用第一個，如果是連續空間就用第二個，舉個例子：  

```cpp
int main()  
{  
  int *p1 = new int;  
  delete p1;  
  p1 = nullptr;  

  int *p2 = new int[3];  
  delete[] p2;  
  p2 = nullptr;  
}  
```

一開始我們 `new` 了一個整數物件出來，之後將其解構，並將 `p1` 指標歸位，`p2` 同理。  

有 `new` 物件出來就需要有 `delete` 將其解構，否則那個物件會一直存在 heap 區，導致 memory leak  

### 動態配置陣列  

之前有說過陣列的大小需要是靜態時期就能夠知道大小的常數，除非是動態配置，由此我們可知動態配置出來的陣列可以在執行期才決定大小，舉個例子：  

```cpp
#include <iostream>  

int main()  
{  
  int size = 0;  

  std::cout << "輸入長度: ";  
  std::cin >> size;  
  int *arr = new int[size]{ 0 };  

  for (int i = 0; i < size; i++)  
    arr[i] = i;  

  std::cout << "元素值: " << '\n';  
  for (int i = 0; i < size; i++) {  
    std::cout << "arr[" << i << "] = " << arr[i]  
              << '\n';  
  }  

  delete[] arr;  

  return 0;  
}  
```

可以看見 `arr` 陣列的大小為 `size`，是一個執行期才知道的數，再來再舉一個二維陣列的例子：  

```cpp
#include <iostream>  

int main()  
{  
  int size1, size2;  
  int **arr = new int *[2];  

  std::cout << "input size1: ";  
  std::cin >> size1;  
  arr[0] = new int[size1]{ 0 };  

  std::cout << "input size2: ";  
  std::cin >> size2;  
  arr[1] = new int[size2]{ 0 };  

  /*---------------輸出---------------*/  
  for (int i = 0; i < size1; i++) {  
    std::cout << arr[0][i] << " ";  
  }  
  std::cout << '\n';  

  for (int i = 0; i < size2; i++) {  
    std::cout << arr[1][i] << " ";  
  }  
  std::cout << '\n';  
  /*--------------------------------*/  

  // 清除陣列  
  for (int i = 0; i < 2; i++) {  
    delete[] arr[i];  
  }  
  delete[] arr;  

  return 0;  
}  
```

例子 source：[new 與 delete](https://openhome.cc/Gossip/CppGossip/newDelete.html)  

# Parameter List  

Parameter List 還有一些地方還沒講，但在講之前，大家需要先知道什麼是 reference，接下來的 call by value、call by reference 才聽得懂。  

前面一直都有提到 reference 這個東西，reference 的中文叫「別名」或「參考」，通常參考當動詞用，別名當名詞用。在 `int &a = b` 這個語句中，我們會說 `a` 參考 `b`，或 `a` 引用了 `b`，因此 `a` 是 `b` 的別名，或 `a` 是 `b` 的參考，講法很多，自己溝通時聽得懂就好。  

reference 是一個很重要的東西，在近代 C\+\+ 中大量的被使用，算是近代 C\+\+ 的一大基石。  

以下內容我直接從 C\+\+ Miner 的 Value Categories 篇擷取過來，做了一點小刪減，讓新手比較好看懂。  

## Reference  

### 什麼是參考 (Reference) ?  

參考是一種變數，其型別(Type) 是他連結到的東西的型態的引用(reference to type)，這邊不講物件的原因是因為他連結到的東西不一定是個物件，也有可能是函式之類的東西，語法長這樣：  

> T & attr(opt) declarator	(1)	  
> T && attr(opt) declarator	(2)	(since C++11)  

第一個為 Lvalue Reference，第二個為 Rvalue Reference，後面會介紹兩者的差異。  

Reference 會像是被連結到的東西的別名一樣，呼叫、使用他，就等於呼叫、使用被連結到的東西。舉個例子：  
```cpp
int main  
{  
  int i = 0;  
  int &r = i;  
}  
```

`i` 是一個物件，而我們最一開始有舉過一個例子說物件就像一個箱子，名字就是箱子上的標籤，而 reference 又像被連結到的東西的別名，換句話說 reference 就是另外一張標籤：  

<center><img src = "https://i.imgur.com/tw1yi84.png"></center><br>  

直接操作 `r` 就等同於對物件 `i` 操作，所以才會說 `r` 是 `i` 的別名。  

參考並不一定會有 memory allocate，而且參考也不是一個 object，這樣才符合他的精神「別名」。儘管為了實現語意，Compiler 可能會給他 memory allocate。  

參考一定要被初始化，來看下面這個<a href = "https://eel.is/c++draft/dcl.init.ref" class = "pinklink">例子</a>：  
```cpp
int g( int ) noexcept;    //回傳 int 的函式  

void f() {  
    int i;  
    int &r = i;    // r 與 i 連結  
    r = 1;    // r 連結到的變數的值變為 1，也就是說 i 變為 1  
    int *p = &r;    // p 為 i 的指標  
    int &rr = r;    // rr 連結到 r 連結的變數，也就是說 rr 連結到 i  
    int ( &rg )( int ) = g;    // rg 是函式 g 的參考，  
    rg( i );    // 呼叫 g 函式  

    int a[3];  
    int( &ra )[3] = a;    // ra 連結到 a，ra 是 a 陣列的參考  
    ra[1] = i;    // 改變 a[1] 的值  
}  
```

一但 reference 被初始化，我們就無法再對 reference 本身進行操作了。我們也不能有參考的參考(reference to reference)，參考的陣列(arrays of reference)，和參考的指標(pointer to references)，因為 reference 並不是物件(object)。  

> [n4868(9.3.4.3)](https://timsong-cpp.github.io/cppwp/n4868/dcl.ref#5)：There shall be no references to references, no arrays of references, and no pointers to references.  

這樣看起來，一旦參考被初始化後，參考「本身」好像就消失在程式當中，真的變成一個「別名」了，那麼底層是如何實作的呢? 不一定! 通常是利用指標來實作，但要記得，他不一定要是指標，參考與指標本質上是不同的東西，僅僅是因為他們倆個的性質很相似而已。  

那麼，Compiler 知道參考本身的存在嗎? 答案是知道：  

```cpp
int a = 0;  
int &b = a;  

std::cout << std::boolalpha   
          << std::is_same_v<decltype(a), decltype(b)>; // false  
```

可以看見 Compiler 可以分出來兩者的差別，因為兩者的 type 是不同的。那麼我們來看看一個有編譯器優化的狀況：  

<center><img src = "https://i.imgur.com/EEpi2JR.png"></center><br>  

<a href = "https://godbolt.org/z/8KqxsKrPM" class = "pinklink">連結</a>在這裡，記得要把編譯器優化打開。 我們可以看見 `std::cin >> r;` 像是被替換成 `std::cin >> a;` 了，跟 `inline` 類似。 <span class = "yellow">(感謝Cy大神補充)</span>  

如果對 Referecne 的定義與相關的詳細規範還有興趣，可以到<a href = "https://eel.is/c++draft/dcl.ref" class = "pinklink">這裡</a>看看。  

### Lvalue Reference  
那麼現在大家都有背景知識了，就來看一下什麼是 Lvalue Referecne 和 Rvalue Referecne 吧。  

首先我們看 Lvalue Reference，語法長這樣 <a href = "https://en.cppreference.com/w/cpp/language/reference" class = "pinklink">(來源)</a>：  

> & attr(opt) declarator  

lvalue 裡面的 「l」 主要的意涵是 locatable，也就是有固定位址的，因此 Lvalue Reference 會與一個有固定位址的物件連結，`int &r = a;` 這樣的話 r 就是一個 Lvalue Referecne，型態是 `int&`。那麼他有幾個用處：  

+ Lvalue Reference 可以用來連結一個已經存在的 object，也就是 Lvalue Expression 回傳的物件：  
	```cpp
	#include <iostream>  
	#include <string>  

	int main() {  
	    std::string s = "Ex";  
	    std::string &r1 = s;  
	    const std::string &r2 = s;  

	    r1 += "ample";           // 改動了 s  
	//  r2 += "!";               // error: cannot modify through reference to const (不能更改一個 const string 的 reference)  
	    std::cout << r2 << '\n'; // Example  
	}  
	```

+ 可以用來當作 pass-by-reference 的函式參數：  
	```cpp
	#include <iostream>  
	#include <string>  

	void double_string(std::string &s) {  
	    s += s; // 's' 和 main() 裡面的 `str` 是同一個物件  
	}  

	int main() {  
	    std::string str = "Test";  
	    double_string(str);  
	    std::cout << str << '\n';  
	}  
	```

+ 可以用來當作函式回傳的物件，如此一來函式回傳的型態會是 `int&`：  
	```cpp
	#include <iostream>  
	#include <string>  

	char& char_number(std::string &s, std::size_t n) {  
	    return s.at(n); // string::at() 回傳一個reference，型態是 char&  
	}  

	int main() {  
	    std::string str = "Test";  
	    char_number(str, 1) = 'a';  
	    std::cout << str << '\n'; // Tast  
	}  
	```

這些差不多就是 Lvalue Reference 的作用了，應該非常符合他是一個「別名」的意義吧!  

### Rvalue Reference  

#### <span class = "wheat">先談談歷史</span>  

在進到 Rvalue Reference 前，我們要先稍微了解一下歷史，才會知道為何要有 Rvalue Reference，所以我們先從舊的 C++ 講起。  

在以前，我們只擁有 Lvalue Reference，但 Lvalue Reference 沒有辦法連結到沒有 cv 限定詞的暫時物件或常量：  
```cpp
int main() {  
    int &r = 0;    //error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'  

    return 0;  
}  
```

因為 Lvalue Reference 只能綁定到一個 Lvalue Expression 回傳的物件，從語意上來講，更改暫時物件的值不一定是合理的，而常量值更不用說了，他甚至可能不是個物件，沒有儲存位址，要怎麼改動他呢? 由於 Lvalue Refference 綁定到的物件基本上可以被更改(除非他有加上 const 限定詞)，所以我們無法綁定 Prvalue Expression 與 Xvalue Expression (統稱 Rvalue)。  

那麼這會出現什麼問題呢? 最明顯的問題出在函式 call-by-reference 的身上：  
```cpp
#include <iostream>  

int func(int &a, int &b) {  
    return a + b;  
}  

int main() {  
    int a = 1, b = 1;  
    int c = func(a, b);    // ok  

    int d = func(1, 1);    //error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'  
}  
```

可以看見 `func(a, b)` 是合法的，這很合理，我們傳了兩個有固定位址的物件進去，但是   
`func(1, 1)` 就有問題了，因為這兩個是數字，根本沒有在記憶體佔有空間，而我們的需求也很直覺，就是將傳入的兩個整數相加後回傳。  

怎麼解決呢? 照剛剛的邏輯，不能利用 Lvalue Reference 做連結的原因是因為常量值照理說不應該被更改。那麼只要我們保證不更改他就好了對吧! 所以答案是<strong><span class = "yellow">加上 const</span></strong>：  
```cpp
#include <iostream>  

int func(const int &a, const int &b) {  
    return a + b;  
}  

int main() {  
    int a = 1, b = 1;  
    int c = func(a, b);    // ok  

    int d = func(1, 1);    // ok  
}  
```

我們先在這邊暫停一下，剛剛不是說了常量值可能沒有記憶體位址? 那麼就算加上了 `const` 也不該可行呀! 所以這時候出現了一個東西叫做臨時物化(TMC)，我們來看看利用 `const int&` 型態的 reference 綁定 `1` 時到底發生了什麼：  
```cpp
int main() {  
    const int &cr = 1;  
}  
```

組語輸出：  

```assembly  
main:  
        push    rbp  
        mov     rbp, rsp  
        mov     eax, 1  
        mov     DWORD PTR [rbp-12], eax  
        lea     rax, [rbp-12]  
        mov     QWORD PTR [rbp-8], rax  
        mov     eax, 0  
        pop     rbp  
        ret  
```

不會組語的朋友沒關係，我這邊寫個偽代碼來示意：  
```cpp
int main() {  
    int __e = 1;    //mov     DWORD PTR [rbp-12], eax  
    const int &cr = __e;    //mov     QWORD PTR [rbp-8], rax  
}  
```

發現了嗎? 產生了一個新的變數! 其實他是個匿名物件，但為了方便我把它取名叫 `__e`，如此一來我們的確無法透過 `cr` 來更改到 `1`(\_\_e) 的值，卻又連結到了 `1` 這個常量，就好像替 `1` 取了一個別名叫做 `cr`。  

但這樣又出現了兩個問題，首先是我們無法區分 Rvalue 與 Lvalue，剛剛的例子中可以看見 `1` 和 `a`、`b` 都一樣可以傳進 `func()` 裡面。  

再來是既然產生了臨時物件，我們也透過 Reference 連結到它了，而且它本身也不是個不可更改的物件，因為在被 Reference 連結後，它的生命週期已經大幅增加，不再是個暫時物件了，那麼也就是說：  
```cpp
int main() {  
    int __e = 1;  
    const int &cr = __e;  
    cr;    // cr 是 Lvalue Expression  
}  
```
`cr` 已經是一個 Lvalue Expression 了，既然我們都知道會有臨時物件出現，並且它的生命週期會因為 Reference 的連結而增加，那麼可以被更改也很合理吧? 但是在函式中很明顯它不這麼覺得：  
```cpp
#include <iostream>  

int func(const int &a, const int &b) {  
    ++a;    //error: increment of read-only reference 'a'  
    return a + b;  
}  

int main() {  
    int a = 1, b = 1;  
    int c = func(a, b);    // ok  

    int d = func(1, 1);    // ok  
}  
```

`++a` 被擋了下來，這也很正常，因為它是唯讀的，那麼要怎麼做才可以讓它像這樣呢：  
```cpp
int main() {  
    int __e = 1;  
    int &cr = __e;  
}  
```
如果能夠沒有 `const`，那麼 `++a` 就可以過，事情就解決了對吧？  

但這樣就回到了一開始的問題，常量並沒有記憶體位址，語意上來說不該能被更改，我們會說它能更改是從臨時物化與記憶體的角度去看，但在寫程式的時候我們應該由語意方面來寫，而不是底層，因為還有 Compiler 優化與平台差異的問題，底層應該交給他們去判斷。 於是陷入了死胡同，直到 C++11 時為了滿足移動語意，Rvalue Reference 的出現及值類別、TMC、Copy Elision 等更詳細的定義，這個問題才被解決。  

#### <span class = "wheat">Rvalue Reference 的出現</span>  

所以為了滿足移動語意，Rvalue Reference 出現了，大量取代了以前使用 const Reference 的情景。 Rvalue Reference 是 C\+\+11 後的東西，使用前記得看一下自己的 C\+\+ 版本，語法長這樣：  

> && attr(opt) declarator  

沒錯，就只是多了一個 `&`，意義就大有不同，注意它不是參考的參考，前面已經說過不能有參考的參考了。  

Rvalue Reference 有幾種特性 <a href = "https://en.cppreference.com/w/cpp/language/reference#Rvalue_references" class = "pinklink">(來源)</a>：  

+ 延長暫時物件的生命週期：  

    ```cpp
    #include <iostream>  
    #include <string>  

    int main() {  
      std::string s1 = "Test";  
    //  std::string&& r1 = s1;           // error: can't bind to lvalue  

      const std::string& r2 = s1 + s1; // okay: 利用唯讀的 Lvalue Reference 來延長 s1 + s2 的生命週期  
    //  r2 += "Test";                    // error: can't modify through reference to const  

      std::string&& r3 = s1 + s1;      // okay: 利用 Rvalue Reference 來延長 s1 + s2 的生命週期  
      r3 += "Test";                    // okay: 可以更改連結到的物件的值  
      std::cout << r3 << '\n';    //TestTestTest  
    }  
    ```

    可以看見唯讀的 Lvalue Reference 跟 Rvalue Reference 都可以延長暫時物件的生命週期，但只有 Rvalue Reference 可以更改連結到的物件的值，因為其不是唯讀的型態。  

+ 函式 call-by-reference 時能夠區分 Rvalue 與 Lvalue：  

    ```cpp
    #include <iostream>  
    #include <utility>  

    void f(int& x) {  
      std::cout << "lvalue reference overload f(" << x << ")\n";  
    }  

    void f(const int& x) {  
      std::cout << "lvalue reference to const overload f(" << x << ")\n";  
    }  

    void f(int&& x) {  
      std::cout << "rvalue reference overload f(" << x << ")\n";  
    }  

    int main() {  
      int i = 1;  
      const int ci = 2;  
      f(i);  // calls f(int&)  
      f(ci); // calls f(const int&)  
      f(3);  // calls f(int&&)  
              // would call f(const int&) if f(int&&) overload wasn't provided  
      f(std::move(i)); // calls f(int&&)  

      // rvalue reference variables are lvalues when used in expressions  
      int&& x = 1;  
      f(x);            // calls f(int& x)  
      f(std::move(x)); // calls f(int&& x)  
    }  
    ```

    可以看見 Prvalue Expression 與 Xvalue Expression 會傳入 Rvalue Reference 的版本，與 Lvalue Reference 的版本區分開來了。  

    這是我們能夠實現移動語意的關鍵，因為移動建構子或其他接受移動的函式吃的就是 Xvalue，建構子需要能夠區分 Lvalue 與 Rvalue。  

## Parameter Passing  

Function 中傳遞參數的方式有兩種，一為 pass by value、另一種為 pass by reference。  

### Pass by value  

前面我們已經知道了在 call function 時記憶體內會建立 stack frame，參數會被<span class = "yellow">複製</span>進去，舉個例子：  

```cpp
#include <iostream>  

void fn(int a, int b)  
{  
  a = 5;  
  b = 10;  
}  

int main()  
{  
  int i = 0, j = 0;  
  fn(i, j);  

  std::cout << i << " " << j;  
}  
```

上例中有一個函式 `fn`，吃兩個參數 `a` 與 `b`，並在 function body 中更改了他們的值。 而 main function 中有兩個整數物件 `i` 與 `j`，都初始化為 0，並傳進 `fn` 中。  

在 `fn` 呼叫完畢後我輸出了 `i` 與 `j` 的值，因為參數傳遞的方式是 pass by value 的關係，所以 `fn` 裡面的修改並不會影響到 main function 內的 `i` 與 `j`。  

圖解起來長這樣：  

![](https://i.imgur.com/bkW3vY2.png)  

整體步驟大概是：  

1. 進入 main function，配置好 `i`、`j` 的記憶體空間並初始化  
2. 呼叫 `fn`，將參數存入暫存器或 stack  
3. 建立起 `fn` 的 stack frame，並將 `a` 與 `b` 透過暫存器的值初始化  
4. function body，執行 `a = 5;` 與 `b = 10;`，更改 `a` 與 `b` 的值  
5. 退出 function，解構 `a`、`b`，並回收 stack frame  

### Pass by reference  

上面那個方法將會複製傳進去的參數，這有一個壞處，當傳進去的物件很大時，將其複製就會很花時間，或是有些物件根本就不允許複製，此時就可以使用 reference 來傳遞參數，如此一來就可以避免整個物件的複製。  

另外一點就是當我們想要更改原物件時，由於傳遞進去的是一個複本，因此在 function 內做的更改並不會去修改到原先的物件。  

聽起來 pass by reference 的好處很多，你可能會想說那總是使用 reference 來傳遞就好，但其實對於小物件，編譯器與底層硬體會有針對 pass by value 的優化，所以像是 `int` 這種小物件，用 value 來傳並不一定會比較慢，甚至會比較快。  

pass by reference 比較快的結果只有在一種情況下會發生，那就是接受參數的 function 內沒有發生 parameter optimization 導致 function local 必須配置記憶體給 parameter。且 pass by value 有時還有 cache-inline 的優勢。  

如果單純只考量速度，通常來說只要 value size < pointer size(通常是 64 bits)，那我們就會使用 pass by value，然而實際上在寫 code 的時候還要考慮維護性，此時可能會使用 const reference，這部分就要請大家自己拿捏判斷了。  

講了這麼多，那就開始看要怎麼使用 reference parameter 吧！  

只需要直接在參數列使用 reference type 就好，例如：  

```cpp
#include <iostream>  

void fn(int& a, int& b)  
{  
  a = 5;  
  b = 10;  
}  

int main()  
{  
  int i = 0, j = 0;  
  fn(i, j);  

  std::cout << i << " " << j;  
}  
```

執行結果可以看見 `i` 與 `j` 的值在經過 `fn` 後都改變了，因為我們傳的不是利用 copy，而是利用 reference 來傳遞參數，我們可以利用 pointer type 的參數做到類似的事情：  

```cpp
#include <iostream>  

void fn(int *a, int *b)  
{  
  *a = 5;  
  *b = 10;  
}  

int main()  
{  
  int i = 0, j = 0;  
  fn(&i, &j);  

  std::cout << i << " " << j;  
}  
```

可以看見 `i` 與 `j` 的值一樣也改變了，但要注意 pointer 是個物件，而 reference 不是，所以我們能利用 pointer 改變原物件的原因是因為我們複製了原物件的位址進去，並利用位址來操作原物件，而 reference 則是直接將參數與原物件連結，以概念上來說是不一樣的，reference 的方法中，等同於直接操作原物件，但 pointer 的方法則是間接操作原物件。  

額外閱讀：[Want Speed? Pass by Value.](http://web.archive.org/web/20140113221447/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/)  
額外閱讀：[Pass by value faster than pass by reference](https://stackoverflow.com/questions/22840120/pass-by-value-faster-than-pass-by-reference)  
額外閱讀：[WANT SPEED? DON’T (ALWAYS) PASS BY VALUE.](https://juanchopanzacpp.wordpress.com/2014/05/11/want-speed-dont-always-pass-by-value/)  

## 預設引數 Default Argument  

有些函式可能有某個參數在大部分的情況下都是固定的值，例如一個生成視窗的函式，一個視窗預設大小為 250 $\times$ 250，此時就可以利用 Default argument 來實作：  

```cpp
Window make_window(int length = 250, int width = 250) {  
    Window window(length, width);    // 生成視窗  
    //...  

    return window;  
}  
```

預設的參數是<span class = "yellow">由左至右</span>來解讀的，也就是說呼叫時長這樣：  

```cpp
int main() {  
    Window A = make_window();    // length = 250, width = 250  
    Window B = make_window(100);    // length = 100, width = 250  
    Window C = make_window(100, 150)    // length =   
}  
```

也因為是由左至右解讀的，所以以下這個形式的 Default argument 是錯的：  

```cpp
Window make_window(int length = 250, int width);    // error:default argument missing for parameter 2 of 'Window make_window(int, int)'  
```

因為是由左至右解讀的，因次在呼叫時若寫 `make_window(100)`，這個 `100` 是會傳給 `length` 而不是 `width`，導致 `width` 沒有收到值也沒有預設值，所以上面那個形式是錯的。  

我們可以透過多個有預設引數的宣告來解決這個問題：  

```cpp
Window make_window(int length, int width = 50);  
Window make_window(int length = 250, int width);  
```

這樣在解讀時就不會有漏掉值的問題了。  

# 函式重載 Function Overloading  

當有很多個 Function 可能有類似的操作時，或是一個 Function 有很多種操作時，如 `print` 函式可能可以輸出字串、整數、布林值，此時我們可以透過 Function Overload 來幫助我們實作。  

Function Overload 的中文翻譯很多，有重載、多載、過載等等，建議記英文比較好，因為還有兩個東西叫 Override 和 Overwrite，中文名稱也很像，之後講 Class 時會提到。  

那我們就來看看怎麼使用 Function Overloading 吧：  

```cpp
#include <iostream>  
#include <string>  

void Print(int i) {  
    std::cout << i << '\n';  
}  

void Print(std::string str) {  
    std::cout << str << '\n';  
}  

void Print(bool flag) {  
    std::cout << flag << '\n';  
}  

int main() {  
    Print(123);  
    Print(std::string("Hello world"));  
    Print(true);  
}  
```

上面有三個同樣名稱但參數不同的函式，此時編譯器就會根據你傳入的參數去找應該要呼叫哪一個 function，這個動作叫做函式匹配(Function matching)。  

在 Function matching 中，他會先去查名字(Name lookup)，如果有同名的 Function，才會去對照 parameter list，這個叫做 ADL(Argument-dependent lookup)。  

## Top level const、Low level const  

考慮這個例子：  

```cpp
void Print(int i) {  
    std::cout << "without const\n";  
}  

// error: redefinition of 'void Print(int)'  
void Print(const int i) {  
    std::cout << "with const\n";  
}  
```

你會發現這個編譯器報了錯，很顯然的下面那個 Function 參數列中的 `const` 並無法幫助 ADL 分辨 Function，這個概念牽扯到 const 的 level。  

const 是有分頂層(top-level) 與底層(low-level) 的，廣義來說，頂層的 const 代表 const 作用在物件本身，如 `const int i`、`int *const i_ptr` 等等，物件本身被賦值後就無法再做修改；底層的 const 代表 const 作用在型態上，這會出現在<span class = "yellow">複合型別</span>內，如 `const int *ptr`、`const int &r` 等等，這個 const 表示物件連結到的物件的型態有 const 修飾。  

而回到上面的例子，編譯器會報錯的原因很單純，因為 ADL 無法區分頂層的 const，原因是 parameter 中頂層的 const 並不會算在 function type 的裡面，而 ADL 是根據 function type 來查找的，因此其便會判斷上下兩個 `Print` 是同一個 Function，因此有兩個定義而報錯。  

但 low-level 的 `const` 是沒有問題的：  
```cpp
void Print(int *i) {  
    std::cout << "without const\n";  
}  

void Print(const int *i) {  
    std::cout << "with const\n";  
}    // okay, passed the compiled  
```
# 遞迴 (Recursion)  

在函式中是可以再呼叫自己的，可以是間接呼叫或是直接呼叫，這種呼叫自己的函式我們稱為遞迴函式，一個常見的例子是費氏數列的運算：  

```cpp
#include <iostream>  

int fib(int n)  
{  
  if (n == 1 || n == 2)  
    return 1;  

  if (n > 2)  
    return fib(n - 1) + fib(n - 2);  

  return 0;  
}  

int main()  
{  
  std::cout << fib(1) << " " << fib(2) << " " << fib(3) << " " << fib(4) << "...";  
}  
```

上例中 `fin(1)` 與 `fin(2)` 會直接進到第一個 if statement，因此輸出為 1，之後的會進到第二個 if statement，因此會將 `n - 1` 與 `n-2` 當參數再去呼叫一次 `fib`。  

我們可以把樹畫出來看，以 `fib(4)` 為例，會長的像這樣：  

![](https://i.imgur.com/1KpgDMA.png)  

額外閱讀：[Tail recursion in C++](https://stackoverflow.com/questions/2693683/tail-recursion-in-c)  

# Function Pointer  

Function Pointer 與一般的指標差別在於 Function 並不是物件，因此特性有一些差別，Function Pointer 一樣會指向一個特別的型態，這個型態以 Function 的 return type 與 parameter list 組成，如一個指向 `int(int, int)` 的指標 `int (*ptr)(int, int)`。  

一個 function pointer 可以利用 non-member function 或 static member function 來初始化，在賦值時會 implicity 的加上 `&`：  

```cpp
#include <iostream>  

int fn()  
{  
  return 0;  
}  

int main()  
{  
  int (*ptr1)() = &fn;  
  int (*ptr2)() = fn;    // implicit add & operator, equivalent to above one  

  int a = ptr1();  
  int b = ptr2();  

  std::cout << a << " " << b;  
}  
```

implicity 加上 `&` 的這個動作算一種 conversion，叫做 function-to-pointer conversion。  

如果要使用 function pointer 指向的函式，可以直接使用 calling operator `()` 而不用使用 `*` operator，但也是可以 deference 之後再去呼叫，這樣會變成使用 function lvalue 來呼叫，通常來說外顯行為是一樣的：  

```cpp
#include <iostream>  

int fn()  
{  
  return 0;  
}  

int main()  
{  
  int (*ptr1)() = &fn;  
  int (*ptr2)() = fn;    // implicit add & operator, equivalent to above one  

  int a = (*ptr1)();  
  int b = (*ptr2)();  

  std::cout << a << " " << b;  
}  
```

# Main function 中的 argc、argv 參數  

我們可以透過命令列傳一些參數進去 main function，此時這些資訊會利用 `argc` 與 `argv` 這兩個參數來儲存，`argc` 是個整數，記錄命令列參數的個數，而 `argv` 則是一個指向 char array 的 pointer，儲存命令列內容：  

```cpp
#include <iostream>  

int main(int argc, char *argv[])  
{  
  std::cout << argc << " ";  

  for (int i = 0; i < argc; ++i)  
    std::cout << argv[i] << " ";  
}  
```

假設我們的執行檔名稱為 test.exe，那麼當我執行指令 `./test.exe 1 2 3` 時，argc 就為 4，第一個參數為程式的名稱，第二三四個參數則為 `1 2 3`，因此上面的例子會輸出 `4 C:\Mescpp\test\test.exe 1 2 3` (我這邊執行檔位置在 Mescpp\\test 下)  

![](https://i.imgur.com/bUQTYHO.png)  

