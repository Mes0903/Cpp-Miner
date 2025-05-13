---
title: C++ 教學系列 ── 淺談計算機概論
date: 2024-10-13
tag: C++ Miner-tutorial
category: C++ Miner
---

# <div class = "center-column"><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50><br>淺談計算機概論</div>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

## 前言

首先對於這系列的文，如果有什麼寫錯的地方，都歡迎直接修改或與我討論，文章有被修改我都會收到 email 通知，這部分我想老生常談了，因此就不再多做展開

由於 Miner 的前置是針對新手在寫的，所以還是先來閒聊一下，在閆令琪老師的 GAMES202 的第一堂課中，老師有一段話我覺得講得蠻好的：

> 我們在學 202(課名) 的過程中一定要記得這件事情，科學與技術是不等同的，當然這件事情是我個人的理解，我認為科學和技術是嚴格意義上不等同的兩樣事情。 科學代表著知識，代表你對於這個世界應當如何去運作的理解；然後技術是什麼呢? 技術更多代表的是技巧，就是所謂的 engineering skills，工程能力啊。 然後技術是用來幹什麼的呢? 是要把科學轉換為產品的這麼個意思<br><br>
>
> 而科學和技術通常人們會簡稱科技，這是為什麼呢? 這是因為絕大多數人認為這兩件事情，反正我都不懂，所以像是一回事對吧，但對於我們來說，要區分一下 science 和 technology<br><br>
>
> 一個最典型的例子就是「車」對吧，大家知道汽車這個東西，大家在很久之前，就已經知道應該怎麼樣去運作了，但是直到今天，咱們國產的車子性能仍然不是特別那個對吧(XD)，所以說啊，這就是「技術」在背後起到的關鍵作用<br><br>
>
> 所以說科學和技術不等同，但我沒有說哪一項更重要，技術和科學我可以認為他們同等重要

對於工程的問題，我們通常會有 spec 來嚴格定義我們所想要的行為，遇到什麼狀況需要做什麼事。 而我個人感覺學校在教「科學」類型的課程時，通常不會講得這麼「繁瑣」，因為這類的系統比較開放，你很難像工程的問題那樣對所有的事都做定義

換句話說就是比較模糊，而這類問題通常數學就會多，也因此我個人覺得「數學」其實是門很模糊的語言，用 CS 的角度去看很多東西的定義都有點模糊，換句話說「數學」的定義很強，一個定義就可以解釋很多事情，但也因此用起來很不直觀，你往往需要很多的先備知識才能很好的使用這個語言。 也因此想要用數學把想法描述清楚是一件相對困難的事情，數學系還會花特別一學期來教你怎麼做這件事，通常是基礎數學或是數學導論之類的大一必修課

當然這個是我個人的理解，數學比我好的人多的是，也許他們又會有不同的想法。 但至少與朋友閒聊時我發現有些蠻厲害的工程師也跟我有同樣的觀點，所以我想這至少不是只有我一個人的想法，蠻有趣的

對於語言這個東西，以我的經驗來說，資工系大部分的課就是教 C/C++ 或 python，而教法不外乎就是開始從變數、型態、函式、class 這樣一路講一下概念，帶幾個例子後出個作業就過去了

換句話說，我覺得這些課的教法很接近於「科學」類型的課程的教法，講講概念，讓你看幾個例子有個經驗，出幾個作業讓你熟悉一下這個「感覺」

然而語言是一個工程類的東西，它是有 spec 的，裡面嚴格定義了什麼樣的輸入該有什麼樣的輸出，因此學習語言的過程你應該要去讀 spec，而不是依靠「經驗」，它不是一個開放的系統，它是一個人定義出來的封閉系統

換句話說學語言跟學下棋是很類似的，例如西洋棋，在學的時候你需要先理解棋子移動的規則，而且你需要去遵守這些規則，所以你會去看西洋棋的規則書/說明書(spec)，而不是找個人來告訴你棋子移動的「經驗」法則    

同樣地，你需要去遵守語言 spec 裡面的規則，而不是自己去嘗試跑一次你的 code，發現會動後就認為那樣的寫法是對的，甚至是認為不需要遵守 spec 內的規則，會動與正確是兩件事情，在工程上我們會利用 UB、IB 等規則來對這種錯誤的寫法去做規範，之後有機會的話會再講到

另外，要如何利用這些規則來贏遊戲，也就是所謂的戰術，就需要依賴經驗了，這在語言的部分也是一樣的，要怎麼寫出好的程式碼也需要經驗，可能會有一些前人發現的法則、規律，但它們是可調整的，就像下棋一樣，你可以自己去研發自己的套路

總而言之，寫語言記得翻 spec，遵守 spec，語言是被嚴格定義好地，你不該利用經驗法則去判斷一個寫法是否正確

要注意我並不是指「科學」較淺，「技術」較深，而是指這兩種東西的面向不同，學習方法也理當有差，不該混用

這個礦坑系列的教學會時不時地引用 spec 內的定義，來教新手 C\+\+，由於 C\+\+ 相對其他語言來說較為底層，也就是說比較接近硬體，抽象化的程度比較少，因此在進入 C\+\+ 的教學之前，我覺得最好先寫一篇對電腦運作的簡述，帶大家簡單學習一下計算機概論

## 如何學習一樣東西

因為這系列的文是針對新手在寫的，但我後來發現其實新手很常有不知道如何學習的狀況，甚至是不知道如何 google，想 google 卻不知道從何下手，因此這邊來簡單講一下這件事

我覺得學一樣東西最快的方法就是寫部落格，只要將自己的理解輸出出來，你就知道自己大概哪裡卡了，哪裡不懂

為了寫文章，熟悉一些工具肯定是沒有壞處的，具體可以參考 jserv 老師的這篇：

