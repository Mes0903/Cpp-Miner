---
title: C++ 教學系列 ── Object、Expression、Statement
date: 2022-02-25
abstract: 本文介紹了 C++ 中 Object、Expression 與 Statement 的想法，這在 C++ 中是很重要的概念，然而對初學者比較不友善，大家可以斟酌閱讀，可以跳著看，吸收自己能理解的部分
tags: C++ Miner_tutorial
categories:
- C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 ── Object、Expression、Statement <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>  

點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>  

# 前言  

這是 C\+\+ 礦坑系列的前篇，為了讓大家能夠看懂礦坑系列在寫什麼，決定從 0 開始做一整個系列的教學。  

設定的對象是完全沒有學過 C\+\+ 的人，雖說如此，我在大學部試教了一學期後發現大部分人聽不懂，所以建議還是有一定基礎再來看，我會努力寫到讓什麼都不會的人看得懂的XD...  

這個系列主要希望能讓大家擁有看 spec 的能力，也就是所謂的規格書，spec 裡面有非常多的術語，我在剛開始學習時常常由於大量術語疊加起來導致自己看不懂 spec 想表達什麼，因此這個教學我會盡量的提到 spec 裡面的術語，並盡量講的詳細，引用 spec 裡面的語句來解釋，可能會有點枯燥，但都是基本功，想學好 C 或 C++ 的話這些是不可避免的。  

若能讀懂 spec，將來在一些討論區，如 Discord、Stackoverflow 與別人討論時也才能夠聽懂別人在說什麼，在回答問題時我們時常會直接將 spec 的內容搬出來，直接說因為哪個條款，所以有怎樣的結果，若看不懂 spec，很有可能連與別人討論都做不到。  

<center><img src="https://hackmd.io/_uploads/rki6B4bn2.png">(熟悉 C 與 C++ 的人在回答問題時通常會直接將 spec 內容列出來闡述原因)</center></br>  
<center><img src="https://hackmd.io/_uploads/ry4XuXW23.png">(在問問題時引用 spec 條例也能有效幫助別人理解你的疑惑點)</center></br>  

