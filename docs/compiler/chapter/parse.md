# 语法解析

# 介绍

有限状态机无法解析嵌套规则、括号匹配、目标定位，因此需要解析器 `parsing` 进行语法解析
- 输入：通过词法解析获取的 token 序列
- 输出：解析树


![alt|c,50](../../image/compiler/parse_example.png)


> [!tip]
> 词法分析得到的是全部 token 序列，因此，需要解析器区分出哪些 token 有效，哪些 token 无效。

# 上下文无关语法

上下文无关语法 `Context-Free Grammars` : 对递归结构的自然描述。其构成为
- 终结符集合 `terminals` : $T$
- 非终结符集合 `non-terminals` : $N$
- 开始符号 ： $S, S \in N$
- 生产式集合 `production` : $X \rightarrow Y_1 Y_2\dotsm Y_n \quad x \in N, y_i \in N \cup T \cup \{\varepsilon\}$
    1. 生产式只能从 $S$ 开始
    2. 右边的符号 $Y_1 Y_2\dotsm Y_n$ 可以替换左边的符号 $X$
    3. 重复步骤2，最终得到的字符串结果将不存在非终结符，即 $S \overset{*}{\rightarrow} a_1\dotsm a_n, \ a_i \in T$

定义 $G$ 代表开始符号是 $S$ 的上下文无关语法，那么 $L(G)$ 

$$
    L(G) = \{ a_1 a_2 \dotsm a_n | a_i \in T, S \overset{*}{\rightarrow} a_1 a_2 \dotsm a_n \}
$$



**案例**：为起始符号 `EXPR` 定义了三种生产式，`|` 表示或，终结符都使用了小写

![alt|c,60](../../image/compiler/CFG_example.png)

根据上述定义，以下语法结构都将合法

```cool
-- EXPR -> id
id

-- EXPR -> while EXPR loop EXPR pool
while id loop id pool

-- if 与 while 嵌套
if id then
    while id loop id pool
else
    if id then id else id if
if
```

> [!note]
> - 完全展开生产式得到的最终字符串都由终结符组成，字符串将不在改变，**因此终结符应当就是语言的 `token`**
> - CFG 只定义了正确的语法，但并未实现解析树的生成


# 解析树

## 定义

**推导 `Derivation`** : 描述一系列生产式递归展开的过程

$$
    S \rightarrow \dotsm \rightarrow \dotsm \rightarrow \dotsm
$$

推导流程可以绘制成一棵树，该树则被称之为「解析树 `Parse Tree`」
- 根结点 $S$ 
- 叶结点均是终止符
- 非终止符只存在于内部节点
- 生产式 $X \rightarrow Y_1\dotsm Y_n$ ，$Y_1 \dotsm Y_n$都是 $X$ 结点的子结点
- 叶子节点按照中序遍历得到的字符串就是原输入字符串
- 树的层级关系代表了表达式的优先级

**案例**： $E \rightarrow E + E | E * E | (E) | id$，字符串 `id * id + id` 的解析树结构


![alt|c,60](../../image/compiler/parseTreee.png)


一般使用的两种推导方式
- 左推导 `left-most derivation` :  生产式每一步展开，都先替换最左边的非终止符号

    ![alt|c,20](../../image/compiler/leftmost.png)

- 右推导 `left-most derivation` :  生产式每一步展开，都先替换最右边的非终止符号

    ![alt|c,20](../../image/compiler/rightmost.png)

## 歧义

**歧义 `Ambiguity`** : 对于同一个字符串，根据同一个语法会获取到的不同的解析树。

- **重写语法消除歧义**。

**案例一：** 字符串 `id * id + id` 根据 $E \rightarrow E + E | E * E | (E) | id$ 描述就存在歧义

![alt|c,60](../../image/compiler/gammarAmbiguity.png)

拆分语法规则 $E$ 处理运算层级低的加法，$E'$ 则处理优先级高的乘法与括号

$$
    \begin{aligned}
        E &\rightarrow E' + E | E' \\
        E' &\rightarrow id * E' | id | (E) * E' | (E)
    \end{aligned}
$$

且 $E \rightarrow E' + E$ 实现了递归

