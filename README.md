---
title: 礦坑系列 ── 序
date: 2021-05-14
tag: C++ Miner-main
category: C++ Miner
---

<h1 class><div style="display: flex; flex-direction: column; align-items: center;"><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 ── 序 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></div></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

文章一覽：[連結](https://github.com/Mes0903/Cpp-Miner/blob/main/SUMMARY.md)

## 前言 <img src = "https://i.imgur.com/thmVmX6.png?w=930" height = 50>

這是一本介紹 Modern C++ 與一些 C++ 深度解析的書，從我的 hackmd 搬運而來，你可以從[這裡](https://github.com/Mes0903/Cpp-Miner/blob/main/SUMMARY.md)看見文章一覽。 如果您是 C++ 新手，我會建議您先去看 Miner Tutorial 的部分，裡面對初學者會學到的東西做了簡單的介紹，可以幫助大家理解、回憶一些基礎知識，同時可以讓你習慣本書風格

讀著讀著便來到了 Modern C++，語法和文法比以往的 C、C++03 複雜很多，而且在找文章閱讀時發現網路上中文的資源其實沒有想像中的多，受到 Jserv 老師「你所不知道的 C 語言系列」啟發，想著那就自己來寫吧! 於是就有了這個礦坑系列~

原本想說是一個禮拜一篇，但後來發現每篇文章的撰寫時間大概會需要到 50 小時，一個禮拜要一篇實在是有點難哈哈，~~所以後來調整的速度大概會是一個月一篇左右，也有可能兩篇，但也有可能 0 篇QQ~~，後來大約是三個月一篇的速度，若是短篇，像是黑魔法，可能一兩個禮拜便能寫好了，要看寫的內容而定

文章的內容主要會是 Cppreference、draft 或一些好文章的翻譯，可以的話會把語法底層的一些原理也寫進去，可能會碰到一點編譯器和組語，希望能夠寫得盡量詳細，如果有什麼想看的或想補充的也都歡迎留言或私訊告訴我

而取叫礦坑的原因是因為在寫這類文章的時候要挖的東西實在是太多了XD 常常需要挖到 C++ 會議的討論紀錄之類的，像是礦坑那樣深不見底，而想要的東西像鑽石一樣十分難找，自己就像個礦工一樣不停地往內部探索，所以就取叫礦坑系列啦!

感謝一路上教我許多東西的老師、朋友與 Stackoverflow、Discord 上那些不認識但十分熱心的人們

## Summary 文章一覽 <img src = "https://i.imgur.com/thmVmX6.png?w=930" height = 50>

### Miner Tutorial 新手教學

此處為新手教學，可以幫助大家回憶以前的東西，並幫助理解基礎 C++ 與熟悉本書風格

+ [Windows 內的 C++ 與 Vscode 環境建置](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Environment_Building) 
+ [Object、Expression 與 Statement](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Object_Expression_Statement)  
+ [Array 與 Pointer](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Array_Pointer)   
+ [Function 與 Memory](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Function_Memory)   
+ [Class](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/Class)
+ [Class 與物件導向](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_Tutorial/OO)

### Cpp Miner 礦坑系列本文

#### C++11、14

+ [Value Categories 值類別](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Value_Categories)
+ [malloc、new 與 POD Type](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/malloc_new_POD) (部分待補)
+ [std::function](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Std_Function) (source code 解析部分尚未完成)

#### C++17

+ [Structured Binding 結構化綁定](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Structured_Binding)

#### C++20

+ [Concept、SFINAE 與 Detection Idiom](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_main/Concept_SFINAE_DetectionIdiom)

### 礦坑內的黑魔法

+ [用 "explicit" 來偵測複製](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/Explicit_Detect_Copy)
+ [Small String Optimization](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/SSO)
+ [四個不該用 "const" 的時機](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/NoConst)
+ [Indirect through null pointer 是 UB 嗎?](https://github.com/Mes0903/Cpp-Miner/tree/main/Miner_BlackMagic/Indirect_through_null_pointer)

## 貢獻此書 <img src = "https://i.imgur.com/thmVmX6.png?w=930" height = 50>

如果你有任何想討論的內容、想看的內容，或是想添加章節等，歡迎你來聯絡我 [email](mailto:mes900903@gmail.com)，也可以加我 Discord `mes_0903`

如果發現有錯字，可以到 hackmd 處直接修改，我會收到信件通知，你也可以直接發 PR，從 hackmd 搬過來有些格式可能會跑掉，若你發現格式長得有點奇怪，該換行的地方沒有換行等等的，也歡迎告訴我

如果由於版本更新導致文章內容已與事實不符，也歡迎進行更新，文章有時內容會過於艱澀導致我無法翻譯，若你有能力且樂意幫忙翻譯，也很歡迎您修改

## 關於我 <img src = "https://i.imgur.com/thmVmX6.png?w=930" height = 50>

我是 Mes，一個喜愛 C++ 的人，寫文的現在(2021/04/02) 就讀中央大學數學系，目前大一，~~成績很差，盡量不要問我數學~~，如果想一起討論程式問題的話可以加我的 Discord、FB 或 IG，我主要用這三個，程式方面的討論我主要都在 Discord 找人問和回答問題：

- Discord：`mes_0903`
- Facebook：[鄭詠澤](https://www.facebook.com/Mes0903/)
- Youtube：[Mes](https://www.youtube.com/channel/UCT3MbveOznWLlxNIdLUUOhg)

興趣是寫作、Compiler、OS、C++、Assembly，偶爾會去攝影，會在 IG 上寫雜記和分享家裡的貓咪照片，FB 版上全是分享別人動物的貼文，~~愜意的生活~~

## <span class = "orange">一些資源</span> <img src = "https://i.imgur.com/thmVmX6.png?w=930" height = 50>

下面這些是我可能會出現的 Discord 群，我通常會在這些群裡面回答問題或問問題：

+ <strong><a href = "https://discord.gg/programming" class = "wheatlink">The Programmer's Hangout</a></strong>

+ <strong><a href = "https://discord.gg/nRafgDK8fB" class = "wheatlink">Better C++</a></strong>

+ <strong><a href = "https://discord.gg/J5hBe8F" class = "wheatlink">C++ Help</a></strong>

+ <strong><a href = "https://discord.gg/7zfsaTnpbT" class = "wheatlink">Together C & C++</a></strong>

+ <strong><a href = "https://discord.gg/vSYgpmPrra" class = "wheatlink">#include</a></strong>

+ <strong><a href = "https://discord.gg/ypvyFDugM8" class = "wheatlink">中學資訊討論群</a></strong>

如果有哪個連結失效了還請留言或私訊告訴我，我都選了永久的，應該是不會失效才對XD 我非常建議大家加入 Discord 群組討論，Telegram 據我所知還沒有這麼多的群組，而 Discord 有一個優點就是資訊流通的速度非常快，你問完問題馬上就會有人回答了，如果不懂你能馬上再回問她，相較於 FB 社團，甚至是 Email 問外國演講者、作者問題之類的，速度會快上許多

雖然自己思考問題是很好的進步方式，這樣的方式能讓你的思緒更清晰，對事情和原理的理解也會更透徹，但若到了一定階段你還沒思考出來，最好還是找一些人問一下會比較好，別人可能會從你從沒想過的角度來解釋問題，讓你豁然開朗

再來是一些 C++ 的文件：

<strong>

<span class = "burlywood">C/C++ Language References (final/current working drafts)</span>

+ <img src = "https://i.imgur.com/g7fxJnW.png" height = 30>  C89： (沒有 PDF 版本) 、 <a href = "http://port70.net/~nsz/c/c89/c89-draft.html" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/g7fxJnW.png" height = 30> C99 (N1256)： <a href = "http://port70.net/~nsz/c/c99/n1256.pdf" class = "wheatlink">PDF</a> 、 <a href = "http://port70.net/~nsz/c/c99/n1256.html" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/g7fxJnW.png" height = 30> C11 (N1570)： <a href = "http://port70.net/~nsz/c/c11/n1570.pdf" class = "wheatlink">PDF</a> 、 <a href = "http://port70.net/~nsz/c/c11/n1570.html" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/g7fxJnW.png" height = 30> C17 (N2176)： <a href = "https://files.lhmouse.com/standards/ISO%20C%20N2176.pdf" class = "wheatlink">PDF</a> 、 (沒有 HTML 版本)

+ <img src = "https://i.imgur.com/g7fxJnW.png" height = 30> C23： <a href = "http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2596.pdf" class = "wheatlink">PDF</a> 、 (沒有 HTML 版本)

+ <img src = "https://i.imgur.com/OUNNxrC.png" height = 30> C++ 11 (N3337)： <a href = "http://open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf" class = "wheatlink">PDF</a> 、 <a href = "https://timsong-cpp.github.io/cppwp/n3337/" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/OUNNxrC.png" height = 30> C++ 14 (N4140)： <a href = "https://timsong-cpp.github.io/cppwp/n4140/draft.pdf" class = "wheatlink">PDF</a> 、 <a href = "https://timsong-cpp.github.io/cppwp/n4140/" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/OUNNxrC.png" height = 30> C++ 17 (N4659)： <a href = "http://open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf" class = "wheatlink">PDF</a> 、 <a href = "https://timsong-cpp.github.io/cppwp/n4659/" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/OUNNxrC.png" height = 30> C++ 20 (N4861)： <a href = "http://open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4861.pdf" class = "wheatlink">PDF</a> 、 <a href = "https://timsong-cpp.github.io/cppwp/n4861/" class = "wheatlink">HTML</a>

+ <img src = "https://i.imgur.com/OUNNxrC.png" height = 30> C++ 23 (N4885)： <a href = "http://open-std.org/JTC1/SC22/WG21/docs/papers/2021/n4885.pdf" class = "wheatlink">PDF</a> 、 <a href = "https://eel.is/c++draft/" class = "wheatlink">HTML</a>

</strong>

你可以在 <a href = "https://en.cppreference.com/w/cpp/links" class = "wheatlink">Cppreference</a> 看到這些資訊，我的閱讀方式是 Drafts 配 Cppreference 來看，然後上網找例子或自己想一些例子來驗證，如果有不懂的就會到 Google 和 Stackoverflow 搜尋，有時候 CSDN 也會有答案，但就如我前面所說的，比較新的東西通常中文資源很少，不太會有什麼文章。不過據我所知，Drafts 這樣密密麻麻的英文字對許多人來說並不是很友善，所以我的方法可能並不適合你，希望你可以找到自己的方法

那如果一直找不到 (可能是比較深或偏向英文方面意思理解有困難等等)，那我就會到 Discord 群裡面發問，發問的方式可以參考 <strong><a href = "https://github.com/ryanhanwu/How-To-Ask-Questions-The-Smart-Way" class = "wheatlink">How To Ask Questions The Smart Way</a></strong>，總之不要問那些一到 Google 就馬上可以找到的問題，如果你問完問題別人馬上丟了一個解答的網址給你，你應該要檢討一下XD 

---

再來是一些不錯的頻道：

<strong>

+ <a href = "https://www.youtube.com/channel/UCQ-W1KE9EYfdxhL6S4twUNw" class = "wheatlink">The Cherno</a> (親民)

+ <a href = "https://www.youtube.com/channel/UCNge3iECU0XKjshac_hdejw" class = "wheatlink">Italian Cpp Community</a>

+ <a href = "https://www.youtube.com/channel/UCIm-u7l65hp5jboSJrB7U5w" class = "wheatlink">. GUTS</a> (jserv)

+ <a href = "https://www.youtube.com/channel/UCYO_jab_esuFRV4b17AJtAw" class = "wheatlink">3Blue1Brown</a> (有點像科普)

+ <a href = "https://www.youtube.com/user/CppCon/videos" class = "wheatlink">CppCon</a> (非常推薦這個，有很多大佬會分享東西)

+ <a href = "https://www.youtube.com/channel/UCxHAlbZQNFU2LgEtiqd2Maw" class = "wheatlink">Cᐩᐩ Weekly With Jason Turner</a> (這個很猛)

</strong>

---

前言差不多就這樣啦，我是 Mes，一個喜愛 C++ 的人，主推 ina (x

<div style="display: flex; flex-direction: column; align-items: center;"><img src = "https://i.imgur.com/SLfT4YJ.png" height = 400></div>
