{%hackmd aPqG0f7uS3CSdeXvHSYQKQ %}

# 前言

在閆令琪老師的 GAMES202 的第一堂課中，老師有一段話我覺得講得蠻好的：

> 我們在學 202(課名) 的過程中一定要記得這件事情，科學與技術是不等同的，當然這件事情是我個人的理解，我認為科學和技術是嚴格意義上不等同的兩樣事情。 科學代表著知識，代表你對於這個世界應當如何去運作的理解；然後技術是什麼呢? 技術更多代表的是技巧，就是所謂的 engineering skills，工程能力啊。 然後技術是用來幹什麼的呢? 是要把科學轉換為產品的這麼個意思
>
> 而科學和技術通常人們會簡稱科技，這是為什麼呢? 這是因為絕大多數人認為這兩件事情，反正我都不懂，所以像是一回事對吧，但對於我們來說，要區分一下 science 和 technology。 
>
> 一個最典型的例子就是「車」對吧，大家知道汽車這個東西，大家在很久之前，就已經知道應該怎麼樣去運作了，但是直到今天，咱們國產的車子性能仍然不是特別那個對吧(XD)，所以說啊，這就是「技術」在背後起到的關鍵作用
>
> 所以說科學和技術不等同，但我沒有說哪一項更重要，技術和科學我可以認為他們同等重要

對於工程的問題，我們通常會有 spec 來嚴格定義我們所想要的行為，遇到什麼狀況需要做什麼事。 而我個人感覺學校在教「科學」類型的課程時，通常不會講得這麼嚴謹，因為這類的系統比較開放，你很難像工程的問題那樣對所有的事都做嚴格定義

換句話說就是比較模糊，而這類問題通常數學就會多，也因此我個人覺得「數學」其實是門很模糊的語言，用 CS 的角度去看很多東西的定義都有點模糊，也因此想要用數學把想法描述清楚是一件相對困難的事情，數學系還會花特別一學期來教你怎麼做這件事，通常是基礎數學或是數學導論之類的大一必修課

當然這個是我個人的理解，數學比我好的人多的是，也許他們又會有不同的想法。 但至少與朋友閒聊時我發現有些蠻厲害的工程師也跟我有同樣的觀點，所以我想這至少不是只有我一個人的想法，蠻有趣的

對於語言這個東西，以我的經驗來說，資工系大部分的課就是教 C/C++ 或 python，而教法不外乎就是開始從變數、型態、函式、class 這樣一路講一下概念，帶幾個例子後出個作業就過去了

換句話說，我覺得這些課的教法很接近於「科學」類型的課程的教法，講講概念，讓你看幾個例子有個經驗，出幾個作業讓你熟悉一下這個「感覺」。

然而語言是一個工程類的東西，它是有 spec 的，裡面嚴格定義了什麼樣的輸入該有什麼樣的輸出，因此學習語言的過程你應該要去讀 spec，而不是依靠「經驗」，它不是一個開放的系統，它是一個人定義出來的封閉系統。

換句話說學語言跟學下棋是很類似的，例如西洋棋，在學的時候你需要先理解棋子移動的規則，而且你需要去遵守這些規則，所以你會去看西洋棋的規則書/說明書(spec)，而不是找個人來告訴你棋子移動的「經驗」法則。 

同樣地，你需要去遵守語言 spec 裡面的規則，而不是自己去嘗試跑一次你的 code，發現會動後就認為那樣的寫法是對的，甚至是認為不需要遵守 spec 內的規則，會動與正確是兩件事情，在工程上我們會利用 UB、IB 等規則來對這種錯誤的寫法去做規範，之後有機會的話會再講到。

另外，要如何利用這些規則來贏遊戲，也就是所謂的戰術，就需要依賴經驗了，這在語言的部分也是一樣的，要怎麼寫出好的程式碼也需要經驗，可能會有一些前人發現的法則、規律，但它們是可調整的，就像下棋一樣，你可以自己去研發自己的套路。

總而言之，寫語言記得翻 spec，遵守 spec，語言是被嚴格定義好地，你不該利用經驗法則去判斷一個寫法是否正確

要注意我並不是指「科學」較淺，「技術」較深，而是指這兩種東西的面向不同，學習方法也理當有差，不該混用

這個礦坑系列的教學會時不時地引用 spec 內的定義，來教新手 C\+\+，由於 C\+\+ 相對其他語言來說較為底層，也就是說比較接近硬體，抽象化的程度比較少，因此在進入 C\+\+ 的教學之前，我覺得最好先寫一篇對電腦運作的簡述，帶大家簡單學習一下計算機概論

# 抽象化 Abstraction

## 抽象化是什麼

