---
title: 礦坑系列 ── 序
date: 2021-05-14
tag: C++ Miner-main
category: C++ Miner
star: true
---

# 礦坑系列 ── 序

## 前言 

這是一本介紹 Modern C++ 與一些 C++ 深度解析的書，從我的 hackmd 搬運而來。 如果您是 C++ 新手，我會建議您先去看 Miner Tutorial 的部分，裡面對初學者會學到的東西做了簡單的介紹，可以幫助大家理解、回憶一些基礎知識，同時可以讓你習慣本書風格

讀著讀著便來到了 Modern C++，語法和文法比以往的 C、C++03 複雜很多，而且在找文章閱讀時發現網路上中文的資源其實沒有想像中的多，受到 Jserv 老師「你所不知道的 C 語言系列」啟發，想著那就自己來寫吧！ 於是就有了這個礦坑系列

文章的內容主要來自 Cppreference、draft 或一些好文章的翻譯，可以的話會把語法底層的一些原理也寫進去，帶大家看一下 llvm 的實作，可能會碰到一點編譯器和組語，希望能夠寫得盡量詳細，如果有什麼想看的，或發現有錯字之類的也都歡迎開 issue 或 PR

而取叫礦坑的原因是因為在寫這類文章的時候要挖的東西實在是太多了XD 常常需要挖到 C++ 會議的討論紀錄之類的，像是礦坑那樣深不見底，而想要的東西像鑽石一樣十分難找，自己就像個礦工一樣不停地往內部探索，所以就取叫礦坑系列啦!

感謝一路上教我許多東西的老師、朋友與 Stackoverflow、Discord 上那些不認識但十分熱心的人們。 還有要感謝林信良老師的文章，我的整個資工之路是從林信良老師的 C++ 系列文開始的

由於 hackmd 改了圖片庫，導致後來 github repo 上的圖片連結與 hackmd 上的無法共用，所以後來慢慢將本書改成了以此 repo 為主，放在我自己的 Blog 上也比較好閱讀