$$
    E \rightarrow E' + E \rightarrow E' + E' + E \rightarrow E' + E' + E' + E \rightarrow \dotsm 
$$

**案例二**: $E \rightarrow \text{if E then E | if E then E else E | OTHER}$ 解析条件判断，对于字符串

```cool
if E1 then if E2 then E3 else E4
```
将存在歧义

![alt|c,60](../../image/compiler/parseIfAmbiguity.png)

解决方式也是重写语法规则，使得 `else` 只与最近的 `then` 匹配


$$
    \begin{aligned}
        E \rightarrow&  \text{ MIF    all then are matched } \\
                    |&  \text{ UIF    some then is unmatched}  \\
        \\
        MIF \rightarrow& \text{ if E then MIF else MIF} \\
                      |& \text{ OTHER} \\
        \\
        UIF \rightarrow& \text{ if E then E} \\
                      |& \text{ if E then MIF else UIF} \\
    \end{aligned}
$$

- **通过歧义消除机制防止歧义**

重写语法规则难度很大，而且重写后的规则也晦涩难懂 （例如上述条件判断），为了使得定义语法规则更加方便，**语法解析工具提供了歧义消除机制，可以定义额外的规则**。 

![alt](../../image/compiler/ambiguityPrecedence.png)

`%left` 限定了运算符 `+ *` 只能左推导，且通过顺序确定 `*` 的运算优先级比 `+` 的优先级高

## 异常处理

编译器的目的
1. 检测出无效的代码
2. 将代码翻译成目标结果

在检测阶段就需要编译器具有向程序报告代码错误的能力，例如无效符号、语法错误、语义错误等。现有的异常处理机制有
- `panic mode` 

当错误被检测到时，解析器丢弃当前的 token，直到找到正确的 token 为止，并继续解析。例如 `( 1 + + 2) + 3` ，当检测到第二个 `+` 时，解析器发现异常，然后丢弃 `+`，并继续解析 `2`。在 `Bison` 提供了终止符号 `error` 来实现跳过异常 token 

$$
    E \rightarrow \text{ int | E + E | (E) | error int | (error)}
$$


- `error production` ： 使用生产式将常见的错误语法标记出来，例如编译器产生的 `warning`

- `automatic local or global correction` : 希望编译器能自动纠错


## 抽象语法树


![alt|c,35](../../image/compiler/parseTreeDisadvantage.png)

解析树内容臃肿并不适合进行编程，因此，对其进行改进又提出了抽象语法树 `Abstract Syntax Tree`，只保留关键的数据信息与层级结构

![alt|c,40](../../image/compiler/AST_example.png)


# 自上而下解析算法

> [!tip]
> 解析算法：基于 CFG 定义的规则解析输入的 token 流，其推导流程便是在生成一可棵解析树，
> - token 流的解析树生成成功，满足语法规则
> - token 流的解析树生成失败，语法错误

## 递归下降

### 思想

递归下降 `Recursive Descent` : 自顶向下 `top-down` 的语法解析算法，运行规则
- 从顶部开始
- 从左到右
- 叶子结点的中序遍历结果，就是输入的 token 流


<video src="/image/compiler/recursiveDescent.mp4"  controls="controls" width="700" height="400"></video>

简单算法实现检测字符串 `( 5 )`

1. 定义 token 的类型
2. 全局变量 `TOKEN * next;` 
3. 定义函数
   1. 检测 token 类型的函数 `bool term(TOKEN tok){ return *next++ == tok; }`
   2. 匹配符号 $S$ 对应的第 $n$ 条规则 `bool Sn();`
   3. 匹配 $S$ 对应的所有规则 `bool S();`


伪代码实现