工程的目的是使用科學，科學提供了對自然現象的理解，而抽象則是對現象做包裝的一種方法，例如生活中的「顏色」，顏色是人的感官對於特定波長的電磁波的解釋，但當我們平常聊天在講「紅色」時不會特別地去強調「620-750nm 的電磁波」的這個資訊，更多是在強調人的感官的解釋。

換句話說，「紅色」這個詞是對「620-750nm 的電磁波」所做的抽象化，它對「620-750nm 的電磁波」做了包裝。 在用「紅色」這個詞的時候，我們把「620-750nm 的電磁波」、人眼的構造與大腦對人眼訊號的解析等等地細節都隱含的省略掉了，因為我們只在意 620-750nm 的電磁波對我們造成的感官結果。這讓我們在對話的過程中可以很簡單的達到共識，抓到重點。 

所以，用一句話來描述抽象化的概念就是「不管中間細節，只在乎輸入與輸出的關係」，我們不在意中間過程的細節，想把它全部忽略掉，更多的時候我們會說把它當成一個黑箱，我們只想知道丟了一個東西進去，會有什麼樣的結果出來

再舉個我很喜歡的例子，假設我們要去餐廳點餐，那我們在意的重點是「我要付多少錢才可以拿到我想要的餐點」，我們不在乎餐廳的人員要如何跟廚房溝通、廚師有幾人、用的調味料產自何處和他們要如何製作這份餐點，只要我付完錢，我要的餐點會來，這樣就可以了

畫成圖來說就是這樣：

![image](https://hackmd.io/_uploads/BJxd-y0TC.png)


我不在乎中間那個框框(黑箱)內做了什麼，我只在乎我的錢進去框框後，我的飯會從框框內出來

## 電路抽象化

學電機的人會利用 Maxwell's equations 來研究電磁現象，然後在 Maxwell's equations 之上創建一個新的抽象層，稱為集總電路(lumped circuit abstraction)，利用集總電路我們可以再增加一個新的抽象層，稱為數位層(Digital abstraction)，而利用數位電路又可以再增加一層邏輯閘層(Logic gate abstraction)，以此類推，可以分好多好多層下去，這些不同的抽象層面就對應到電腦科學中不同的專業領域：

![image](https://hackmd.io/_uploads/BkVd-kAaR.png)

這邊的分類是我照著我電子電路學課本「foundation of analog & digital electronic circuits by anant agarwal」內的分類畫出來的，課本的分類我覺得合理。 這中間的每一層都可以再各自有其細分下去的抽象層，而 Programming language 的上方就是各式各樣的程式了。 

然後我大概把資工、電機系內對應的課大概寫了一下，只寫了我有摸過、聽說過的，有些理解的不深，如果有錯麻煩再糾正我一下XD

# 圖靈機 Turing machine

圖靈機是一個在自動機理論與計算理論中定義的數學模型，因為是計概所以我這邊不寫它的數學定義，有興趣的可以看一下自動機理論這門課

我覺得 wiki 上的描述寫得很好，所以讓我們看一下 [wiki](https://zh.wikipedia.org/wiki/%E5%9B%BE%E7%81%B5%E6%9C%BA) 上的描述：

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
4. 一套控制規則數量有限的表格(TABLE)。 它根據當前機器所處的狀態以及當前讀寫頭所指的格子上的符號來決定讀寫頭下一步的動作，並改變狀態暫存器的值，讓機器進入一個新的狀態。 
    
    其會按照以下順序告知圖靈機命令：
    1. 寫入(替換) 或擦除當前符號
    2. 移動讀寫頭，有三種移動方法：「L」表示向左，「R」表示向右，「N」表示不移動。 每次最多移動一格。
    3. 保持當前狀態或者轉到另一狀態。

注意這個機器的每一部分都是有限的，但它有一個潛在的無限長的紙帶，因此這種機器只是一個理想的裝置。 圖靈認為這樣的一台機器就能類比人類所能進行的任何計算過程

另外這篇也寫得很好，也更詳細，建議可以讀一下：[搞懂「通用圖靈機」的終站——它的誕生與意義](https://pansci.asia/archives/203152)

而在現代，我們所謂的「電腦」，或正式一點稱它為「計算機」，其實就是圖靈機的一種實作方法

- 語言的規範是針對圖靈機的

# 馮諾伊曼架構 Von Neumann architecture

- 電腦是一種圖靈機的實作

# CPU & Memory

- 與圖靈機的對應
- CPU 架構
- ISA

# BIOS & OS

## BIOS

### EEPROM

## OS

### 系統呼叫 System call

# ELF & PE File

## 二進位檔案 Binary File

## 可執行檔 Executable File

## 副檔名 Filename Extension