- [GNU/Linux 開發工具共筆](https://hackmd.io/@sysprog/gnu-linux-dev/https%3A%2F%2Fhackmd.io%2F%40sysprog%2Fr1Psrf0KW)

而 blog 的內容不外乎就是記錄自己的思緒，你應該嘗試將你的主題整理出一個脈絡，照著這個脈絡，盡可能簡單地，一步一步的完成你的文章

> If you can’t explain something in simple terms, you don’t understand it ー Richard Feynman

在學習的過程中你肯定會遇到問題，這種時候你應該先嘗試釐清你的問題，將問題化為需求，然後列出完成需求的步驟，如此一來就可以很清晰的一步一步解決你的問題

舉個例子，老師今天發了個 OpenGL 的作業下來，要你們裝一台 Ubuntu 的虛擬機來執行 sample code，但你從來「沒聽過」什麼是虛擬機，那這時候就需要先釐清你的問題，再整理成需求，脈絡可能如下：

1. 什麼是虛擬機?
2. 我的電腦是 windows 的系統，我要怎麼在 windows 上裝 Ubuntu 的虛擬機?
3. google 後發現有許多個虛擬機可以裝，像是 VMware、VirtualBox、WSL 等，裝哪個好?
4. 假設我想裝 WSL，那 WSL 要怎麼裝?
5. WSL 裝好後，要怎麼灌 Ubuntu?
6. Ubuntu 灌好後，要怎麼裝 OpenGL? 我要如何知道我的 OpenGL 裝成功了?
7. 我要怎麼編譯和執行 OpenGL 的程式嗎?

這時你就成功把你的問題化成了 7 個「需求」，或者說比較直接的問題，這種問題你一 google 下去，十之八九可以直接 google 到答案，不需要再在一堆搜尋結果中找半天

換句話說，「需求」很接近你完成問題的「步驟」

然而在遇到問題時，你肯定無法很直接的像這樣將大部分的需求全部列出來，這種情況你可以先嘗試照著順序將已知的需求完成，通常新的需求就會冒出來了

這個過程我們通常會稱它為：know what, know why, know how

當你無法列出需求時，這通常代表你的背景知識不足，導致你建立不了步驟與步驟之間的連結；抑或是這個東西非常的冷門，此時你就該找人詢問了，有關提問的方法請參考非常有名的一篇文章：

- [提問的智慧](https://github.com/ryanhanwu/How-To-Ask-Questions-The-Smart-Way)

而各位生在 2024 年這個 LLM 如此發達的時代，肯定會想要利用它來學習，LLM 能夠很快的幫你分析出你的需求，但我個人不建議新手直接使用 LLM，原因不外乎有二

1. 新手非常需要練習這種「釐清問題」的能力，在你的生涯中，你遇到的問題會越來越大，當需求越來越多時，你再使用 LLM 這樣幫你跳過分析的過程，那你高機率會陷入擁有需求卻無法解決的狀況
2. 你在分析問題過程中，或多或少都會摸到一些額外的知識，像是你小時候在 wiki 裡面逛一個一個的標籤一樣，這些機會會是你增加知識廣度的一個很大來源

LLM 十分方便沒錯，是個很好的工具，能幫助你節省時間，但如果你從一開始就在節省時間，那在建構你自己的思考模型時就很容易會有缺漏，就好像吃飯吃太快會消化不良一樣，知識也會消化不良

如果是狀況緊急，需要速成的情況則另當別論，直接用 LLM 跳過思考的過程是個不錯的方法，能夠幫助你很快的解決需求，但在這種知識這麼集中的環境下學習，就如同機器學習會 overfitting 一樣，很容易讓你將局部的知識當作普遍定理，導致誤解，切要注意這點

以上都為個人經驗與理解，如果你已經有自己的學習方式了，那照你自己舒服的方式繼續你的旅程就好！ 接下來就開始進到計概的內容~~

## 抽象化 Abstraction

### 抽象化是什麼

工程的目的是使用科學，科學提供了對自然現象的理解，而抽象則是對現象做包裝的一種方法，例如生活中的「顏色」，顏色是人的感官對於特定波長的電磁波的解釋，但當我們平常聊天在講「紅色」時不會特別地去強調「620-750nm 的電磁波」的這個資訊，更多是在強調人的感官的解釋

換句話說，「紅色」這個詞是對「620-750nm 的電磁波」所做的抽象化，它對「620-750nm 的電磁波」做了包裝。 在用「紅色」這個詞的時候，我們把「620-750nm 的電磁波」、人眼的構造與大腦對人眼訊號的解析等等地細節都隱含的省略掉了，因為我們只在意 620-750nm 的電磁波對我們造成的感官結果。這讓我們在對話的過程中可以很簡單的達到共識，抓到重點    

所以，用一句話來描述抽象化的概念就是「不管中間細節，只在乎輸入與輸出的關係」，我們不在意中間過程的細節，想把它全部忽略掉，更多的時候我們會說把它當成一個黑箱，我們只想知道丟了一個東西進去，會有什麼樣的結果出來

再舉個我很喜歡的例子，假設我們要去餐廳點餐，那我們在意的重點是「我要付多少錢才可以拿到我想要的餐點」，我們不在乎餐廳的人員要如何跟廚房溝通、廚師有幾人、用的調味料產自何處和他們要如何製作這份餐點，只要我付完錢，我要的餐點會來，這樣就可以了

畫成圖來說就是這樣：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/abstraction.png"><br> 

</div>

我不在乎中間那個框框(黑箱)內做了什麼，我只在乎我的錢進去框框後，我的飯會從框框內出來

### 電路科學的抽象化

學電機的人會利用 Maxwell's equations 來研究電磁現象，然後在 Maxwell's equations 之上創建一個新的抽象層，稱為集總電路(lumped circuit abstraction)，利用集總電路我們可以再增加一個新的抽象層，稱為數位層(Digital abstraction)，而利用數位電路又可以再增加一層邏輯閘層(Logic gate abstraction)：

以此類推，可以分好多好多層下去，這些不同的抽象層面就對應到電腦科學中不同的專業領域：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/abstraction-2.png"><br> 

</div>

這邊的分類是我照著我電子電路學課本「foundation of analog & digital electronic circuits by anant agarwal」內的分類畫出來的，課本的分類我覺得合理。 這中間的每一層都可以再各自有其細分下去的抽象層，而 Programming language 的上方就是各式各樣的程式了    

然後我大概把資工、電機系內對應的課大概寫了一下，只寫了我有摸過、聽說過的，有些理解的不深，如果有錯麻煩再糾正我一下XD

## 圖靈機 Turing machine

圖靈機基本上是一系列的自動機所建構出來的理論模型，因為是計概所以我這邊就不講太多，也不寫它的數學定義，有興趣的可以看一下自動機理論和計算理論的課：

<div class = "center-column">

<img src ="https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_Tutorial/Computer_Introduction/image/automata.png?raw=true"> 

(source：[Wikipedia](https://en.wikipedia.org/wiki/Combinational_logic))

</div>

這裡可以提一下的是上圖中最內層的組合邏輯(Combinational logic)，這是一種由 [Boolean Circuit](https://en.wikipedia.org/wiki/Boolean_circuit) 所組成的數位邏輯，這裡的 [Boolean Circuit](https://en.wikipedia.org/wiki/Boolean_circuit) 是一種於計算理論中的數學模型，用來描述形式語言，所以並非真實的電路。 提這個的主要原因是，計算機內部的電路為組合邏輯與時序邏輯([Sequential logic](https://en.wikipedia.org/wiki/Sequential_logic))混合的實作，所以可以稍微知道一下有這個東西

而對於圖靈機，我覺得 wiki 上的描述寫得很好，所以讓我們看一下 [Wikipedia](https://zh.wikipedia.org/wiki/%E5%9B%BE%E7%81%B5%E6%9C%BA) 上的描述：

圖靈機基本思想是用機器來模擬人類用紙筆進行數學運算的過程，它把這個過程看作下列兩種簡單的動作：

- 在紙上寫上或擦除某個「符號」
- 把注意力從紙的一處移動到另一處

而在每個階段，人要決定下一步的動作依賴於兩個條件：

- 此人當前所關注位置上的符號
- 此人當前思維的狀態

為了模擬這個過程，圖靈構造出一台假想的機器，該機器由以下幾個部分組成：

1. 一條無限長的紙帶(TAPE)，其被分為一個接一個的小格子，每個格子上包含一個來自有限字母表的符號，字母表中需要一個特殊的符號代表空白。 紙帶上的格子由左至右依次編號為 0, 1, 2, ...，無限伸展下去   
    :::info   
    :bulb: 這裡的有限字母表基本上就是一個有限集合，元素是定義的符號   
    :::   
2. 一個讀寫頭(HEAD)。 它可以在紙帶上左右移動，讀出當前所指的格子上的符號，並且能改變它
3. 一個狀態暫存器。 它用來儲存圖靈機當前所處的狀態。 圖靈機的所有可能狀態的數目是有限的，並且有一個特殊的狀態，稱為停機狀態，在這邊不是我們的重點，可以先不理他
4. 一套控制規則數量有限的表格(TABLE)。 它根據當前機器所處的狀態以及當前讀寫頭所指的格子上的符號來決定讀寫頭下一步的動作，並改變狀態暫存器的值，讓機器進入一個新的狀態    
    
    其會按照以下順序告知圖靈機命令：
    1. 寫入(替換) 或擦除當前符號
    2. 移動讀寫頭，有三種移動方法：「L」表示向左，「R」表示向右，「N」表示不移動。 每次最多移動一格
    3. 保持當前狀態或者轉到另一狀態

注意這個機器的每一部分都是有限的，但它有一個潛在的無限長的紙帶，因此這種機器只是一個理想的裝置。 圖靈認為這樣的一台機器就能類比人類所能進行的任何計算過程

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/turing-machine.png"><br> 

</div>

另外這篇也寫得很好，有興趣的也可以讀一下：[搞懂「通用圖靈機」的終站——它的誕生與意義](https://pansci.asia/archives/203152)

而在現代，我們所謂的「電腦」，或正式一點稱它為「計算機」，其實就是圖靈機的一種實作方法，現在較主流的實作方法是使用馮諾伊曼架構，比較大的特色是將儲存裝置與中央處理器分開了    

以比較常見的家用 PC 來說，對應的關係大概如下：

| 圖靈機組件 | 對應電腦元件 | 功能 |
| --------- | ------------ | -------- |
| 紙帶（Tape） | 主記憶體（RAM）或儲存裝置，像是 Disk | 提供資料和指令的存儲空間 |
| 讀寫頭（Head） | 記憶體控制器、匯流排 | 負責控制 CPU 與記憶體之間的資料傳輸 |
| 狀態暫存器（State Register） | CPU 中的暫存器（如程序計數器、狀態暫存器） | 保存當前的執行狀態和指令位置，控制程序的執行流程 |
| 規則表格（Transition Function/Table） | CPU 的控制單元和指令集架構 | 根據當前狀態和指令來解碼和執行操作，控制資料流和指令執行 |

另外還有一些其他的實作方法，但目前就較偏向用在特殊場合的設計，所以我們這裡就不特別展開敘述了

由於現代的電腦只是種圖靈機的實作，因此在定義語言的時候，我們會針對圖靈機去定義，而不是針對電腦去定義，因為沒有人可以確定之後不會出現新的實作方法，在 spec 中可以看見這段話：

> [n4861(4.1.1-1)](https://timsong-cpp.github.io/cppwp/n4861/intro.abstract#1):
> The semantic descriptions in this document define a parameterized <span class = "yellow">nondeterministic abstract machine</span>. This document places no requirement on the structure of conforming implementations. In particular, they need not copy or emulate the structure of the abstract machine. Rather, conforming implementations are required to emulate (only) the observable behavior of the abstract machine as explained below.

這邊 spec 用的詞是「abstract machine」，在這邊可以先簡單理解為差不多的東西

## CPU & RAM & Bus

### RAM (random-access memory)

從圖靈機我們可以看出，其實只要有 CPU、RAM、Bus，就差不多可以完成一台計算機了，現在我們就來針對這三個硬體來做介紹，幫助大家更理解計算機的運作。 這邊有一部很好的影片，大家也可以看看：[How a CPU Works](https://www.youtube.com/watch?v=cNN_tTXABUA)

RAM 主要有兩種：靜態 RAM 與動態 RAM，又分別被稱為 SRAM 與 DRAM； SRAM 相對來說速度比較快，但成本比較高，我們個人電腦上的 main memory 全名為 DDR SDRAM，屬於一種 DRAM，因此相較於 SRAM 來說速度就比較慢，而 SRAM 通常我們會將其使用在 CPU 內的 cache 上。 在電腦科學中到處都有這類的 trade-off，所以要如何做取捨就是門學問了

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/ddr4.png" width = 60%>

(兩支美光 Crucial 8GB DDR4-2133 ECC 1.2V RDIMM)    
(source：[Wikipedia](https://zh.wikipedia.org/zh-tw/DDR4_SDRAM#/media/File:Two_8_GB_DDR4-2133_ECC_1.2_V_RDIMMs.jpg))

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/NES-SRAM.png" width = 60%>

(任天堂 NES 內的 SRAM，容量為 2Kx8 bit)   
(source：[Wikipedia](https://en.wikipedia.org/wiki/Static_random-access_memory#/media/File:Hyundai_RAM_HY6116AP-10.jpg))

</div>

而前面有提到 memory 對應到圖靈機中的紙帶的部分，因此它負責提供資料和指令的存儲空間，我們可以將紙袋上的每一個格子視為一個 Byte，並且我們會給記憶體標上位址：

1. 位址通常以 Byte 為單位，也就是 8 個 bit
2. 紙帶一格可以儲存一個符號，因此對應到記憶體中的一個 Byte

假設是 4G 的 memory，那它的記憶體位址就可以從 0 一路寫到 4 \* 1024 \* 1024 \* 1024，換句話說就是從 0 到 4,294,967,295（我這裡從 0 開始）。 我們通常喜歡用 16 進位表示，所以會寫成 0 ~ 0xFFFFFFFF：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/memory-address.png"> 

(source: [Integer Arithmetic and Memory Access](https://www.eecis.udel.edu/~davis/cpeg222/AssemblyTutorial/Chapter-04/ass04_5.html))

</div>

### CPU (Central processing unit)

CPU 中文為中央處理器，大多數 CPU 的目的，無論 CPU 的形式為何，都是執行一連串被儲存的指令，這些指令會被保存在記憶體中。 而執行的步驟主要分為提取(Fetch)、解碼(Decode)、執行(Execute)和儲存(Write Back)：

- 提取(Fetch)：從記憶體中讀取指令
- 解碼(Decode)：解析指令，確定 operation 的類型與 operator
- 執行(Execute)：使用 ALU 或其他執行單元進行計算或操作
- 儲存(Write Back)：將結果存到暫存器/記憶體中

我們會針對 CPU，或更 general 的說，針對 Processor 定義軟體與硬體的介面，這個介面被我們稱為 ISA，基本上會包含 Processor 讀取和操作的指令格式、machine code、Address mode、Registers 等等

因為是介面，所以 ISA 是個抽象化的過程，其將 Processor 真正的硬體實作給藏起來了，統一了一個對外的介面。 而對於 CPU 真正的硬體實作，我們稱其為 Microarchitecture，它基本上描述了一顆特定的 CPU 如何實現 ISA 的介面

CPU 的內部主要可以分為三大部分：控制單元(Control unit)、暫存器(Register) 與算術邏輯單元(ALU)，下圖是一個簡單的擁有單處理器 CPU 的計算機架構示意圖，紅線表示 data flow，黑線表示 control flow：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/uniprocessor-CPU.png" width = 60%> 

(source：[Wikipedia](https://en.wikipedia.org/wiki/Computer_architecture#/media/File:Computer_architecture_block_diagram.png))

</div>

暫存器負責儲存指令，operator 和計算的中間結果的數據，速度非常快。 而 ALU 則負責執行算術運算、邏輯運算和位移運算等，是 CPU 執行運算的主要元件

Control unit 用來命令 Processor 的操作，主要負責剛剛提到的 Fetch 與 Decode，另外還會有管理指令的執行過程、協調 ALU 與暫存器等功用

因此一般來說，CPU 執行指令的方式是從記憶體中取出指令，並使用 ALU 執行運算，期間可能會將一些中間產物存到暫存器中，最後再將結果儲存到記憶體中    

雖然最後需要將結果儲存到記憶體中，但由於相較於和 memory 溝通的速度，CPU 內部元件的溝通速度快非常多，在程式的執行過程中，我們往往有些資料會需要重複使用，因此我們會於 CPU 內部加一些 memory 進去，這些記憶體被稱為 cache，而由於對速度的要求，因此 cache 所使用的為 SRAM，而非前面講的 DRAM，也因此 cache 的大小相較於 main memory 來說小很多(很貴)

而對於 cache 的部分，我個人覺得這部分不是我們這篇計概(了解電腦運作的 map )的重點，所以就先不寫了，不過 jserv 老師有翻譯一篇很好的論文，雖然難度比較高，但有興趣的可以去讀看看：[每位程式開發者都該有的記憶體知識](https://sysprog21.github.io/cpumemory-zhtw/introduction.html)

### 舊時代的南北橋

當 CPU 想要讀寫 memory 時，需要傳一個訊號給記憶體控制器，這東西裡面包含了讀寫 DRAM 所需的邏輯。 因此你可以看到 CPU 上有很多針腳，它們會接到 Bus 上，讓 CPU 能夠透過 Bus 收發資料：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/CPU.png" width = 40%><br> 
(source：[Wikipedia](https://en.wikipedia.org/wiki/Central_processing_unit#/media/File:Laptop-intel-core2duo-t5500.jpg))

</div>

以前會把記憶體控制器做在北橋裡面，北橋再拉 Bus 到 memory 上，因此 CPU 想要跟 memory 溝通，就需要先到北橋，再到 memory 去。 其中 CPU 到北橋的這段 Bus 有個名字被稱為 Front-side bus(FSB)，而記憶體控制器到 memory 的這段 Bus 叫 memory bus：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/chipset-layout.png" width=50%> 

(Pentium II/III 時代的典型晶片組佈局)   
(source：[Wikipedia](https://en.wikipedia.org/wiki/Front-side_bus#/media/File:Motherboard_diagram.svg))

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/chipset-layout-2.png" width=50%> 

(我覺得這張寫得更清楚一點)    
(source：[Anyone know what this part of my motherboard is?](https://www.reddit.com/media?url=https%3A%2F%2Fpreview.redd.it%2Fanyone-know-what-this-part-of-my-motherboard-is-v0-794wyr5ninvc1.jpeg%3Fwidth%3D498%26format%3Dpjpg%26auto%3Dwebp%26s%3D7dab33952489ae88d70d9369411c91e93845a972))

</div>

### 現代 intel PCH / AMD FCH

於上面的圖中我們可以看到北橋還會接一些較高速的周邊裝置，通常使用的是 AGP 或是 PCIe 協定。 後來，CPU 持續的在變快，導致 FSB 的頻寬跟不上上了，這個瓶頸導致大概在 2008 年時 FSB 就被淘汰了

之後的設計慢慢改成了點對點和 serial 對接，將記憶體控制器與較高速的 AGP/PCIe 通道這兩個東西直接併入到了 CPU 晶片中，讓裝置直接與 CPU 對街，並將北橋剩下其他剩下的小功能併入了南橋中，從而將傳統的北橋給消除掉了

在接顯卡時大家都會說顯卡接在直連 CPU 的通道，也是因為以前北橋上的高速 PCIe 通道被併進去了，所以才會說是「直連 CPU 的通道」。 而圖中的另一個 bridge 為南橋，負責處理與其他速度較低的外設的溝通，像是 EIOE ATA、SATA、USB、ISA slot、PCI/PCIe slot 或是 BIOS 的溝通等等

在 Intel 的術語中，其以 memory controller hub(MCH) 稱呼北橋，以 I/O controller hub(ICH) 稱呼南橋，在將北橋的剩餘功能移到南橋內之後，其將 ICH 改名為了 Platform Controller Hub(PCH)；而在 AMD 的架構中，與 PCH 對應的晶片組被稱為 FCH

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/pch.png"> 

(source：[Wikipedia](https://en.wikipedia.org/wiki/Platform_Controller_Hub#/media/File:Intel_5_Series_architecture.png))

</div>

除了南橋的所有功能之外，PCH 還合併了一些剩餘的北橋功能（例如 clock）。系統時脈以前是與專用晶片連接的，但現在被合到了 PCH 裡面。 PCH 和 CPU 之間有兩種不同的連接：Flexible Display Interface(FDI)和 Direct Media Interface(DMI)，不過這我也不熟，就不展開了

題外話，PCH 中的有顆東西叫 IME，其全名為 Intel Management Engine，原本在北橋內，從 Nehalem 處理器和 Intel 5-Series 系列晶片組開始改成內置在 PCH 中了。 它是一個獨立的子系統，擁有自己的 MAC 和 IP 位址，而且能夠在系統啟動前、OS 運行期間甚至是關機的情況下運行，用來為搭載 Intel 處理器的電腦系統提供各種功能與服務

很諷刺的是之前有駭客扁進了 Intel Management Engine 然後把所有資訊倒了出來，有興趣的可以看一下：[36C3 - Intel Management Engine deep dive](https://www.youtube.com/watch?v=3CQUNd3oKBM)

### Bus

Bus 負責傳輸資料，傳統的 Bus 主要分成三種：Data bus、Address Bus、Control Bus

- Address Bus   
  Address Bus 用來指定 <span class = "yellow">physical address</span>，當 CPU 或傳統上支援 DMA 的裝置需要讀取或寫入某個記憶體位址時，就會透過 Address Bus 來指定該記憶體位址。 較現代的 DMA 裝置通常改成透過專用的 Bus 或 Controller，像是 PCIe、SATA Controller 等，來與 memory 溝通
  
  Address Bus 的寬度決定了系統可以尋址的記憶體量，例如 32-bit 的 Address Bus 就可以尋址 $2^{32}$(4,294,967,295) 個位址
- Data Bus  
  Data Bus 用來傳送實際的資料，通常是雙向的
- Control Bus   
  Control Bus 用來傳輸控制信號，像是讀/寫命令，clock 信號等等

當 CPU 欲與 memory 溝通時，會先將欲操作的位址在 Address Bus 上設成高電位，再利用 Control Bus 將 control bit 依照 spec 設好，最後透過 Data Bus 讀寫資料：

[How a CPU Works](https://www.youtube.com/watch?v=cNN_tTXABUA) 這部影片裏面引用了一個簡單的 CPU 模型，稱為 Scott's CPU，其 Address Bus 和 Data Bus 各有 8 個 bit，Control Bus 則有 set 與 enable 這兩個 bit，對於讀取來說就會長這樣：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/scott-read.gif" width=90%><br> 

</div>

先在 Address Bus 上將要操作的 address 設為高位，接著將 Control Bus 上的 enable 設為高位，最後將資料讀進來

寫入則長這樣：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/scott-write.gif" width=90%><br> 

</div>

順序稍微不一樣，先將 Address Bus 設好，然後將 data 送上 Data Bus，最後將 Control Bus 的 set 設為高位

現代由於已經沒有南北橋晶片了，memory controller 做在了 CPU 裡面，因此與 memory 是直接對接的，而中間溝通的這個 Bus 則被稱為 memory bus

然後要注意這是一個教學用的簡化模型，實際情況不長這樣，有興趣的可以去翻一下 [DDR4 之類的 spec](https://xdevs.com/doc/Standards/DDR4/JESD79-4%20DDR4%20SDRAM.pdf) 看看

## BIOS & OS

### BIOS

透過上面那些知識，我們知道了 CPU 要如何與 memory 溝通，和如何運行一個程式(一系列的指令)，現在我們終於要開始看一下電腦開機後到底發生了什麼

電腦上有個東西叫做 Basic Input/Output System，簡稱為 BIOS，是我們個人電腦開機時載入的第一個軟體。 BIOS 的作用是初始化和測試硬體元件

早期的 BIOS 通常儲存在 BIOS ROM 裡面，廠商在寫完 BIOS 後會將其執行檔透過燒錄器或是 flash tool 燒錄到這個 ROM 內，因此在主機板出廠時 BIOS 就會預先安裝在主機板上了。 然而隨著 BIOS 的大小和複雜度增加，硬體更新的速度變快，導致現代的 BIOS 也必須能夠更新，以支援新的硬體，因此 BIOS 就改成存在 EEPROM 或是 Flash memory 中讓使用者可以更新了

:::info   
:bulb: RAM 和 ROM 的差別我這邊不細講，只要記得 RAM 內部資料可以任意讀寫；而 ROM 的內部資料寫入後需要透過特殊的方式才能修改資料    
:::   

有關傳統 BIOS 的詳細行為，有興趣的人可以看看小華的部落格：[[我所知道的 BIOS]](https://biosengineer.blogspot.com/search?q=%5B%E6%88%91%E6%89%80%E7%9F%A5%E9%81%93%E7%9A%84BIOS%5D)

我們前面有提到 CPU 會透過 memory address 來找到要執行的程式(Fetch)，因此我們需要將 BIOS 放在一開機時 CPU 就會開始讀的起始位址，這個位置通常是在 0xFFFFFFF0 這個地方，如果我沒記錯，UEFI 也是一樣的位置

當 BIOS 開始執行時，會先執行[自檢(POST)](https://en.wikipedia.org/wiki/Power-on_self-test)，開始識別、測試系統設備，像是 CPU、chipset、RAM、主機板、顯卡、鍵盤滑鼠、硬碟等等的設備

當 BIOS 完成 POST 後，會再去掃描 Option ROM modules，用途是識別、測試和初始化其他的設備，常見的像是 VGA card、Lan card 和 RAID card 等等。 這些 ROM 可能會直接在裝置(如網卡、RAID 控制器等)上，也有可能包在 BIOS 的 image 內

在 POST 與 Option ROM 的初始化都做完後，電腦所有的硬體就都被初始化完畢了，接下來 BIOS 會呼叫 interrupt 19h，嘗試從「開機裝置（像是硬碟、CD、USB 等等）」中找到並執行開機載入程式(boot loader)

Boot loader 啟動後會載入並執行他找到的第一個啟動軟體，通常是我們的作業系統(OS)，但也有可能是其他的，像是你在重灌電腦的時候，跑的就不是你原本的 OS，而是你的重灌軟體（雖然如果我沒記錯的話，有些裡面也是有 OS）

底下是一張簡單的啟動示意圖：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/boot-process.png">

(source：[Wikipedia](https://en.wikipedia.org/wiki/BIOS#/media/File:Legacy_BIOS_boot_process_fixed.png))

</div>

另外 BIOS 會使用 CMOS 來儲存一些 BIOS 的設定，像是系統時間、硬體配置和你的開機順序等。 而 CMOS 使用的是揮發性的 SRAM，換句話說斷電後儲存的設定會消失，因此主機板上通常會配一顆小型的電池來使其不斷電，這顆電池我們叫他 CMOS 電池。 小時候在修電腦的時候，我們如果想要重置 BIOS 的設定，就會把這顆電池拔掉，如此一來存的設定消失，就會重置 BIOS 的設定了

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/CMOS.png" width = 70%>

(Pico ITX 主機板中的 CMOS 電池，右邊那顆銀色圓圓的就是)    
(source：[Wikipedia](https://en.wikipedia.org/wiki/Nonvolatile_BIOS_memory#/media/File:Bottom_EPIA_PX10000G_Motherboard_new.jpg))

</div>

### 作業系統(OS)

前面講完了 BIOS，BIOS 正常情況下最終會將作業系統(OS) 給啟動，並將電腦的控制權交給 OS，所以我們現在就來看一下什麼是 OS，這邊抄一下之前的 OS 筆記XD...

「作業系統（英語：Operating System，縮寫：OS）是一組主管並控制電腦操作、運用和執行硬體、軟體資源和提供公共服務來組織使用者互動的相互關聯的系統軟體程式」── [Wikipedia](https://zh.wikipedia.org/wiki/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F)

簡單來說就是一個管理電腦的系統程式，其是使用者和電腦硬體的介面(interface)

整個電腦系統主要可以分成四個部分：硬體、作業系統、應用程式和使用者，這邊先對它們做個簡單的介紹，但這些不是定義，只是一個描述

- 使用者(User)：   
  使用者可以是人、機器或其他的電腦，只要是可以操控這台電腦的都可以算  
- 應用程式(Application)：
  能幫助使用者解決問題的軟體都可以算，像是 Browser、Compiler 或一般的 Text Editor 都算  
- 作業系統(Operating System)：
    能夠幫助我們「控制(control)」和「協調(coordinate)」資源的系統軟體
    - 控制(control) 指的大概就是 Device Driver，能夠幫助我們去控制硬體的，重點在協調(coordinate)，怎麼樣去協調使用者，分配資源，是 OS 裡面比較複雜的部份
    - 因此我們也可以說 OS 是幫忙分配資源的軟體(resource allocator)，像是 memory management 會幫忙分配記憶體，file system 會幫忙分配 disk block 等等
- 硬體(Hardware)：
    能夠拿來給我們操作，做運算的硬體，因此硬體在 OS 這邊我們習慣稱它們回資源 (resources)，我們不在意它是透過哪種硬體提供的，只在意它能夠提供哪種資源，可以怎麼運用它

硬體的部分我們剛剛前面都講完了，他和 OS 中間還會有一層 BIOS，或是也可以把它歸類在 OS 裡面，無論如何 BIOS 前面我們也講了，剩下的兩個部分就比較偏應用層面，我們暫時不關心

由上可見 OS 可以看作是一個 resource allocator 和 control program，我們也稱它為 kernel，是一個在電腦內部隨時都在執行的程式，前面兩個名詞可以不用特定去記，因為只是一個比較常見的定義，但其實沒有很確切的定義 OS 是什麼，<span class="yellow">但 kernel 這個名詞很重要，要記一下</span>

因此作業系統就會有一些 for virtual resource 的 API 可以使用，這些 API 我們稱它為 system call，<span class="yellow">這些 API 是 user 與 resource 間唯一的 interface</span>。 這裡的 virtual resource 主要指的就是剛剛我們講的硬體，因為要抽象化，所以我們通常會用虛擬的介面來定義

我們看一個例子：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Mes_Note/refs/heads/main/Operating_System/Ch1_Introduction/Image/General_Purpose_Operating_Systems.jpg" width = 70%>

(source：[清大周志遠教授的 OS 講義](https://ocw.nthu.edu.tw/ocw/index.php?page=course_news_content&cid=141&id=999))

</div>

最下面是硬體，上面則是我們寫的程式，程式大致上可以分成「和作業系統相關的」與「和作業系統無關的」，也就是圖上的 user mode 與 kernel mode，這在後面的章節會再提更多

Device Driver 也算 OS 的一部份，把它抽出來是因為它是可以一直更新、抽換的；Driver 上面就會有 OS 的主要部分，也就是如何管理 memory、cpu 等等的軟體，這個軟體會提供一些 API，也就是前面提到的 System call，OS 會把它包起來成 System library，讓我們使用

我們寫完 Program 後要先 Compile，然後透過 Linker 把這些 Compile 完的 `.o` 檔 link 在一起，代表我們要去使用別人 implement 好的 Program，因此他不會只 link 我們 include 的那些 library，也一定會 link System library，link 完後才成為一個能執行的執行檔

以 `printf` 來舉例，我們要印出東西在終端機上，這個動作不會是 C library 去做，而是 OS 去做的，需要呼叫到 system call，可能不是直接呼叫，但一定間接會呼叫到它，所以一定要 link System library，等到後面講 system call 時會更清楚

因此這個過程中我們會先呼叫 `printf`，然後會呼叫到 system call，再去呼叫到 driver 這樣一層一層下去

## 二進制檔案 Binary File

現在我們有了 OS 的概念，也就可以跟硬體打交道了，所以最後我們來看一下我們到底要怎麼執行一個程式。 我們的執行檔都是一種二進制檔案，目的是使計算機根據機器碼執行指定的任務

二進位檔案通常被認為是 Byte 的序列，這代表 binary digits(bit) 以八個為一組，有關計算機編碼的部分，由於已經有一篇非常棒的文章了，因此我這邊不打算詳細展開，請自行去閱讀 jserv 老師的[解讀計算機編碼](https://hackmd.io/@sysprog/binary-representation)文章

我們之後的文章都會假設你已經讀過了這篇文章，理解了計算機編碼。 這是一篇科普文，門檻不高，建議你它看完，肯定會對你理解計算機有所幫助

另外這邊分享一個有趣的小故事：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/von-Neumann-machine-code.png" width = 80%><br>

</div>

> von Neumann 認為直接用機器碼來寫程式就好了。 von Neumann 有一個博士班學生，因為用機器碼太煩了，著手設計組合語言。 von Neumann 知道了以後大為惱火，認為他不該把寶貴的時間浪費在這種無用的工具上<br><br>
>
> 題外話，這名學生叫 [Donald B. Gillies](https://siebelschool.illinois.edu/about/awards/faculty-awards/chairs-and-professorships/donald-b-gillies-chair-computer-science)

真硬核XD...

一些二進制檔案會以特定的格式組成，通常這些格式會有指定的 headers，內部通常會有個簽名，稱為 Magic Numbers，github 上有人整理一張表，有興趣的可以看看：[File Magic Numbers](https://gist.github.com/leommoore/f9e57ba2aa4bf197ebc5)

headers 內不只包含了簽名，還會擁有一些輔助解析該格式的資料，例如 GIF 檔案可以包含多個圖像，而它的 header 內就會有用於標識和描述每個圖像的資料

OS 會利用副檔名來去選擇預設的對應程式來開啟該檔案，例如副檔名是 `.gif`，那預設就會使用 GIF Viewer 之類的程式來開啟該檔案，而這類對應的程式往往能夠根據該格式來正確解析出此二進制檔，進而達成我們想要的目的

如果我們使用了不正確的副檔名，那就會導致預設上會使用不正確的工具來打開該二進制檔，那結果通常就是該工具無法解析給定的二進制檔，從而導致錯誤，例如我於 windows 的系統上用一個記事本打開一個 `.exe`，那看到的結果就會是記事本內顯示了一堆亂碼。 雖然這些亂碼看起來毫無意義，但本質上他們只不過是 Byte sequence，如果用正確的工具打開並解析它，那就能得到預期的結果

我們能夠使用 HxD 或是 Vscode 內的 Hex Viewer 等工具來觀察二進制檔，這些工具會將 Byte sequence 利用八進制/十六進制等方式顯示出來。 假設我們於 windows 系統上利用 HxD 打開一個會印出 "Hello World!" 字串的 `.exe` 檔，那結果會像這樣：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/binary-file.png" width = 70%><br>

</div>

### 可執行檔 Executable File

從剛剛的圖中，我們可以看見最一開始的地方擁有 `4D 5A` 這個簽名，這是 windows 上的執行檔簽名，格式被稱為 PE-file format。 現今的執行檔主要有兩種格式，一種為 ELF，主要在 Unix 與 Unix-like 的 OS 上被使用；另一種則為 PE，主要用在 windows 的 OS 上

對於這兩種格式，這邊不打算多做展開，因為這展開還蠻複雜的，但是之前因為我在數學系的專題是刻 boosting 演算法和 Random Forest 演算法去偵測一個 PE File 是否為惡意軟體，所以有稍微讀過 PE File 的細節，有興趣的可以看看我的筆記：[PE FILE](https://hackmd.io/@Mes/PE_File_Format)

另外，如果對 windows 上的行為，包括這邊講的 PE File，又或是 windows OS 本身有興趣的，可以去讀讀看馬哥的書：[Windows-APT-Warfare](https://github.com/aaaddress1/Windows-APT-Warfare)

而對於 Linux，去修 jserv 老師的 Linux 核心設計啦XDD

最後附上兩種執行檔格式的細節，我們通常會稱這個為 File ayout：

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/ELF-format.png" width = 80%>

(ELF File layout)   
(source：[Wikipedia](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#/media/File:ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini.png))

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Computer_Introduction/image/PE-format.png" width = 80%>

(PE File layout)    
(source：[Wikipedia](https://en.wikipedia.org/wiki/Portable_Executable#/media/File:Portable_Executable_32_bit_Structure_in_SVG_fixed.svg))

</div>

## Reference

- [(Wikipedia) AMD Fusion Controller Hub](https://zh.wikipedia.org/zh-tw/AMD_Fusion_Controller_Hub)
- [(Wikipedia) Arithmetic logic unit](https://en.wikipedia.org/wiki/Arithmetic_logic_unit)
- [(Wikipedia) BIOS](https://en.wikipedia.org/wiki/BIOS)
- [(Wikipedia) Binary file](https://en.wikipedia.org/wiki/Binary_file)
- [(Wikipedia) Bus](https://en.wikipedia.org/wiki/Bus_(computing))
- [(Wikipedia) Central processing unit](https://en.wikipedia.org/wiki/Central_processing_unit)
- [(Wikipedia) Central processing unit](https://en.wikipedia.org/wiki/Central_processing_unit)
- [(Wikipedia) Channel I/O](https://en.wikipedia.org/wiki/Channel_I/O)
- [(Wikipedia) Computer architecture](https://en.wikipedia.org/wiki/Computer_architecture)
- [(Wikipedia) Control bus](https://en.wikipedia.org/wiki/Control_bus)
- [(Wikipedia) Control unit](https://en.wikipedia.org/wiki/Control_unit)
- [(Wikipedia) DDR Interface Protocol DDR](https://electronics.stackexchange.com/questions/465749/ddr-interface-protocol)
- [(Wikipedia) DDR2 SDRAM](https://en.wikipedia.org/wiki/DDR2_SDRAM)
- [(Wikipedia) DDR3 SDRAM](https://en.wikipedia.org/wiki/DDR3_SDRAM)
- [(Wikipedia) DDR4 SDRAM](https://en.wikipedia.org/wiki/DDR4_SDRAM)
- [(Wikipedia) Direct Mdeia Interface](https://en.wikipedia.org/wiki/Direct_Media_Interface)
- [(Wikipedia) Dynamic random-access memory](https://en.wikipedia.org/wiki/Dynamic_random-access_memory)
- [(Wikipedia) EEPROM](https://en.wikipedia.org/wiki/EEPROM)
- [(Wikipedia) Executable](https://en.wikipedia.org/wiki/Executable)
- [(Wikipedia) Flexible Display Interface](https://en.wikipedia.org/wiki/Flexible_Display_Interface)
- [(Wikipedia) Front-side bus](https://en.wikipedia.org/wiki/Front-side_bus)
- [(Wikipedia) Instruction set architecture](https://en.wikipedia.org/wiki/Instruction_set_architecture)
- [(Wikipedia) Memory Controller](https://en.wikipedia.org/wiki/Memory_controller)
- [(Wikipedia) Memory management unit](https://en.wikipedia.org/wiki/Memory_management_unit)
- [(Wikipedia) Microarchitecture](https://en.wikipedia.org/wiki/Microarchitecture)
- [(Wikipedia) Motherboard](https://www.sciencedirect.com/topics/engineering/motherboard)
- [(Wikipedia) Northbridge (computing)](https://en.wikipedia.org/wiki/Northbridge_(computing))
- [(Wikipedia) Option ROM](https://en.wikipedia.org/wiki/Option_ROM)
- [(Wikipedia) PCI Express](https://en.wikipedia.org/wiki/PCI_Express)
- [(Wikipedia) Platform Controller Hub](https://en.wikipedia.org/wiki/Platform_Controller_Hub)
- [(Wikipedia) Portable Executable](https://en.wikipedia.org/wiki/Portable_Executable)
- [(Wikipedia) Power-on self-test](https://en.wikipedia.org/wiki/Power-on_self-test)
- [(Wikipedia) Processor (computing)](https://en.wikipedia.org/wiki/Processor_(computing))
- [(Wikipedia) Southbridge (computing)](https://en.wikipedia.org/wiki/Southbridge_(computing))
- [(Wikipedia) Static random-access memory](https://en.wikipedia.org/wiki/Static_random-access_memory)
- [(Wikipedia) System Bus](https://en.wikipedia.org/wiki/System_bus)
- [Anyone know what this part of my motherboard is?](https://www.reddit.com/r/pcmasterrace/comments/1c77mbm/anyone_know_what_this_part_of_my_motherboard_is/)
- [Basic Knowledge of Industrial Computers (CPU/Chipset)](https://www.contec.com/support/basic-knowledge/edge-computing/cpu/)
- [Cisco Data Center Virtualization Server Architectures](https://www.ciscopress.com/articles/article.asp?p=1606902&seqNum=2)
- [How does cpu communicate with peripherals?](https://stackoverflow.com/questions/6852332/how-does-cpu-communicate-with-peripherals)
- [John Louis Von Neumann](https://history.computer.org/pioneers/von-neumann.html)