```cpp
enum TOKEN_TYPE_E{
    INT,
    OPEN,   // (
    CLOSE,  // )
    PLUS,   // +
    TIMES   // *
};

struct TOKEN_S{
    TOKEN_TYPE_E type;
    std::string str;
};

class RecursiveDescent{
public:

    // 解析以 start 为起始，并以 end 结束的 token 流程，其范围为 [start, end)
    bool parse(TOKEN_S * start, TOKEN_S * end){
        m_next = start;

        // 1. 根据 CFG 定义，解析就是从非终结符号 E 开始的
        // 2. 输入 token 流 [start, end) 要与语法完全匹配才接受，部分匹配都是错的
        return E() && m_next == end;
    }

private:
    /* 
        E -> T
           | T + E
     */
    bool E1() { return T();}
    bool E2() { 
        // 使用 term(PLUS) 表示 +
        return T() && term(PLUS) &&  E();
    }
    bool E(){
        // 使用 save 实现了回溯
        auto save = next;
        return (next = save, E1())
            || (next = save, E2());
    }

    /* 
        T -> int
           | int * T
           | ( E )
     */
    bool T1() { return term(INT); }
    bool T2() { return term(INT) && term(PLUS) && T(); }
    bool T3() { return term(OPEN) && E() && term(CLOSE); }
    bool T(){
        auto save = next;
        return (next = save, T1())
            || (next = save, T2())
            || (next = save, T3());
    }

    // 输入 type 与当前的 token 类型是否匹配
    bool term(TOKEN_TYPE_E type){
        auto curr = m_next; 
        ++m_next; // 移动
        return curr->type == type;
    }

private:
    TOKEN_S * m_next; // 表示 token 流中，下一个将输入的 token
};
```

上述代码根据递归下降法实现了一个简单的解析器。但是该解析器只能正确匹配出 `int`，`( int )` 等类型的字符串。**当匹配 `int * int`时，`E() -> E1() -> T() -> T1()` 匹配到 `int` 后便直接返回，并未尝试解析 `T2()`，因此最后结论是失败**。

> [!tip]
> - 市面上存在真正通用的梯度下降算法，但是具有复杂的回溯机制，用以解决上述问题，后续的 `LL(1)` 也能解决该问题
> - 递归下降法通过函数递归实现了解析树生成


### 左递归问题

左递归问题 `Left Recursion` : 「递归下降算法」针对 $S \rightarrow S \alpha | \beta$ 的生产式会陷入死循环。

```cpp
bool S1() {return S() && term(a);}
bool S2() {return term(b);}
bool S(){
    auto save = next;
    return (next = save, S1()) 
        || (next = save, S2());
}
```
从 $S$ 进入，就会产生 `S() -> S1() -> S() -> S1() -> ...` 死循环调用。 $S \rightarrow S \alpha | \beta$ 描述的推导流程是 $S \overset{*}{\rightarrow} \beta \alpha \dotsm \alpha$，采用左递归的推导方式，是从右向左生成字符串，因此，要解决该问题只要从左向右生成字符串就行，即改写为右递归 `right recursion`

$$
    \begin{aligned}
        S \rightarrow& \beta S' \\
        S' \rightarrow& \alpha S' | \varepsilon
    \end{aligned}
$$


## 预测解析

### 概念

**预测解析 `Predictive Parse`:** 属于自上而下解析算法，可以预测接下来应该使用哪个生产式
- 会执行 `lookahead` 操作，向前多检测几个 token
- 没有回溯机制
- 适用于 `LL(k)` 文法

**`LL(k) `文法**： **每一步需要检索 k 个 token 后才能确定生产式，且每次最多仅有一个生产式能匹配，通常使用 `LL(1)`**
- `Left to right` : 从左向右扫描 token
- `Left-most derivation` : 左推导

### 左因子分解

在递归下降算法中

$$
\begin{aligned}
    E \rightarrow& \text{ T + E | T} \\
    T \rightarrow& \text{ int | int * T | (E)} \\
\end{aligned}

$$
- `int * int` : 对于 $T$ 而言， $T \rightarrow \text{ int | int * T }$ 都可以匹配字符串，若选择 $ T \rightarrow \text{ int}$ 将导致字符串将匹配失败
- `int + int` : 对于 $E$ 而言，$E \rightarrow \text{ T + E | T}$ 均能匹配字符串，若选择 $E \rightarrow \text{T} $ 也将导致字符串匹配失败

针对上述问题，可以采用 **左因子分解 `left-factor`** 进行处理

$$
    \begin{aligned}
        E \rightarrow& \text{ T + E}\\
          \rightarrow& \text{ T}
    \end{aligned}
$$

两个生产式都具有公共符号 $T$ 导致字符串推导可以具有不同选择，可以将公共部分提取，重新改写生产式，去除二义性

