---
title: 礦坑系列 ── Indirect through null pointer
date: 2023-08-12
tag: C++ Miner-BlackMagic
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50><br>Indirect through null pointer</center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

## 前言

事情起於 jserv 的講義裡面有個 `&((data*)0)->c)` 這樣的操作，作用是求 `c` 在 `data` 這個 struct 中的偏移量，但那個 `0` 實在是讓我覺得很不順眼，為了確定他到底是不是 UB，我翻了一個多禮拜的 standard 與 committee paper，甚至翻了 CWG issue 和 Standard Defect Report，才總算是有個結果 (翻到快吐了)

雖然問題本身莫名其妙變得很複雜，但其實結論很簡單：因為 standard 沒有要求，所以是 UB

:::info  
更：在 2023-11-06 時標準將其 specify 為 UB 了，但本文仍能很好的幫助你理解其歷史與判斷 UB 的脈絡，更新我會將其補在最後面，如果你只想看原因那可以直接跳到最後面觀看  

另外，像是這種網址 `https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232`，因為他是官方文件所以會把所有東西都丟到同一個頁面，不好閱讀，你可以手動改成非官方的 mirror：`https://cplusplus.github.io/CWG/issues/232.html`，這邊是以 issue 為單位整理成頁面的，比較好閱讀一點  
:::

不過在翻的過程，找到了一些有趣的討論，所以這邊就記錄一下為何 `&((data*)0)->c)` 是 UB，與為何 *Indirect through null pointer* 會有是不是 UB 的疑慮