- [github repo 連結](https://github.com/Mes0903/Cpp-Miner)
- [Blog 連結](https://mes0903.github.io/Cpp-Miner/)

另外由於原則上這是給也是給我自己看的筆記，主要都是放在自己的 Blog 上，所以 markdown 中會有一些給我 Blog 用的 CSS 或 tag

預計是會有兩個 branch：`main` 與 `my-blog`，後者用於我的 blog 上，我更新時也會先丟到這個 branch，如果想看一下 WIP 的文章可以到那邊看看。 後來發現寫一篇都要很久，所以就佛系更啦

本 repo 採用 MIT License，你可以拿去自由使用、修改等，只要附上本 repo 的來源即可

## Summary 文章一覽 

### Miner Tutorial 新手教學

此處為新手教學，可以幫助大家回憶以前的東西，並幫助理解基礎 C++ 與熟悉本書風格

- [淺談計算機概論](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Computer_Introduction)
- [Windows 內的 C++ 與 Vscode 環境建置](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Environment_Building) 
- [Object、Expression 與 Statement](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Object_Expression_Statement)
- [Array 與 Pointer](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Array_Pointer) 
- [Function 與 Memory](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Function_Memory) 
- [Class](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Class)
- [Class 與物件導向](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/OO)

### Cpp Miner 礦坑系列本文

- [Value Categories 值類別](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Value_Categories)
- [（WIP）malloc、new 與 POD Type](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/malloc_new_POD)
- [（WIP）std::function](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Std_Function)
- [Structured Binding 結構化綁定](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Structured_Binding)
- [Concept、SFINAE 與 Detection Idiom](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Concept_SFINAE_DetectionIdiom)
- [（WIP）Dependent Name 與 Name lookup](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Dependent_Name)
- [Allocator & PMR](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Allocator_PMR)

### 礦坑內的黑魔法

這邊主要拿來放一些有趣的小東西，實不實用另說XD

- [用 "explicit" 來偵測複製](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/Explicit_Detect_Copy)
- [Small String Optimization](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/SSO)
- [四個不該用 "const" 的時機](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/NoConst)
- [Indirect through null pointer 是 UB 嗎?](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/Indirect_through_null_pointer)

## 一些資源

下面是一些 C/C++ 的 spec：

C/C++ Language References (final/current working drafts)

- C89：(沒有 PDF 版本)、[HTML](http://port70.net/~nsz/c/c89/c89-draft.html)
- C99 (N1256)：[PDF](http://port70.net/~nsz/c/c99/n1256.pdf)、[HTML](http://port70.net/~nsz/c/c99/n1256.html)
- C11 (N1570)：[PDF](http://port70.net/~nsz/c/c11/n1570.pdf)、[HTML](http://port70.net/~nsz/c/c11/n1570.html)
- C17 (N2176)：[PDF](https://files.lhmouse.com/standards/ISO%20C%20N2176.pdf)、(沒有 HTML 版本)
- C23：[PDF](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2596.pdf)、(沒有 HTML 版本)
- C++ 11 (N3337)：[PDF](http://open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf)、[HTML](https://timsong-cpp.github.io/cppwp/n3337/)
- C++ 14 (N4140)：[PDF](https://timsong-cpp.github.io/cppwp/n4140/draft.pdf)、[HTML](https://timsong-cpp.github.io/cppwp/n4140/)
- C++ 17 (N4659)：[PDF](http://open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf)、[HTML](https://timsong-cpp.github.io/cppwp/n4659/)
- C++ 20 (N4861)：[PDF](http://open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4861.pdf)、[HTML](https://timsong-cpp.github.io/cppwp/n4861/)
- C++ 23 (N4950)：[PDF](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf)、[HTML](https://timsong-cpp.github.io/cppwp/n4950/)

你可以在 [Cppreference](https://en.cppreference.com/w/cpp/links) 看到這些資訊，我的閱讀方式是 Drafts 配 Cppreference 來看，然後上網找例子或自己想一些例子來驗證，如果有不懂的就會到 Google 和 Stackoverflow 搜尋，但就如我前面所說的，比較新的東西通常中文資源很少，不太會有什麼文章。不過據我所知，Drafts 這樣密密麻麻的英文字對許多人來說並不是很友善，所以我的方法可能並不適合你，希望你可以找到自己的方法

那如果一直找不到 (可能是比較深或偏向英文方面意思理解有困難等等)，那我就會到 Discord 群裡面發問，發問的方式可以參考 [How To Ask Questions The Smart Way](https://github.com/ryanhanwu/How-To-Ask-Questions-The-Smart-Way)，總之不要問那些一到 Google 就馬上可以找到的問題，如果你問完問題別人馬上丟了一個解答的網址給你，你應該要檢討一下XD 

---

再來是一些不錯的 C++ 資源：

- [CppCon](https://www.youtube.com/user/CppCon/videos)：Cpp conference
- [Cᐩᐩ Weekly With Jason Turner](https://www.youtube.com/channel/UCxHAlbZQNFU2LgEtiqd2Maw)
- [《C++20 STL Cookbook》2023](https://github.com/Mq-b/Cpp20-STL-Cookbook-src)：Mq 白的書
  - [Loser-HomeWork](https://github.com/Mq-b/Loser-HomeWork) ：對應的作業
- [小彭老师现代 C++ 大典](https://142857.red/book/)：這位也是大神，蠻厲害的
- [C++ Quiz](https://cppquiz.org/quiz/question/)：蠻好玩的網站，上面有一堆 Modern C++ 的小題目
- [C++ Standard Core Language Active Issues, Revision 117](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html)：你可以從這邊看到一些 open issue 的討論狀況
  - [C++ Standard Draft Sources](https://github.com/cplusplus/draft)：這邊也可以看到

## LICENSE

This book is licensed under the MIT License. See the [LICENSE](https://github.com/Mes0903/Cpp-Miner/blob/main/LICENSE) file for details.