$$
    \begin{aligned}
        E \rightarrow& \text{ T X} \\
        X \rightarrow& \text{ + E | } \varepsilon
    \end{aligned}
$$

对于 $T$ 也一样提取公共 $int$

$$
    \begin{aligned}
        T \rightarrow& \text{ int Y | (E)}\\
        Y \rightarrow& \ \varepsilon \text{ | * T}
    \end{aligned}
$$

### LL(1) 解析表

![alt|c,70](../../image/compiler/LL1_parsingTable.png)

- `$` : 字符串结束的标记

将现有语法规则通过「左因子分解」分解后，可以绘制出 `LL(1)` 预测解析表，可以实现每个 token 至少存在一条生产式
- 当前处于 $E$ ，且输入 `int`，可得 $E \rightarrow \text{ T X}$
- 当前处于 $Y$ ，且输入 `*`，可得 $Y \rightarrow \text{ * T}$
- 当前处于 $X$ ，且输入 `int`, 没有生产式，语法错误


```cpp
// 基于 LL(1) 解析表的 token 流解析算法
bool parse(LL1 table,  Token[] tokens){

    // 初始化
    std::stack<Symbol> stack;
    stack.push('$'); // token 流结束标记
    stack.push('E'); // 解析算法开始的标记

    auto next = tokens;

    do{
        // 还没完成匹配，token 流输入就没了
        if(next == nullptr){
            return false;
        }

        Symbol top = stack.top();

        if(top.isTerminal()){
            if( top == *next++){
                stack.pop();
            }else {
                return false;
            }
        }else if(top.isNonTerminal()){

            // 查询 LL(1) 解析表获取生产式
            auto grammar = table[top, *next];

            // 生产式为空，输入异常
            if(grammar == nullptr) return false;

            stack.pop();

            // 将生产式的符号都入栈
            for(int i = grammer->size() - 1; i >= 0; --i){
                stack.push(grammer[i]);
            }    
        }
    }while(stack.size() > 0)

    return true;
}
```

![alt|c,70](../../image/compiler/LL1_example.png)

### 首集/跟随集

解析表 $T$ 中，在非终止符为 $A$且 token 为 $t$ 的情况下，生产式为 $T[A,t] = \alpha$。$\alpha$ 能匹配输入 $t$ 只存在两种情况
- $\alpha \rightarrow^* t \ \beta$ ：最终展开 $A$ 后得到的 stack 栈顶是 $t$，该情况称之为 $t \in first(\alpha)$
- $\alpha \rightarrow^* \varepsilon$ 且 $S \rightarrow^* ..At..$: 最终展开 $A$ 的结果是 $\varepsilon$；在生产式推导过程中，一定存在符号 $t$ 在符号 $A$ 后面的结果，该情况称之为 $t \in follow(A)$

#### 首集

首集 `First Set` 定义

$$
    First(X) = \{ t | X \rightarrow^* t \alpha\} \cup \{ \varepsilon | X \rightarrow^* \varepsilon \} , \ t \in \text{terminals}
$$


引理：
1. $First(t) = \{ t \}$
2. $\varepsilon \in First(X)$ 的成立条件
    - $X \rightarrow \varepsilon$
    - $X \rightarrow A_1 \dotsm A_n, \ \varepsilon \in Fisrt(A_i)$
3. 若 $X \rightarrow A_1 \dotsm A_n \alpha, \ \varepsilon \in First(A_i)$，则 $First(\alpha) \subseteq Fisrt(X)$
4. 若 $E \rightarrow X \ Y, \ \varepsilon \notin First(X)$，则 $First(E) = First(X)$

![alt|c,60](../../image/compiler/firstSetExample.png)

#### 跟随集

跟随集 `Follow Set` 定义：**在生产式推导过程中，存在 $X$ 在 $t$ 前面的情况，且不包含 $\varepsilon$**

$$
    Follow(X) = \{ t | S \rightarrow^* \beta X t \delta \} , \ t \in \text{terminals}
$$