註：本文的標準將以 n4861(C++20) 內的定義為主  
註2：本文討論的內容都假設 class type 為 standard layout type，非 standard layout type 不在本文討論範圍內，相關內容詳見 [offsetof](https://en.cppreference.com/w/cpp/types/offsetof?fbclid=IwAR0qW2nZ1MX3ZPlmDXROTMNxB4fY_1Ba637-j_6ew_RAU2-T7HyshlQ-QQs#:~:text=offsetof%20cannot%20be%20implemented%20in,specified%20by%20C%20DR%20496)

## Implicit Undefined Behavior?

Implicit Undefined Behavior 是口語的說法，不是標準內的用語，其表達的意義就如前言所說：

> standard 沒有要求的東西歸為 UB

C 和 C++ 都有於 standard 內列下這個規則，下面是 C 與 C++ 對於 undefined behavior 的定義：

> [c17#3.4.3.p1](https://cigix.me/c17#3.4.3.p1): behavior, upon use of a nonportable or erroneous program construct or of erroneous data, for which this <span class = "yellow">International Standard imposes no requirements</span>

> [n4861(defns.undefined)](https://timsong-cpp.github.io/cppwp/n4861/defns.undefined#sentence-1)：behavior for which this document imposes no requirements  
> [ Note: <span class = "yellow">Undefined behavior may be expected when this document omits any explicit definition of behavior or when a program uses an erroneous construct or erroneous data.</span> Permissible undefined behavior ranges from ignoring the situation completely with unpredictable results, to behaving during translation or program execution in a documented manner characteristic of the environment (with or without the issuance of a diagnostic message), to terminating a translation or execution (with the issuance of a diagnostic message). Many erroneous program constructs do not engender undefined behavior; they are required to be diagnosed. Evaluation of a constant expression never exhibits behavior explicitly specified as undefined in [intro] through [cpp] of this document ([expr.const]). — end note ]

也就是說，如果我們要說一個 behavior 是 UB，那麼推導的過程是：

1. 找出該 behavior 在標準內的定義，若其有在標準內被定義為 well-defined、undefined、unspecified 或 implementation-defined behavior，那它就屬於標準內定義的那類
2. 否則，其為 undefined behavior，這被稱為 implicit undefined

一般來說會使用第一點，畢竟 C++ 明確定義的東西很多，也會希望能明確表明其為 undefined，不過這次使用的是第二點

某種程度上這類 UB 屬於 open issue，很多的 issue 都圍繞這類行為在做討論，這也會導致 compiler 實作上的困難：[Over-aggressively optimization on infinite loops](https://github.com/llvm/llvm-project/issues/60622#issuecomment-1426860735)

另外，在實作上，*"Technically UB but it’ll always do what you expect"* 的狀況也是有的

## 問題討論

對於 `&((T*)NULL)->member` 這個行為，重點在於中間的 `((T*)NULL)->member`

要推出這段 code 是 UB，你有三條路徑可以選擇：

1. pointer arithmetic 的路徑
2. standard 內對於 `*` 與 `->` operator 操作定義的路徑
3. 找不到所以 UB (Implicit Undefined Behavior)

看了非常多篇的 stackoverflow，這三條都有人用

我們先看看前兩條，試著找出標準內能套用到這個 behavior 上的定義

#### pointer arithmetic 的路徑

我們先談一下 pointer arithmetic 的 UB 行為，看以下的例子：

```c
int i = 0;
int *p = &i;
p += 1;    // p = &i[1]; okay, well-defined

int *p2 = NULL;
p2 += 1;    // p2 = &NULL[1]; invalid, NULL is not an object
```

> [c17#6.5.6.p7](https://port70.net/~nsz/c/c11/n1570.html#6.5.6p7)：For the purposes of these operators, a pointer to an object that is not an element of an array behaves the same as <span class = "yellow">a pointer to the first element of an array of length one with the type of the object as its element type</span>.

這裡說的是，在進行 pointer arithmetic 的時候，如果指標指向的東西不是 array 的 member，那麼指標的行為會和「指向長度為 1 的陣列的第一個元素的指標，其元素類型為指向的物件的類型」相同

換句話說，`&( ((int*)0) + 1 )` 也是 UB

而通常 compiler 會利用 `pointer arithmetic` 的操作來實作 `->`，<span class = "yellow">然而這並沒有在標準內被 specified</span>，因此以語言面來說，這個理由並不能套用到 `((T*)NULL)->member` 上

但對於實務層面(compiler 的視角) 來說，這是一個說明它 *invalid* 的好原因。 另外因為還有一個 address-of operator 在外面，編譯器產生的代碼通常不會有 dereference 的步驟，所以這是一個經典的 *"Technically UB but it’ll always do what you expect"* 的情況

#### `*` 與 `->` 的路徑

C++ 中有個條款明確定義了 `->` 與 `*` 在語言上的等價性：

> [n4861(expr.ref#2)](https://timsong-cpp.github.io/cppwp/n4861/expr.ref#2)：For the first option (dot) the first expression shall be a glvalue. For the second option (arrow) the first expression shall be a prvalue having pointer type. <span class = "yellow">The expression `E1->E2` is converted to the equivalent form `(*(E1)).E2`</span>; the remainder of [expr.ref] will address only the first option (dot).

因此我們可以將 `((T*)NULL)->member` 寫成 `(*((T*)NULL)).member`

再來 `*` 的條款說：

> [n4861(expr.unary.op#1)](https://timsong-cpp.github.io/cppwp/n4861/expr.unary.op#1.sentence-1)：The unary `*` operator performs indirection: the expression to which it is applied shall be a pointer to an object type, or a pointer to a function type and the result is an <span class = "yellow">lvalue referring to the object or function to which the expression points</span>.

而 `NULL` 不是 object 也不是 function，所以 `*NULL` 不在標準要求內，換句話說 `NULL` 被標準漏掉了，並沒有被 specify，因此為 implicit UB

<span class = "yellow">但在 C 的標準中並沒有定義 `->` 與 `*` 的等價性，所以這條是只有 C++ 能走的路線</span>

#### 結論

剛剛說有三條路線

對於 C，前兩個都不通，標準中也沒有特別定義此行為，因此為 Implicit Undefined Behavior

對於 C++，無論用第一還是第二條，也都是 Implicit Undefined Behavior。 如果覺得這兩條不該套用在這個情況，那因為標準中也沒有其他地方有相關定義了，因此還是 Implicit Undefined Behavior

總而言之，雖然 `((T*)NULL)->member` 沒有直接在標準中被列為 UB，但一定有條路線是可以推出它是 UB 的，所以此 behavior 為 UB 是確定的，但原因則有許多可能

然而原則上來說我們仍希望 UB 可以被 explicit specified，畢竟像這樣可能有多種原因的例子並不好，可能會有一些衍生的 issue 出現，接下來就看看 C++ 中的例子 [CWG issue #232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232)

## Indirect through null pointer?

去年(2022) 12 月，又有人在 CWG 的 github 發了 [issue](https://github.com/cplusplus/CWG/issues/198)  問 [CWG issue #232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232) 的結果，可見 _Indirect through null pointer_ 為 UB 的原因時至今日都還未被完全解決

個人推測是因為在實務上不是那麼重要，但在 standard 中卻是類似於憲法的存在，所以雖然有 issue，但改起來麻煩，一開始改就會有一大串的東西被牽動，所以才從 2004 年到現在都還沒有個結論

這是 C++ committe 成員在那篇 [github issue](https://github.com/cplusplus/CWG/issues/198) 留的原文：

> I would really welcome the suggested change here. This bit in the standard is a topic of recurring questions, and it always takes unfortunately long to explain that a null pointer does not point to an object, and hence dereferencing it is undefined behavior. We have a similar problem at [expr.ref]/6.2, where we say "the expression designates the corresponding member subobject of the object designated by the first expression". There we don't (bother) say(ing) that if no object is designated, we are in the land of UB. That tends to take even longer to explain than the unary operator*. Even if we end up making mere operator* of a null pointer well-defined (See https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#232), in all likelihood we'll still (continue to) require that the LHS of an operator-> must be non-null, and questions about that will continue. Unless we just clarify it, along the lines of what's been suggested here.

在 stackoverflow 的討論中我看見了一個不錯的[例子](https://stackoverflow.com/questions/28482809/c-access-static-members-using-null-pointer/28483477)，實務上是有可能遇到的，所以這邊先貼出來給你思考一下

```cpp
#include <iostream>
class demo {
public:
  static void fun()
  {
    std::cout << "fun() is called\n";
  }
  static int a;
};

int demo::a = 9;

int main()
{
  demo *d = nullptr;
  d->fun();
  std::cout << d->a;
  return 0;
}
```

`demo` 這個 class 裡面有一個 static member `a` 與一個 static member function `fun`，在 `main` 中有一個 pointer `d`，型態為 `demo*`，值為 `nullptr`

問題來了，`d->fun()` 合法嗎?

## `->` 與 `*` 

前面有提到在 C++ 中 `->` 與 `*` 有這樣的等價關係：

> [n4861(expr.ref#2)](https://timsong-cpp.github.io/cppwp/n4861/expr.ref#2)：For the first option (dot) the first expression shall be a glvalue. For the second option (arrow) the first expression shall be a prvalue having pointer type. <span class = "yellow">The expression `E1->E2` is converted to the equivalent form `(*(E1)).E2`;</span> the remainder of [expr.ref] will address only the first option (dot)

這邊主要講了三件事：

1. `E1.E2` 的 `E1` 需要為 glvalue
2. `E1->E2` 的 `E1` 需要為 prvalue
3. `E1->E2` 等價為 `(*(E1)).E2`

所以接下來 `d->fun()` 我都會將其寫成 `(*d).fun()`，因此關鍵的部分在前面的 `(*d)`，因為這邊的 `d` 是個 `nullptr`


這邊複習一下，expression 通常有兩個特性
1. evaluated
    可能有 return 值，可能會被丟棄(如棄值表達式 Discarded-value expressions)
2. side-effect
    可能有，可能沒有

這裡講得比較通俗好懂，原文是這樣的：

> [n4861(expr#pre-1)](https://timsong-cpp.github.io/cppwp/n4861/expr#pre-1)[ Note: [expr] defines the syntax, order of evaluation, and meaning of expressions. An expression is a sequence of operators and operands that specifies a computation. <span class = "yellow">An expression can result in a value and can cause side effects.</span> — end note ]

好，那我們看一下 `->` 的操作定義：

> [n4861(expr.ref#1)](https://timsong-cpp.github.io/cppwp/n4861/expr.ref#1)：A postfix expression followed by a dot . or an arrow ->, optionally followed by the keyword template ([temp.names]), and then followed by an id-expression, is a postfix expression. <span class = "yellow">The postfix expression before the dot or arrow is evaluated;</span> the result of that evaluation, together with the id-expression, determines the result of the entire postfix expression.

這邊講的事很簡單，`E1->E2` 的 `E1` 會被 evaluate，所以 `d->fun()` 中的 `d` 會被 evaluated，換句話說，`(*d).fun()` 中的 `*d` 會被 evaluated

`*d` 是 class member access expression，屬於棄值表達式，我們來看一下標準內有關棄值表達式的描述

> [n4861(stmt.expr#1)](https://timsong-cpp.github.io/cppwp/n4861/stmt.expr#1)：The expression is a discarded-value expression. <span class = "yellow">All side effects from an expression statement are completed before the next statement is executed.</span> An expression statement with the expression missing is called a null statement. [ Note: Most statements are expression statements — usually assignments or function calls. A null statement is useful to carry a label just before the } of a compound statement and to supply a null body to an iteration statement such as a while statement. — end note ]

因此如果 `*d;` 這個 statement 沒問題，那 `(*d).fun();` 自然沒問題，因此 `d->fun();` 沒問題

## 標準自己用了 indirect through null pointer?

[CWG issue#232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232) 是 2004 年發起的 issue，標題內容大大的就打了「Is indirection through a null pointer undefined behavior?」

討論的問題主要是說標準內有些舉例說這是 UB，但又有 well-defined 的例子，造成了歧異

我們現在就來看看這個 well-defined 的例子，要注意的是它被保留下來了，也就是說標準曾認定 indirection through null pointer 不全為 UB

issue 內有貼上了當年 spec 內使用的段落：

> If the lvalue expression is obtained by applying the unary * operator to a pointer and the pointer is a null pointer value (7.3.12 [conv.ptr]), the typeid expression throws the bad_typeid exception (17.7.5 [bad.typeid]).

對應到 n4861 的內容：

> [n4861(expr.typeid#2)](https://timsong-cpp.github.io/cppwp/n4861/expr.typeid#2)：When typeid is applied to a glvalue whose type is a polymorphic class type ([class.virtual]), the result refers to a std​::type_­info object representing the type of the most derived object ([intro.object]) (that is, the dynamic type) to which the glvalue refers. If the glvalue is <span class = "yellow">obtained by applying the unary * operator to a pointer and the pointer is a null pointer value</span> ([basic.compound]), the typeid expression throws an exception ([except.throw]) of a type that would match a handler of type std​::​bad_­typeid exception ([bad.typeid]).

我們看個例子：

```cpp
#include <iostream>
#include <typeinfo>

int main()
{
  try {
    // Polymorphic type
    class A {
      virtual ~A() {}
    };

    typeid(*((A *) 0));

  } catch (std::bad_typeid) {
    std::cerr << "bad_exception\n";
  }
}
```

這個便是標準中給的 well-defined 例子，在這裡我們會 catch 住 `std::bad_typeid` 進而輸出 `bad_exception`

這邊 `*((A*)0)` 為 lvalue expression，根據標準，在 `typeid` 中它會 evaluated 並導致 `std::bad_exception`

> [n4868(expr.typeid#3)](https://timsong-cpp.github.io/cppwp/n4861/expr.typeid#3)：<span class = "yellow">When typeid is applied to an expression other than a glvalue of a polymorphic class type, the result refers to a std​::​type_­info object representing the static type of the expression.</span> Lvalue-to-rvalue, array-to-pointer, and function-to-pointer conversions are not applied to the expression. If the expression is a prvalue, the temporary materialization conversion is applied. The expression is an unevaluated operand.

## CWG issue\#232 的結論 (C++ 26 前)

最後這個 issue 討論的結果為

> `p = 0; *p;` is <span class = "yellow">not inherently an error</span>. An lvalue-to-rvalue conversion would give it undefined behavior.

丟給了 lvalue-to-rvalue 轉換來處理，簡單來講就是沒有真正需要讀值的需求就沒事，用英文來說的話就是指 "The identity of the object is not required"，你不要直接寫 `*0` 這樣讓它一定要讀值就 ok

對此，[CWG issue#315](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html?fbclid=IwAR0rntH_qBbUXw0Gh0evoaNY6mv04hXsOk-qkhDEc6axL2IUMGRZTP97GEU#315) 內明確表示了此處 `*d` 不會有 lvalue-to-rvalue conversion：

> Rationale (October 2003):  
> We agreed the example should be allowed. `p->f()` is rewritten as `(*p).f()` according to 7.6.1.5 [expr.ref]. `*p` is not an error when `p` is null unless the lvalue is converted to an rvalue (7.3.2 [conv.lval]), which it isn't here.

換句話說，在 C++26 以前這些都是合法的：

```cpp
T* p = nullptr;
*p;
int* q = &(*p);
(*p).static_member();
```

但在 C++26 後為 UB，等等會提及為什麼

然而在 C++23 的期間(2023-05-10 以前)，[CWG issue#232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232) 的結果仍<span class = "yellow">未被整入標準</span>，雖有[論文](http://arcoth.github.io/Proposals/EmptyLvalues.html)在進行中，裡面更嚴謹的討論了除了這個 issue 以外的狀況(issue 內提出了一個稱為 `empty lvalue` 的概念，但還未加進標準)，但終究是進行中(那篇論文也有一段時間了，過了那麼久感覺是沒審過)，因此雖然我們知道應該要是可行的，但目前仍是 UB(一樣 by Implicit Undefined Behavior)

[CWG issue#232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232) 還提到 null reference 是 UB：

> [n4861(dcl.ref#5)](https://timsong-cpp.github.io/cppwp/n4861/dcl.ref#5)：There shall be no references to references, no arrays of references, and no pointers to references. The declaration of a reference shall contain an initializer ([dcl.init.ref]) except when the declaration contains an explicit extern specifier ([dcl.stc]), is a class member ([class.mem]) declaration within a class definition, or is the declaration of a parameter or a return type ([dcl.fct]); see [basic.def]. A reference shall be initialized to refer to a valid object or function. [ Note: In particular, a null reference cannot exist in a well-defined program, because the only way to create such a reference would be to <span class = "yellow">bind it to the “object” obtained by indirection through a null pointer, which causes undefined behavior.</span> As described in [class.bit], a reference cannot be bound directly to a bit-field. — end note ]

另外，在 C++98 的 spec 中舉了 dereference null pointer 為 UB 的例子：

> n1146(intro.execution#4)：Certain other operations are described in this International Standard as undefined (<span class = "yellow">for example, the effect of dereferencing the null pointer</span>). [Note: this International Standard imposes no requirements on the behavior of programs that contain undefined behavior. ]

但這個例子在 2010 [CWG issue#1102](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1102) 討論後<span class = "yellow">被改掉了</span>：

> [n3337(intro.execution#4)](https://timsong-cpp.github.io/cppwp/n3337/intro.execution#4)：Certain other operations are described in this International Standard as undefined (for example, the effect of attempting to <span class = "yellow">modify a const object</span>). [ Note: This International Standard imposes no requirements on the behavior of programs that contain undefined behavior.  — end note ]

issue#1102 中給的理由是

> There are core issues surrounding the undefined behavior of dereferencing a null pointer. It appears the intent is that dereferencing is well defined, but using the result of the dereference will yield undefined behavior. This topic is too confused to be the reference example of undefined behavior, or should be stated more precisely if it is to be retained.

簡單來說就是這個問題還在討論中，不要用這個當例子

## CWG issue#2823

在 2023-11-08，這件事終於迎來了轉機，讓我回來更新了這篇文章XD

在 [CWG issue#2823](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#2823) 中，committee 修改了 unary * operator 的條例：

> The operator yields an lvalue of type T <span class = "deletion">~~denoting the object or function to which the operand points~~</span>. <span class = "addition">If the operand points to an object or function, the result denotes that object or function; otherwise, the behavior is undefined except as specified in 7.6.1.8 [expr.typeid].</span>

很清楚的直接寫說不是 object 或 function 的情況是 UB 了(除了 typeid 的情況)

目前官方態度是以此更動為準，一切對 null pointer 的 dereference 都是 UB (除了提到的 `expr.typeid`)，在 [Mq 白於「360 安全规则集合」建立的 issue 中](https://github.com/Qihoo360/safe-rules/issues/52)，有人直接到 std-discussion 問了(讓你寄信問 committee 的平台)，回復如下：

> That issue is more than 20 years old.  
> Meanwhile, the direction of CWG has changed towards a uniform treatment of null pointer values.

我還把他的信挖出來了，[連結在這](https://lists.isocpp.org/std-discussion/2024/06/2558.php)

而 [CWG issue#232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232) 和 [CWG issue#315](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#315) 的狀態都變為 NAD 了(關閉了)，至此事件基本上就有個結果啦~~

:::info  
如果你仔細看 [CWG issue#2823](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#2823) 的狀態，其為 DRWP，意思是僅供參考，不能將其當作標準。 因此最後我們還是要來看標準，而 spec 已經在 [2023-11-14 的 PR](https://github.com/cplusplus/draft/pull/6684) 中將其合入了(`Fixes cplusplus/CWG#198` 那行)

只是因為 C++23 的 draft 最後一更是在 2023-05-10，因此這要等到 C++26 的 spec 發布後才可以看見了，目前可以在[最新的 draft](https://eel.is/c++draft/expr.unary.op#1.sentence-4)看見該片段，沒意外的話就會變標準了  
:::

## 參考連結

1. [Dereference null is not always UB?](https://stackoverflow.com/questions/43533262/dereference-null-is-not-always-ub)
2. [c++ access static members using null pointer](https://stackoverflow.com/questions/28482809/c-access-static-members-using-null-pointer/28483477)
3. [CWG issue#232](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#232)
4. [CWG issue#315](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#315)
5. [CWG issue#342](http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#342)
6. [CWG issue#1102](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1102)
7. [CWG issue#2823](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#2823)
8. [What are the rules for a valid dereferencing of a null pointer?](https://stackoverflow.com/questions/64167285/what-are-the-rules-for-a-valid-dereferencing-of-a-null-pointer)
9. [Implicit Undefined Behavior in C](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2248.pdf)
10. [X3J11 DR#017](https://www.open-std.org/jtc1/sc22/wg14/www/docs/dr_017.html)
11. [Is dereference null pointer UB in C++20?](https://stackoverflow.com/questions/76894031/is-dereference-null-pointer-ub-in-c20)
12. [CWG232 [expr.unary.op] p1 should explicitly specify the behavior if the expression does not refer to the object or function ](https://github.com/cplusplus/CWG/issues/198)
13. [cpp-ub-list 55. 空指针解引用](https://github.com/Qihoo360/safe-rules/issues/52)