講義主要參考了 SICP、C++ Primer 的編排方式，並參考[語言技術：C++ Gossip](https://openhome.cc/Gossip/CppGossip/index.html)、spec 與 cppreference 來撰寫內容。  

2023/08/09 謝謝良葛格老師一直以來的文章，一路走好  

由於我也不是特別厲害的大佬，只是個想分享一些心得的人，所以講義都是可編輯的，若發現文章有謬誤，或是有什麼想補充的，都歡迎大家直接修改文件~  

2022/02/25 by Mes  

# Hello World  

我們的 code 寫好後會經過前處理器處理，然後把檔案交給編譯器編譯，變成組合語言，再透過組譯器轉換為機械語言，也就是一堆指令，存在硬碟裡面，這樣的東西我們稱之為 Program。  

當我們要執行這個 Program 時，電腦會把它從硬碟搬到記憶體裡面，CPU 再去讀取指令，這樣正在執行的 Program 我們稱之為 Process。  

![](https://hackmd.io/_uploads/B1IDUV-3n.png)  
> source：[淺談 c++ 編譯到鏈結的過程](https://medium.com/@alastor0325/https-medium-com-alastor0325-compilation-to-linking-c07121e2803)  
在[環境建置篇](https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FMinerT_enviroment)內有類似這樣的一段程式碼：  

```cpp
#include <iostream>  

int main()  
{  
    std::cout << "Hello World";  
    return 0;  
}  
```

前篇裡面輸出的字串是 `"test"`，這邊則是 `"Hello World"`，這是許多初學者都會學到的第一個程式，現在我們來看一下這段 code 在說什麼。  

首先看第一行：  
```cpp
#include <iostream>  
```

這行是給前置處理器看的，`#include` 這段告訴前置處理器要引用一個檔案，`<iostream>` 則是我們要引用的檔案，前置處理器會幫忙把我們引用的檔案的內容像複製貼上一樣到現在這個 cpp 檔裡面，我們也可以自己寫要引用的檔案，這邊我們建立一個檔案叫做 `include.h`：  

```cpp
// in include.h  
#include <iostream>  
```

```cpp
#include "include.h"  

int main() {  
    std::cout << "Hellow world";  
    return 0;  
}  
```

在 Vscode 裡面看起來像這樣：  

![](https://i.imgur.com/eQdlBHY.png)  

可以看見輸出結果跟一開始的一樣。  

`#include "include.h"` 代表我們要引用一個檔案，檔案名字叫 `include.h`，你可能有發現上面的 `iostream` 是用 `<>` 包起來的，而這邊的 `include.h` 則是用 `""` 包起來的，這是因為如果是標頭檔這類有在系統目錄的檔案我們會使用 `<>`，如果是自定義的則會使用 `""` 來包。  

對於 `<>`，前處理器會先去系統目錄找要引用的檔案，以這邊的 `<iostream>` 來說，位置就在一開始下載的 `mingw/include/c++/11.2.0/` 裡面，因為我們事先有將 `mingw/bin` 加到系統環境變數裡面了，所以它才找的到：  

![](https://i.imgur.com/FepAPRM.png)  

而對於 `""`，前處理器會先在現在檔案所處的資料夾找檔案，如果沒找到，則會去系統目錄找，所以我們可以做個測試，新建一個空的檔案叫做 `iostream`，跟 `test.cpp` 擺在同一個資料夾下：  

![](https://i.imgur.com/NmMB0Nb.png)  

此時我們引用的 `iostream` 會是 `test` 這個資料夾下的 `iostream` ，因為是空的，沒有引用到我們真正需要使用的，在系統目錄裡的 `iostream`，所以編譯會出現錯誤：  

![](https://i.imgur.com/GFUgJPf.png)  

可以看見編譯器提醒我們要 `#include <iostream>`；而如果我們把 `test/iostream` 拉到外面一層：  

![](https://i.imgur.com/IPsQWvO.png)  

就能夠通過編譯，正常執行，因為當前路徑資料夾下並沒有 `iostream` 這個檔案，所以前處理器會去系統目錄下找檔案：  

![](https://i.imgur.com/WbyGRyC.png)  

我們自己寫的檔案會用 `.h` 來當附檔名，並用 `""` 來引用。  

接下來我們看下面的 `int main() {...}`，這是一個函式，函式會有一個回傳型態，以這個例子來說是 `int`，然後會有個名字，以這邊來說就是 `main`，之後會接一個參數列表，裡面可能會有 0 或 0 以上個 parameters，然後會接一組 `{}` 當作 function body，若函式有，關於函式我們後面會更詳細地提到它。  

`main` 是一個特別的函式，它是 Program 的進入點，也就是你的程式會從 `main` 這個函式開始執行。  

這邊 `main` function 裡面有一行 `std::cout << "Hello world"`，這行是拿來輸出東西在終端機上的，關於這行後面會再進行更多說明，最後有一個 `return 0;`，代表這個函式結束後會回傳 `0`，對於程式來說，main function 執行完，程式基本上就結束了，所以 `main` function 的回傳值還有一些特殊意義，`0` 代表程式正常結束。  

# Object and Variable  

## Definition of object  

接下來要正式進入教學了，我們從生活化的例子來切入，假設今天你跟朋友到早餐店，點餐的時候跟店員說了要兩杯大冰奶，於是店員就拿出了一張小便條紙寫了 `2*15`，放在桌上，然後你又點了兩個吐司夾蛋，此時店員又拿了<span class = "yellow">另外一張便條紙</span>，上面寫了 `2*30`，接著你說這樣就好，於是他便算出 `30+60 = 90`，跟你收了 90 元，然後把兩張便條紙丟掉。  

你可能會覺得很奇怪，沒事幹嘛拿兩張便條紙，但先忽略這點，因為我想不到更好的例子ㄏㄏ。 上面這個例子中，店員用了兩張便條紙，一張上面存了「兩杯大冰奶」的資訊，一張上面存了「兩個吐司夾蛋」的資訊，在 C++ 中，這種能像便條紙一樣存取資料，或像紙箱一樣可以裝資料的東西我們稱為「<span class = "yellow">物件</span>」。  

讓我們來看一下 spec 裡面是怎麼說物件的：  

> n4868(6.7.2) : An object occupies a region of storage in its period of construction ([class.cdtor]), throughout its [lifetime](https://timsong-cpp.github.io/cppwp/n4868/basic.life), and in its period of destruction ([[class.cdtor]](https://timsong-cpp.github.io/cppwp/n4868/class.cdtor))  

也就是說一個物件會經過三個時期：建構、建構後解構前、解構，在這期間物件都會佔有一段空間，也就是邏輯上的記憶體空間，以上面的例子來說，就是便條紙被撕下的那一瞬間，放在桌上的那一段期間，把便條紙丟掉的那一瞬間，這個便條紙都會「存在」且在桌上佔有空間。  

## Type  

而我們會利用型態(type) 對儲存的資料進行分類，就像一個標籤一樣，拿來形容物件、引用、函式與表達式  

> n4868(6.8.1) : [[basic.types]](https://timsong-cpp.github.io/cppwp/n4868/basic.types) and the subclauses thereof impose requirements on implementations regarding the representation of types. There are two kinds of types: fundamental types and compound types. Types describe objects, references, or functions.  

:::info  
註：這邊標準沒有列到 expression 是有原因的，但由於背後牽涉到 expression type 與 object type，解釋起來略為複雜，這邊就先不贅述，大家可以先忽略它  
:::  

可以看見型態分兩種，fundamental types 和 compound type，[cppreference](https://en.cppreference.com/w/cpp/language/type#Type_classification) 有表可以看。而常見的原始型別，又稱為算術型別(arithemetic types)，則是 fundamental types 的一種。  

算術型別主要有兩種，浮點數(floating-point types)與整數(integral types)，而整數型別又可再往下分 bool、character types、signed integer types、unsigned integer types。  

> n4868(6.8.2) : Types bool, char, wchar_t, char8_t, char16_t, char32_t, and the signed and unsigned integer types are collectively called integral types. A synonym for integral type is integer type.  

bool 是一種特殊的型態，bool 型態的物件只能儲存兩種值：true 與 false，bool 沒有分正負號，也沒有 long bool 這種東西，bool 能幫助我們判斷邏輯，像是真假值表就會利用到 bool。  

> n4868(6.8.2) : Type bool is a distinct type that has the same object representation, value representation, and alignment requirements as an implementation-defined unsigned integer type. The values of type bool are true and false.  

而 character types 則是字元型態，character types 的物件所儲存的數字會被對應到字元集去解釋和實作，字元集又稱字元編碼，常見的有 ASCII、Big5、UTF-8 等等，下面這是 ASCII 字元編碼表  

<center><img src = "https://i.imgur.com/xr4FPxZ.png"></center>></br>  

> source : http://kevin.hwai.edu.tw/~kevin/material/JAVA/Sample2016/ASCII.htm  

而 signed interger 與 unsigned integer 則都是整數，不過前者有分正負號，而後者則只有正號，  

### Data size  

資料型態會影響物件在記憶體空間佔有的大小，最常見的模型中，`int` 佔 4 bytes，`char` 佔 8 bits，`bool` 佔 1 byte，`float` 佔 4 bytes，`double` 佔 8 bytes。  

但這只是最常見的模型，實際上不同的環境下可能會有不同的結果，以 `int` 來說，在 C99 標準，64-bits 的 Unix 環境下 `int` 就佔 8 bytes，可以看看 wiki 上的[表格](https://en.wikipedia.org/wiki/Integer_(computer_science)#Long_integer)。  

# Declaration  

## Simple-declaration  

而我們還可以給物件取名字，習慣上我們會稱有名字的物件為「變數 (variable)」，但 spec 裡面有寫道：  

> n4868(6.1) : A *variable* is introduced by the declaration of a reference other than a non-static data member or of an object. The variable's name, if any, denotes the reference or object.  

所以在定義上，只要是物件或者是 static data member 的 reference 都可以稱為變數，後者大家可以先不用管他，只要知道口語上的變數指的是有名字的物件就好。  

延伸閱讀 : [Why is a non-static data member reference not a variable?](https://stackoverflow.com/questions/12987259/why-is-a-non-static-data-member-reference-not-a-variable)  

我們會通過「宣告 (declaration)」來給變數取名字，宣告的意義在告訴編譯器有某個東西的存在，長的會像這樣：  

```cpp
int main()  
{  
  int a;  
  int b = 0;  
  const int c = 0;  

  return 0;  
}  
```

因為 C++ 中有很多不同的東西，所以宣告也有[很多種形式](https://timsong-cpp.github.io/cppwp/n4868/dcl.dcl)，而對於變數，我們用的通常是 simple-declaration，語法有三種：  

> decl-specifier-seq init-declarator-list(opt) ;	(1)	  
> attr decl-specifier-seq init-declarator-list ; (2)  
> attr(opt) decl-specifier-seq ref-qualifier(opt) [identifier-list] initializer ; (3)  

基本上大同小異，我們用的是第一種，他有兩個部分：decl-specifier-seq 和 init-declarator-list。  

註：第一個的 opt 指的是初始化器，  

> init-declarator-list:  
>> init-declarator  
>> init-declarator-list , init-declarator  

> init-declarator:  
>> declarator initializer(opt)  
>> declarator requires-clause  
## Declare Specifier  

第一部分是 decl-specifier-seq，由 Declare Specifier 組成，[cppreference](https://en.cppreference.com/w/cpp/language/declarations#Specifiers) 上有列表可以看，裡面有很多關鍵字，像是 `inline`、`constexpr` 等等，但我們這邊最主要要討論的是 type specifiers，也就是我們上面講到的型態，像是 `int`、`char` 或 `bool` 之類的，也可以是你自己定義的型態，還有 cv-qualifiers。  

簡單舉幾個例子，`int a`、`char c`、`bool b` 都有 type specifiers。  

而 cv-qualifiers 指的是兩個關鍵字：`const` 與 `volatile`，`const` 表示後面接的型態是唯讀的，`volatile` 表示每次使用後面此型態的物件時都要從變數位址裡面讀資料，舉幾個例子：`const int a`、`volatile char c`、`const volatile bool b` 都擁有 cv-qualifiers。  

> n4868(9.1) : An object definition causes storage of appropriate size and alignment to be reserved and any appropriate initialization ([dcl.init]) to be done.  

要注意 type specifier 跟 type 是不一樣的，可以看一下簡單的[例子](https://timsong-cpp.github.io/cppwp/n4868/tab:dcl.type.simple)，可以看見有些不同的 type specifier 所指定的 type 是相同的，如 `unsigned` 與 `unsigned int`。  

## Declarator  

第二個部分是 init-declarator-list，由宣告器(declarators) 組成，宣告器之間以逗號分割，每個宣告器裡面可能會有初始化器(initializer)。  

declarator 用來引入變數，最簡單的宣告器由一個變數名組成，像是 `int a` 裡面的 `a` 就是宣告器，而 `int a, b = 5` 則有兩個宣告器，引入 `a` 與 `b` 兩個變數，而後方的 `b` 還包含了一個 initializer，將 `b` 變數的值初始化為 5。  

declarator 可以擁有很複雜的語法，以便我們宣告更複雜的東西，如之後的章節會講到的 pointer、reference、array 等等，但每個 declarator 一定需要有一個 declarator identifier，也就是你宣告的東西的名字。  

舉幾個例子：`int *a, b` 裡面有兩個宣告器，一個是 `*a`，代表 `a` 是個指標，而 `b` 則是最一般的宣告器；`int *const p` 的宣告器則是 `*const p`，表示 `p` 是一個唯讀的指標，也就是說 `p` 物件裡面存的東西只能讀不能改。  

延伸閱讀：[What are declarations and declarators and how are their types interpreted by the standard?](https://stackoverflow.com/questions/13808932/what-are-declarations-and-declarators-and-how-are-their-types-interpreted-by-the)  

## Definition  

而我們前面說宣告的意義在於告訴編譯器有某個東西的存在，但對物件來說，這不代表他現在存在，他可能等等才會出現，只是先告訴了編譯器有這個東西而已，也就是說可能有了名字，卻還沒有記憶體空間。  

而如果要給予物件記憶體空間，則需要去定義他，前面沒有這種感覺是因為物件的宣告本身就會跟著定義，除非我們加上了 `extern` 標示詞。  

> n4868(9.1) : An object declaration, however, is also a definition unless it contains the extern specifier and has no initializer. An object definition causes storage of appropriate size and alignment to be reserved and any appropriate initialization to be done.  

也就是說以下的 identifier `a` 並沒有記憶體位址：  

```cpp
#include <iostream>  
extern int a;  

int main() {  
    std::cout << &a;  
    return 0;  
}  
```

因此在編譯(連結)時會有錯誤顯示 "undefined reference to \`a\` "。  

下面的部份可以待大家後面的章節讀熟後再回來看，因為 Declaration 其實是個很複雜的概念。  

一般而言 definition 的目的是為了完整前面宣告所引入的一個 entity，entity 是一個抽象的概念，基本上指的是一個元件，具體來說可以是值、物件、referencce、function 等等  

> n4868(6.1)：An entity is a value, object, reference, structured binding, function, enumerator, type, class member, bit-field, template, template specialization, namespace, or pack.  

因此大部分的 Declaration 都會是個 Definition，因為有完整引入資訊，如 namespace 的宣告，然而若是 function、class 等，就需要額外加上對應的定義，如：  

```cpp
void fn();  
class T;  
```

這兩個宣告並沒有完整引入資訊，這通常會在 forward declaration 時使用，需要有對應的 definition 才可以開始使用  

這邊就可以代入一個觀念叫 incomplete type，他分為 `void` 與 incompletetly-defined object type，我們重點在後面這個，從名字可以看出他是有關物件型態的東西，具體指：  
1. 宣告但未定義的 class  
2. underlying type 尚未確定的 enum  
3. 未知長度的 array  
4. 成員為 incomplete type 的 array  

看這個表可能會有點矇，實際上想表達的意義就是還不知道物件「大小」的型態，我們知道 `int` 通常是 4-byte，就算不是 4-byte，在你 compile 的時候他一樣會有個大小，但上面這些東西我們並不會知道它的大小  

> [n4659(6.9)](https://timsong-cpp.github.io/cppwp/n4659/basic.types#5) : A class that has been declared but not defined, an enumeration type in certain contexts, or an array of unknown bound or of incomplete element type, is an incompletely-defined object type.46 Incompletely-defined object types and cv void are incomplete types. Objects shall not be defined to have an incomplete type.  

## Scope  

C++ 裡面所有的 name 都只能在某個範圍內可以被看見，這個範圍我們稱為 Scope，口語上稱為可視範圍。  

有這樣的設計是因為在程式的任何段落上，name 都指涉一個特定的實體，像是變數、函式、型態等，為了讓一個名字能重複使用，這代表同一個名字在不同地方可能會指涉到不同實體，需要有 Scope 的設計，同一個名字只能出現在不同的 Scope 內出現，這稱為 variable shadowing。  

> n4868(6.4.1) : exactly one declaration shall declare a class name or enumeration name that is not a typedef name and the other declarations shall all refer to the same variable, non-static data member, or enumerator, or all refer to functions and function templates; in this case the class name or enumeration name is hidden ([basic.scope.hiding]).  

C++ 大部分的 Scope 都是由一組大括號 `{}` 來建立的，預設上操作 name 時會對同一個 Scope 下的 name 操作，如果同一個 Scope 沒有這個 name，那麼就會再往外面一層 Scope 找，這個行為稱為 Unqualified name lookup。  

Scope 也影響了變數的生命週期，一旦變數離開他建構時所在的 Scope 便會解構，舉個例子  

```cpp
int main() {  
    int a = 0;  
    int i = 0;  
    ++i;  
    {  
        int i = 1; // i 建構  
        i = 42; // 改的會是裡面的 i (上面那行那個)  

        int j = a; // 合法的操作，這裡的 a 是外面那個 a，a 的生命週期還沒結束，所以可以使用  
    } // 裡面的 i, j 離開了建立時所處的 Scope，因此解構  

    j = 5; // 這行會導致編譯錯誤，因為 j 所處的 Scope 已經結束了  

    return 0;  
}  
```

延伸閱讀：[What are qualified-id/name and unqualified-id/name?](https://stackoverflow.com/questions/7257563/what-are-qualified-id-name-and-unqualified-id-name)  

# Expression  

講完宣告了，接下來要講表達式(expression)，Expression 用來描述一段計算過程，由運算元(operand) 與運算子 (operator) 組成，Expression 會有一個計算(Evaluation) 的結果，且可能會有 side-effect。  

舉幾個簡單的例子：`2 + 5` 是一個表達式，當中 `2` 是一個運算元，`5` 是另一個運算元，`+` 是運算子；`(2 + 5) * 2` 也是一個表達式，而這個表達式擁有子表達式 `(2 + 5)`，當中 `2` 和 `5` 是運算元，`+` 是運算子，而對於表達式 `(2 + 5) * 2` 來說，`(2 + 5)` 和 `2` 是運算元，`*`是運算子，這種有兩個以上運算子的表達式我們稱之為複合表達式(compound expression)。  

而所謂的 side-effect 是一種額外的操作，舉個例子，我們宣告一個整數 `int i;`，然後寫了 `i = 3`，這個運算式計算返回的結果是 `i`，但途中卻順便讓變數 `i` 儲存的值變為 `3` 了，這就是 side-effect，有時候 side-effect 才是我們操作的主要目的。  

> n4868(6.9.1) : Reading an object designated by a volatile glvalue, modifying an object, calling a library I/O function, or calling a function that does any of those operations are all side effects, which are changes in the state of the execution environment.  

Expression 有兩個特性：Type 與 Value Category，Type 就是前面講的那個 Type，而 Value Category 講起來非常複雜，這個系列應該是不會講到，有興趣的可以去看礦坑系列的文章。  

## 運算子  

根據運算元的數目，我們可以簡單將運算子區分為單元運算子(unary operator)、二元運算子(binary operator) 和三元運算子(ternary operator)。  

而運算子在 [cppreference](https://en.cppreference.com/w/cpp/language/expressions#Operators) 上有表可以看，在複合運算式中運算子會有執行的優先順序，就像是在數學式裡面也有先乘除後加減，括號要先做等等的規則，有關順序的規則可以看[這裡](https://en.cppreference.com/w/cpp/language/eval_order)。  

後面我把一些比較常用的運算子挑出來討論，每個類別裡面還會有其他的運算子，記得點進 cppreference 看全部的運算子。  

### 指派運算子 Assignment operators  

指派運算子用來修改物件儲存的值，像是前面已經看過幾次的 `=` 就是一種指派運算子，叫做 simple assignment operator，如 `a = 5`，這樣我們會說將數字 5 賦值給變數 `a`。  

而另外還有一些指派運算子，如 addition assignment operator `+=`，作用是將左方的運算元加上右方的運算元，例如 `a += 5`，就會把變數 `a` 儲存的值加上 5，另外還有 `*=`、`/=`、`%=` 等等的運算子  

```cpp
int main() {  
    int a = 0;  
    a += 1; // 等價於 a = a + 1, 也就是說 a = 0 + 1，因此 a 現在儲存的值變為 1 了  
    a *= 2; // 等價於 a = a * 2。也就是說 a = 1 * 2，因此 a 現在儲存的值變為 2 了  
    a /= 2; // 等價於 a = a / 2。也就是說 a = 2 / 2，因此 a 現在儲存的值變為 1 了  

    return 0;  
}  
```

這裡有一個重點，<span class = "yellow">初始化與賦值是不同的操作</span>，如果把物件想像為一個箱子，那麼初始化就是將箱子拿出來時便順便把東西放進箱子裡，而賦值則是箱子拿出來後先放在桌上一段時間，隨後再把東西丟進箱子，雖然兩者看起來很像，但性質上完全不同，賦值可以有很多次，我們可以不斷的更改變數裡面儲存的資料，但初始化只會有一次，發生在物件生成時。  

### 算術運算子 Arithmetic operator  

一般的加減乘除、模除，取正負等數學運算，在 C++ 中可以利用算術運算子來操作，這些運算子基本上由左至右運算，而遇到加減乘除等問題時也是先乘除後加減，需要時可以利用括號來讓某段子運算式先算。  

舉幾個簡單的例子：`1 + 1`、`i * 2`、`i / 3`。  

### 遞增遞減運算子 Increment/decrement operators  

寫程式的時候對變數遞增 1 或遞減 1 是很常見的事，例如計數器就會用到這種操作，遞增遞減運算子有四種：pre-increment operator、post-increment operator、pre-decrement operator、post-decrement operator，前兩者的符號都為 `++`，後兩者的符號都為 `--`  

差別在於運算元放的位置，前置的兩者，運算元會放在運算子的後面，如 `++a`、`--a`，而後置的兩者則相反，如 `a++`、`a--`。  

前置的兩者會先將變數的值加一，隨後將<span class = "yellow">原物件</span>當作運算結果，也就是說下例中的 `b` 所儲存的值會是 1：  

```cpp
int main() {  
    int a = 0;  
    int b = ++a; // b == 1  

    return 0;  
}  
```

而後置的兩者則會先生成一個運算元的<span class = "yellow">複本</span>，當作運算結果，然後再對原物件加一，也就是說下例中的 `b` 所儲存的值會是 0：  

```cpp
int main() {  
    int a = 0;  
    int b = a++; // b == 0  

    return 0;  
}  
```

換句話說 `int b = a++;` 這行的外顯行為與 `int b = a;  a += 1;` 是一樣的。  

### 比較運算子 Comparison operator  

比較運算子用來比較運算元的關係，像是常見的大於 `>`、小於 `<`，還有相等 `==` 和不相等 `!=`，注意 equal-to operator `==` 是兩個等號，一個等號的是簡單指派運算子。  

two-way 比較運算子，也就是上面常見的那些，會回傳 bool type 的 value，也就是 `true` 和 `false`，  

1. a < b  
    如果 a 小於 b，運算結果為 true，否則為 false  
2. a > b  
    如果 a 大於 b，運算結果為 true，否則為 false  
3. a <= b  
    如果 a 小於等於 b，運算結果為 true，否則為 false  
4. a >= b  
    如果 a 大於等於 b，運算結果為 true，否則為 false  
5. a == b  
    如果 a 等於 b，運算結果為 true，否則為 false  
6. a != b  
    如果 a 不等於 b，運算結果為 true，否則為 false  

在這些例子裡面，`a` 和 `b` 一定要符合下面兩個條件中的其中一個  

+ 型態為 arithmetic 或 enumeration  
+ 型態為 pointer type  

### 成員訪問運算子 Member access operators  

常見的有 subscript-operator `[]`、indirection `*` 和 address-of operator `&`，第一個之後講 array 時會更詳細的講  

address-of operator 可以拿來取運算元的記憶體位址，例如 `&a` 的運算結果就會是 `a` 的記憶體位址，如果我們想要拿變數儲存記憶體位址，則我們會需要 pointer type 的變數。  

pointer type 變數的宣告方式在上面有看見，會在宣告器的部分會加上 `*`，如 `int *p` 為一個儲存「整數物件記憶體位址」的變數，口語上會說他是一個指向整數物件的指標，而 `int **p` 則同理，為一個儲存「『儲存整數物件記憶體位址的物件』的位址的物件」，念起來很繞口，簡單來講就是一個「儲存指標物件位址」的物件，口語上會說他是一個指標的指標。  

想要使用指標可以如下這樣：  

```cpp
int main() {  
    int a;  
    int *p = &a;  

    return 0;  
}  
```

如此一來 `p` 為一個指向整數物件的指標，而 `&a` 的運算結果為 `a` 的記憶體位址，在宣告語句中我們利用了 `a` 的記憶體位址來初始化變數 `p`。  

而 indirection operator `*` 則是幫我們看某段記憶體上儲存了什麼，如 `*(&a)`，`(&a)` 是 `a` 的記憶體位址，利用 `*` 則能幫我們看 `a` 裡面儲存了什麼，所以 `*(&a)` 等價於 `a`。  

舉個簡單的例子  

```cpp
int main() {  
    int a = 5;  
    int *p = &a;  

    int b = *p;  

    return 0;  
}  
```

這個例子中 `a` 儲存的值為 5，`p` 儲存的值為 `a` 的記憶體位址，而 `b` 儲存的值為 `p` 儲存的位址上「儲存的值」，也就是 `a` 的值。  

有關指標，之後會專門有一小節來講。  

## Conversion  

當我們使用不同型態的值來賦值給變數時會發生型態的轉換，舉幾個例子：  

```cpp
bool b = 42;  // b 的值為 true  
int i = b; // i 的值為 1  
i = 3.14; // i 的值為 3  
double pi = i; // pi 的值為 3.0  
```

> source : C++ Primer  

第一行裡面我把一個整數值指派給一個型態為 bool 的物件 `b`，此時如果整數值為 0，計算的結果為 `false`，否則為 `true`，因此在此例中 `b` 的值為 `true`。  

第二行裡面我把一個型態為 bool 的物件指派給整數物件 `i`，此時如果 bool 物件的值為 `true`，計算結果就會是 1，否則為 0，因此在此例中 `i` 的值為 1。  

第三行內我用一個浮點數值指派給一個整數變數，此時值就會被截斷(truncated)，變數所被指派的值會變為小數點的整數部分，因此在此例中 `i` 的值為 3。  

第四行內我用一個整數值指派給一個浮點數變數，此時小數部分就會是 0，如果整數的位元數比浮點數物件所容納的還多，就會喪失精確度。  

這種運算元型態為 arithmetic 所發生的轉換稱為 usual arithmetic conversion，運算結果的型態會與需要的型態一致，也就是會轉成你需要的型態給你用。  

在 [n4868(7.4)](https://timsong-cpp.github.io/cppwp/n4868/expr.arith.conv) 內可以看到更詳細的解釋，裡面有詳細寫了會發生 arithmetic conversion 的情況。  

# Statement  

上面簡單介紹完 Expression 了，Expression 你可以把它想像成一段完整操作的「組件」，假設我們今天的完整操作是 `(2 + 5) * 2`，則 `(2 + 5)` 與 `2` 都是整個 Expression 其中一個的組件。  

而在 C++ 裡面我們用 Statement 來表示一段「完整操作」，是一個完整程式的碎片，除非有特殊的操作，否則 Statement 會由上往下循序執行，以下面這個例子來說  

```cpp
int main() {  
    int a = 0;  
    int b = a;  
    int c = b + 5;  
}  
```

第二三四行都各是一個 Statement，他們都是這個程式的其中一小個碎片，且都是一段「完整操作」，程式會在第二行執行完後執行第三行，再去執行第四行。  

你可以把 Statement 想成一個程式的基本單位，大部分的 statement 都會以分號 `;` 當作結尾，表示這組操作已經結束了。  

C++ 的 Statement 主要有下面這些：  

1. labeled statements;  
2. expression statements;  
3. compound statements;  
4. selection statements;  
5. iteration statements;  
6. jump statements;  
7. declaration statements;  
8. try blocks;  
9. atomic and synchronized blocks (TM TS);  

也就是說一個完整的 C++ Program 幾乎只會用上面 8 種 Statement 來完成，而每個 statement 裡面可能還可以包含另一個 statement，比較小的 statement 我們稱為 substatement。  

## 運算式陳述句 Expression statements  

一個運算式陳述句以一個 expression 加上分號組成，語法像這樣：  

> expression(opt) ;  

注意 expression 是 optional 的，這代表 `;` 也是一個 statement，它有一個名字叫做 empty statement，也被稱為 null statement，在語言要求需要擺 statement，但邏輯上不需要的地方，就可以用 null statement 來幫忙，之後會有例子。  

Expression statement 是最常見的陳述句，像是賦值、加減乘除和函式呼叫這些都是運算式陳述句，簡單舉一些例子：  

```cpp
int main() {  
    int a = 0, b = 1; // Declaration Statement  

    a = 0; // Expression statements  
    a = 2 * 5; // Expression statements  
    a = ++a; // Expression statements  
    a = b + 5; // Expression statements  
}  
```

第 4 行之後的都是運算式陳述句。  

## 複合陳述句 Compound statements  

compound statement 又稱為 block，由一些小的 statement 組成，小的 statement 稱為 substatement，語法像這樣：  

> { statement...(opt) }  

我們會利用 `{}` 來建立 block，當一個預期只能擺一個 statement 的地方，需要多個 statement 時，我們就能夠利用複合陳述句來包裝，每個複合陳述句會建立自己的 scope，變數離開 scope 時會被解構，忘記的可以回到上面複習一下。  

舉個例子，if-statement(一種 selection statement) 最簡單的語法長這樣：  

> if constexpr(opt) ( init-statement(opt) condition ) statement  

當 codition 為 `true`，程式會去執行後方的 statement，寫起來像這樣：  

```cpp
int main() {  
    int a = 0;  

    if(a == 0)  
        std::cout << "in if statement";  

    return 0;  
}  
```

當 `a == 0` 這個條件符合時(上例符合)，便會去執行 `std::cout << "in if statement";` 這個 expression statement，這邊只做了一組動作，但如果我們想要執行多組 statement，那麼就需要 compound statement 了，寫起來像這樣：  

```cpp
int main() {  
    int a = 0;  

    if(a == 0) {  
        std::cout << "in\n";  
        std::cout << " if statement 1\n";  
    }  

    return 0;  
}  
```

我們利用 `{}` 將兩組 expression statement 包裝成單一一個 compound statement，接在 if-statement 後面，如此一來就可以執行多組操作了。  

## 宣告陳述句 Declaration statements  

宣告本身就是一個 Statement，會在 block 內引入一個以上的 identifier。  

## 選擇陳述句 Selection statements  

選擇陳述句能幫我們進行流程控制，建立分支，語法長這樣：  

> if constexpr(opt) ( init-statement(opt) condition ) statement	(1)	  
> if constexpr(opt) ( init-statement(opt) condition ) statement else statement	(2)	  
> switch ( init-statement(opt) condition ) statement	(3)	  

第一個是上面提到的 if-statement，第二個則是帶有 `else` 的 if-statement，第三個是 `switch` statement，由於第三個需要有 label，因此後面講 Labeled Statements 時再來介紹。  

寫起來會像這樣：  

```cpp
if (condition)  
    statement  
else  
    statement      
```

`condition` 的部分如果運算結果轉為 `bool` 時值為 `true`，那麼會去執行 `if` 後面接的 statement，我們稱之為 `statement-true`；如果為 `false`，那麼就會去執行 `else` 後面接的 statement，我們稱之為 `statement-false`。  

`condition` 部分可以放兩個東西  

1. 可以透過 contextually convertion 轉為 bool 的 expression  
2. > decl-specifier-seq declarator brace-or-equal-initializer  

第二個基本上就是個帶有初始化器的變數宣告，但沒有分號，另外宣告器不能 specify 函式或 array，簡單來說就是變數的型態不能是 array type。  

這些規則也可以套用到等等 for 迴圈、while 迴圈的 codition 上。  

> n4868(8.1) : The rules for conditions apply both to selection-statements and to the for and while statements ([stmt.iter]). A condition that is not an expression is a declaration ([dcl.dcl]). The declarator shall not specify a function or an array. The decl-specifier-seq shall not define a class or enumeration. If the auto type-specifier appears in the decl-specifier-seq, the type of the identifier being declared is deduced from the initializer as described in [dcl.spec.auto].  

後面接的 statement 也可以是一個 if-statement，也就是說我們可以像這樣寫：  

```cpp
if (condition)   
    statement  
else   
    if(condition)  
        statement  
```

習慣上我們會把 `if` 直接寫在 `else` 的後面，長的會像：  

```cpp
if (condition)   
    statement  
else if(condition)  
    statement  
```

如果 `statement-true` 又是一個 if-statement，那麼之後的 else 會與最近的沒有 `else` 的 if 結合，舉個例子：  

```cpp
if (condition)  
    if (condition)  
        statement  
    else  
        statement  
else  
    statement  
```

上例中第四行的 `else` 與第二行的 `if` 是一組的，第六行的 `else` 與第一行的 `if` 是一組的。  

讓我們看一個例子，假設我們紅茶的數量有兩杯以上，就跟店員要袋子，不然就不用袋子：  
```cpp
int main() {  
    bool bag_flag;  
    int num = 2;  

    if (num > 1)  
        bag_flag = true;  
    else  
        bag_flag = false;  

    return 0;  
}  
```

在 C\+\+17 後的版本，`if` 後方的圓括號內還可以有 init-statement，init-statement 可以是 expression statement 或 simple declaration，語法像這樣：  

```cpp
if (init-statement condition)  
    statement  
else  
    statement  
```

它與下面這樣等價：  

```cpp
{  
    init-statement  
    if (condition)  
        statement  
    else  
        statement  
}  
```

沒有 `else` 的 if-statement 也一樣，這邊就不贅述，舉個例子：  

```cpp
int main() {  
    int num = 2;  

    if (bool bag_flag; num > 1)  
        bag_flag = true;  
    else  
        bag_flag = false;  

    return 0;  
}  
```

## 疊代陳述句 Iteration statements  

疊代陳述句可以幫助我們重複執行一段程式碼，口語上我們稱之為迴圈，總共有四種，while 迴圈、do-while 迴圈、for 迴圈、range for 迴圈，range for 迴圈我會在之後講容器時一併介紹。  

`break` statement 可以幫助我們離開迴圈，而 `continue` statement 可以幫助我們跳過當次疊代，直接開始下一次疊代。  

### while loop  

語法長這樣：  

> while ( condition ) statement		  

寫起來像這樣：  
```cpp
while (condition)  
    statement  
```

condition 同上面 if-statement 的 codition，可以是 expression 或宣告，這個 Expression 會在每次疊代前計算，如果計算結果轉換為 `bool` 後為 `false`，會離開迴圈，反之則進入迴圈，舉幾個例子：  

```cpp
#include <iostream>  

int main() {  
    int x = 5;  

    while(false)   
        std::cout << "in while 1\n";  

    while(--x > 0)   
        std::cout << "in while 2\n";  

    while(++x < 3) {  
        std::cout << "in while 3\n";  
        std::cout << "x = " << x << '\n';  
    }  

    x = 0;  
    while(++x) {  
        if(x == 6)  
            continue;  
        else if(x == 8)  
            break;  

        std::cout << "in while 4\n" << "x = " << x << '\n';  
    }  
}  
```

上例中有四個 while 迴圈，我們一個一個來看：  

```cpp
while(false)   
    std::cout << "in while 1\n";  
```

在疊代開始前會先去計算 condition 裡的 expression，計算結果為 `false`，因此不會進入迴圈。  

```cpp
while(--x > 0)   
    std::cout << "in while 2\n";  
```

在疊代開始前會先去計算 condition 裡的 expression，每次迴圈開始前 x 會遞減 1，當結果小於等於 0 時 expression 會回傳 `false`，所以這個迴圈會執行 4 次，但 expression 被計算了 5 次，跳出迴圈後 x 的值為 0。  

```cpp
while(++x < 3) {  
    std::cout << "in while 3\n";  
    std::cout << "x = " << x << '\n';  
}  
```

執行到這個迴圈時 x 的值已經為 0 了，在疊代開始前會先去計算 condition 裡的 expression，因此每次迴圈開始前 x 迴遞增 1，當結果大於等於 3 時 expression 會回傳 `false`，所以這個迴圈會執行 2 次，但 expression 被計算了 3 次，跳出回圈後 x 的值為 3。  

```cpp
x = 0;  
while(++x) {  
    if(x == 6)  
        continue  
    else if(x == 8)  
        break;  

    std::cout << "in while 4, x = " << x << '\n';  
}  
```

在迴圈開始前我先把 x 賦值為 0 了，每次迴圈開始前 x 先遞增 1，而在 while 的 body 裡面有一個 if-else statement，當 `x == 6` 時會跳過此次疊代，直接開始下一次疊代，而當 `x == 8` 時，會直接結束這個迴圈，因此這段迴圈裡 x 的輸出結果為 1 2 3 4 5 7。  

### do-while loop  

語法長這樣：  

> do statement while ( condition ) ;		  

寫起來像這樣：  

```cpp
do   
    statement  
while (condition)  
```

與 while 迴圈類似，差別是 do-while 的 statement 至少會執行一次，就算 expression 計算結果的 bool 值為 `false` 也一樣，  

換句話說，這裡的 statement 會先被執行一次，再去像一般的 while 迴圈判斷，舉個例子：  

```cpp
#include <iostream>  

int main() {  
    int x = 5;  

    do  
        std::cout << "in while 1\n";  
    while(false);  

    do  
        std::cout << "in while 2\n";  
    while(--x > 0);  

    return 0;  
}  
```

同樣地我們一個個來看：  

```cpp
do  
    std::cout << "in while 1\n";  
while(false);  
```

因為 do-while 的 statement 會先執行一次，所以這裡的 "in while 1\n" 會被輸出，之後再去判斷 condition，計算結果為 `false`，所以跳出迴圈。  

```cpp
do  
    std::cout << "in while 2\n";  
while(--x > 0);  
```

do-while 會先執行一次 statement，然後再去判斷，所以這個迴圈的 body 總共會被執行 5 次，condition 被計算了 5 次。  

### for loop  

語法長這樣：  

> for ( init-statement condition(opt) ; iteration-expression(opt) ) statement  

寫起來長這樣  

```cpp
for(init-statement codition; iteration-expression)  
    statement  
```

init-statement 可以是 expression statement、simple declaration、alias declaration。  

condition 同上面 if-statement 的 codition，可以是 expression 或宣告，<span class = "yellow">如果沒寫的話會當作 `true` 來判斷</span>。  

iteration-expression 可以放任何地 expression，每次結束疊代時都會執行這個 expression，執行完再去判斷 condition。  

也就是說 for loop 大致上與   

```cpp
{  
	init-statement  
	while ( condition ) {  
		statement  
		iteration-expression ;  
	}  
}  
```

等價，但不是完全等價，有些狀況下兩者不能互通，如 while loop 裡面的 codition 不能為空。  

簡單舉幾個 for loop 的例子：  

```cpp
#include <iostream>  

int main() {  
    int x = 0;  

    for(; false;)  
        std::cout << "in while 1\n";  

    for(; x < 3; ++x)  
        std::cout << "in while 2\n";  

    for(int counter = 0; counter < 5; ++counter) {  
        --x;  
        std::cout << "x = " << x << '\n';  
    }  
}  
```

同樣地，一個個來看：  

```cpp
for(; false;)  
    std::cout << "in while 1\n";  
```

這個 for loop 的 init-statement 是一個 null statement，condition 的計算結果是 `false`，每次疊代結束不做任何事，因為一開始 condition 的計算結果就是 `false` 了，所以不會進入迴圈的 statement。  

```cpp
for(; x < 3; ++x)  
    std::cout << "in while 2\n";  
```

這個 for loop 的 init-statement 是一個 null statement，condition 為 expression `x < 3`，所以當 x 大於等於 3 時會離開迴圈，每次疊代結束會執行 `++x`，因此這個迴圈的 statement 會執行 3 次，`++x` 也會執行 3 次。  

```cpp
for(int counter = 0; counter < 5; ++counter) {  
    --x;  
    std::cout << "x = " << x << '\n';  
}  
```

這個 for loop 的 init-statement 是一個 simple-declaration，condition 為 `counter < 5`，所以當 counter 大於等於 5 的時候會離開迴圈，每次疊代結束會使 counter 遞增 1，因此這個迴圈的 statement 會執行 5 次。  

init-statement 中宣告的變數的 scope 與 for loop statement 的 scope 是一起的，也就是說你不能這樣做：  

```cpp
for(int i = 0;;)  
    int i = 2; // error: redeclaration of `int i`  
```

`i` 這個 name 在同一個 scope 內被宣告了兩次，而前面有提到同一個變數名只能在不同 scope 出現。  

> n4868(6.4.3) : Names declared in the init-statement, the for-range-declaration, and in the condition of if, while, for, and switch statements are local to the if, while, for, or switch statement (including the controlled statement), and shall not be redeclared in a subsequent condition of that statement nor in the outermost block (or, for the if statement, any of the outermost blocks) of the controlled statement.  

## 跳躍陳述句 Jump statements  

Jump Statements 可以無條件的改變程式執行的順序，有 4 種：`break` statement、`continue` statement、`return` statement、`goto` statement。  

前兩個是用來控制迴圈的流程使用的，上面講迴圈時已經介紹了，`return` statement 會在之後講 function 時介紹，而 `goto` statement 會在後面講 Labeled Statements 時介紹。  

## 標記陳述句 Labeled Statements  

標記陳述句用來製造一個 label ，label 可以拿來幫助控制程式的執行順序，label 主要是給兩種 statement 看的，一是 `goto`(一種 jump statement)，二是 `switch`(一種 selection statement)。一個 statement 裡面可能會有很多個 label。  

語法有三種：  

> identifier : statement	(1)	  
> case constexpr : statement	(2)	  
> default : statement (3)	  

可以看見後面都需要接 statement，但如果在邏輯上不需要，就可以使用 null statement 來幫忙。  

### goto (一種 jump statement)  

(1) 是給 `goto` 看的，同一個 function 內的 lable `goto` 都看的到，且在宣告前也可以看的到，function 後面的章節會教到，這邊可以先當作 `main` function 裡面。  

舉個例子：  

```cpp
#include <iostream>  

int main()  
{  
  goto end;    // 可以在宣告前使用  

begin:  
  std::cout << "begin\n";  
end:  
  std::cout << "end\n";  
}  
```

這裡我建了兩個標籤：`begin` 與 `end`，後面接冒號，這段是 labeld statement，而冒號後面還要再接一個 statement，我這裡兩邊都接了一個 `std::cout << ...;` (一種  expression statement)。  

這個程式的輸出結果只會有一個 "end"，一開始程式是循序執行的，因此進入 main function，隨後執行到第五行時看見了 `goto` 關鍵字，`goto` 會讓程式去執行後面標籤標記的地方，以這裡來說就是 `end` 這個標籤，因此後面的 `std::cout << "begin\n";` 就被跳過了。  

`goto` 可以跳到同一個 function 內的同一個地方，無視 scope，但不能跳過變數的宣告，除非這個變數符合下面其中一個條件：  

1. 變數的型態是 scalar type 且沒有初始化器  
2. 變數的型態是一個有 trivial default constructors and destructor 的 class type，且沒有初始化器  
3. 上面兩個的 cv-qualified 版本  
4. 元素為 1、2 點的變數的 array  

延伸閱讀：[What is a scalar Object in C++?](https://stackoverflow.com/questions/14821936/what-is-a-scalar-object-in-c)  

這些規則適用於所有<span class = "yellow">會轉移程式控制權</span>的語句，下面的 `switch` 語句也適用，下面這個例子編譯不會過，因為變數的宣告有初始化器：  

```cpp
#include <iostream>  

int main()  
{  
  goto end;  
begin:  
  int a = 5;  
  std::cout << "begin\n";  
end:  
  std::cout << "end\n";  
}  
```

但如果有這樣的需求，可以利用 compound statement 來完成，例如：  

```cpp
#include <iostream>  

int main()  
{  
  goto end;  
begin : {  
  int a = 5;  
  std::cout << "begin\n";  
}  
end:  
  std::cout << "end\n";  
}  
```

這樣就可以編譯過了。  

### switch (一種 selection statement)  

(2) 與 (3) 是給 `switch` 看的，語法是   

> switch ( init-statement(opt) condition ) statement  

要注意的是這裡的 init-statement 在 C++17 後才能使用，它可以是 expression statement、simple declaration、alias declaration。  

而 condition 的部分則是要放一個型態是 int、enumeration 或是一個可以透過 Contextual conversions 轉換為 int、enumeration type 的 expression。  

statement 的部分通常會放一個 compound statement，而對於 `switch` 來說，`case:` 和 `default` 這兩個 label，還有 `break` statement(一種 jump statement) 都有特殊的意義。  

switch 的 body 可以有任意數量的 `case:` 標籤，只要 `case` 後面接的 constant-expression 沒有重複就好，但最多只能有一個 `default` 標籤，  

當 condition 裡面放的 expression 計算的結果與 `case:` 後面接的 constant-expression 結果一樣，那麼就會跳到那個 `case` 的部分執行；而如果都沒一樣的，但有 `default` 標籤，那會跳到 `default` 的部分執行；如果也沒有 `default` 標籤，那會直接離開這個 `switch`。  

另外還需要使用 `break` statement 離開 `switch`，舉個例子：  

```cpp
#include <iostream>  

int main()  
{  
  int x = 1;  
  switch (x) {  
  case 0:  
    std::cout << "0\n";  
  case 1:  
    std::cout << "1\n";  
  case 2:  
    std::cout << "2\n";    // 這個也會被執行到，因為沒有寫 break  
    break;  
  case 3:  
    std::cout << "3\n";    // 這行就不會了，因為上面有 break 了  
  }  
}  
```

expression `x` 計算的結果是 1，和 `case 1:` 一樣，因此程式會跳到 `case 1:` 後面<span class = "yellow">繼續循序執行</span>，也就是 `std::cout << "1\n";` 的部分，如果想要提早離開 `switch`，則需要利用 `break` statement 離開，像是上例中 case 2 裡面那樣。  

同樣的不能跳過變數的宣告，除非符合上面 `goto` 裡面提到的四個條件，因此下面這樣編譯不會過：  

```cpp
#include <iostream>  

int main()  
{  
  int x = 1;  
  switch (x) {  
  case 0:  
    int a = 2;  
    std::cout << "0\n";  
  case 1:  
    std::cout << "1\n";  
  }  
}  
```

因為 `int a = 2;` 這裡的 `a` 有初始化器，同樣的，如果有這樣的需求，可以利用 compound statement 來完成：  

```cpp
#include <iostream>  

int main()  
{  
  int x = 1;  
  switch (x) {  
  case 0: {  
    int a = 2;  
    std::cout << "0\n";  
  }  
  case 1:  
    std::cout << "1\n";  
  }  
}  
```