引理
1. $S$ 是开始符号，则一定存在 $ \$ \in Follow(S) $
2. 若 $X \rightarrow \text{ A B}$，则 $Follow(X) \subseteq Follow(B)$
3. 若 $A \rightarrow \alpha X \beta$， 则 $First(\beta) - \{ \varepsilon \} \subseteq Follow(X)$
4. 若 $A \rightarrow \alpha X \beta, \ \varepsilon \in First(\beta)$，  则 $Follow(A) \subseteq Follow(X)$


**引理4 证明：**

根据生产式规则，多次推导一定能得到 $S \rightarrow^* \omega \alpha X \beta \delta$

- 当 $\beta \rightarrow^* \varepsilon$ 时，$A_1 \rightarrow \alpha X$ 且 $S \rightarrow^* \omega \alpha X  \delta$，则 $Follow(A_1) = First(\delta) - \{ \varepsilon \}$
- 当 $ \beta \neq \varepsilon $ 时，$A_2 \rightarrow \alpha X \beta$ 且 $S \rightarrow^* \omega \alpha X \beta  \delta$，则 $Follow(A_2) = First(\delta) - \{ \varepsilon \}$

无论 $\beta$ 是否为 $\varepsilon$ ，$Follow(A) = First(\delta) - \{ \varepsilon \}$ 均成立。且当 $\beta \rightarrow^* \varepsilon$ 时，$First(\delta) - \{ \varepsilon \} \subseteq Follow(X)$，因此  $Follow(A) \subseteq Follow(X)$ 成立

![alt|c,60](../../image/compiler/followSet_example.png)

### LL(1) 表构建

$T$ 表示根据 CFG 创建的 `LL(1)` 解析表。将生产式 $A \rightarrow \alpha$ 放入表中的规则为
- 若终结符 $t \in First(\alpha)$，则 $T[A,t] = \alpha$
- 若 $\varepsilon \in First(\alpha), \ t \in Follow(A)$，则 $T[A,t] = \alpha$
- 若 $\varepsilon \in First(\alpha), \ \$ \in Follow(A)$, 则 $T[A, \$] = \alpha$


![alt|c,70](../../image/compiler/LL1_parsingTable.png)

### 适用范围

$$
    \begin{aligned}
        S &\rightarrow \text{ S a | b} \\
        Fisrt(S) &=  \text{{ b }} \\
        Follow(S) &= \text{{ a, \$ }}
    \end{aligned}
$$ 

- $S \rightarrow Sa$ 且终结符为 $b$ 时，$b \in First(Sa) = First(S)$，则 $T[S,b] = Sa$ 成立
- $S \rightarrow b$ 且终结符为 $b$ 时，$b \in First(b)$，则 $T[S,b] = b$ 成立

综上 $T[A,b]$ 存在两个生成式，违反 `LL(1)` 文法规则，因此 $T$ 并不是 `LL(1)` 解析表。

> [!note]
> - 预测解析算法只适用于 `LL(1)` 文法，判断 CFG 是否为 `LL(1)`，则观察解析表 $T[S,t]$ 是否映射到多值
> - 大多数语言的 CFG 都不满足 `LL(1)` 文法

# 自下而上解析算法

## 介绍

自下而上算法 `Bottom-up` 相较于自上而下算法适用性更广，是现代解析器首选方案。
- 性能更好
- 不需要生产式左因子化
- 对生产式的限制更低，表达上可以更加人性化一些

自下而上算法是扫描字符串 token，从解析树的叶子结点开始，从下往上还原生产式，直到起始符号停止。将字符串还原成生产式的操作称之为 「归约 `reduce`」，归约与生产 `product` 相对应，是生产的反向操作。

$$
    \begin{aligned}
        E \rightarrow& \text{ T + E | T}\\
        T \rightarrow& \text{ int * T | int | (E)}
    \end{aligned}
$$

- 归约：将字符串 token 还原成生产式，例如输入字符串 `int ` 可以反向推理出 $T \rightarrow  \text{ int }$
- 生产：将生产式展开字符串，例如字符串 `int + int` 的推理展开流程为 $E \rightarrow T + E \rightarrow int  +  E \rightarrow int + T \rightarrow int + int$



<video src="/image/compiler/reduce.mp4"  controls="controls" width="700" height="400"></video>



> [!note]
> 自下而上算法是在反向追踪右推导的生产路径

![alt](../../image/compiler/bottomUp.png)