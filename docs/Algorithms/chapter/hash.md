
# 哈希表

# 表结构

一张表存放的数据结构为 `key:value` 的形式，通过 `key` 可以进行一系列基本操作
- 根据 `key` 查找 `value`
- 插入 `key : value` 数据
- 删除 `key : value` 数据

# 哈希表


表结构最简单的实现方式是利用指针数组
- 数组索引为 `key`
- 元素为 `value` 的指针

**但是该方式只适用于有限且不重复的 `key`。** 

![alt|c,60](../../image/algorithm/hashTable.png)

哈希表则在数组直接映射表上的基础上进行改进
- 哈希表同样基于数组 $T$ 来关联 `key` 与 `value`
- 数组 $T$ 的索引不在是 `key`，而是 key 的 `hash` 值
- `key` 会经过一次 「`hash` 函数」运算，得到 hash 值，然后用于检索 $T$ 的槽 `slot`
- 由于槽的个数有限（$m$ 个），因此不同的 `key` 可能存在相同的 hash 值，即冲突 `collision`，因此，$T$ 存放的不再是元素指针，而是同 hash 值元素构成的链表

# 简单哈希表

## 分析

**简单哈希表 `simple uniform hash table`** : 所有 `key` 被映射到每个槽的概率相等，即 $\frac{1}{m}$，且每个 `key` 的 hash 计算相互独立。

> [!note]
> 一个 hash 表存在 $n$ 个 key 和 $m$ 个 slot，那么其装载因子为 $\alpha = n/m$ ，即一个槽平均要存放 $ \alpha$ 个 元素

- **最坏搜索时间**

所有的 key 都映射到一个槽上，其搜索时间为 $\Theta(n)$

- **平均搜索时间**

当 key 被均匀映射到每一个槽，其搜索时间为 $\Theta(1 + \alpha)$。当 $\alpha = O(1)$ (此时 $n = O(m)$)时，搜索时间为 $\Theta(1)$


## 哈希函数

> [!tip]
> 对于一个通用的哈希函数需要满足
> - key 进行 hash 计算后，能被均匀映射到每一个槽上
> - key 的特有分布规律，不会影响 hash 函数计算结果的分布规律

### 除法哈希函数

$$
   h(k) = k \ \text{mod} \ m
$$

-  $m$ 值不要选择太小的

当选择 $m = 2$ 时，如果 key 都是偶数，就会导致所有的元素都集中在一个槽中

- $m$ 值不要使用 $2^r$ 或者 $10^r$

假设 key 的长度为 `16 bits` 且 $m = 2^4$。采用除法哈希函数，对于一个 key 而言，只有低 4 位才会参与 hash 计算

### 乘法哈希函数

计算机内存地址长度为 $w$ 位，哈希表的槽数为 $m = 2^2$

$$
    h(k) = A k \ \text{mod} \ 2^w >> (w - r)
$$

其中 $A$ 为奇数，且满足 $ 2^(w-r) < A < 2^w$

![alt|c,50](../../image/algorithm/multiHash.png)

其计算结果就相当与在 $[0,2^r]$ 范围内绕圈

![alt|c,50](../../image/algorithm/multiHashExample.png)


# 开放寻址

## 设计思路
 
简单哈希表方案采用「链表」解决同哈希值的 `collision`，而开放寻址法 `open addressing` 则保证每一个值都会有一个槽进行存放

1. 槽的个数要大于键的个数，即 $m \ge n$
2. 哈希函数 $h(k,i)$ 不是生成一个 hash 值，而是生成一个关于槽的固定「检索序列 `probe sequence`」，用于装填哈希表
3. 表插入：$h(k,i)$ 会从 $i = 0$ 开始，根据 key 生成 $T$ 的索引 $j$，直到找到空闲的槽，并将 key 放入空闲槽
4. 表检索：$h(k,i)$ 会从 $i = 0$ 开始，根据 key 生成 $T$ 的索引 $j$，直到找到目标 key

![alt|c,70](../../image/algorithm/openaddressing.png)

>[!note]
> - 采用开放寻址设计哈希表，删除操作对表结构影响较大
> - 只有相同 key 肯定会遍历「索引序列」，不同 key 通常只需一次哈希计算便能找到槽 （若槽被其他相同 key 占用，那还是得遍历 「索引序列」）

## 哈希函数

- **线性哈希**

$$
    h(k,i) = [ h(k,0) + i ] \ \text{mod} \ m
$$

使用线性哈希函数存在「一次集群`primary clustering`」问题，如果哈希表的某段区间的槽都被占满，那么下一次添加元素，就需要把这些槽的都得遍历一边，才能找到空闲槽 (**探索序列是由 $h(k,i)$ 递推调用生成的**)

- **二次哈希**

$$
    h(k,i) = [h_1(k)+ i \cdot h_2(k)] \text{mod} \ m
$$

$h_1(k)$ 与 $h_2(k)$ 是两个不同的辅助哈希函数，比较常用的方式


## 耗时分析

- **失败搜索平均时间** : 需要搜索的 key 在哈希表中不存在

> **假设：**
> 1. 所有的 key 都存在 $m!$ 种检测序列
> 2. 所有 key 相互独立


使用 $A_i$ 表示第 $i$ 次的搜索结果，槽存在值的事件；

$$
    Pr\{A_i\} = \begin{cases}
        \frac{n - i + 1}{m - i + 1} , i \le n \le m\\
        0, other
    \end{cases}
$$

$X$ 表示未找到 key，所进行的探测次数。

- 探索 $i$ 次，搜索失败

$$
    \begin{aligned}
        Pr\{X = i\}  &=  Pr\{A_1 \cap A_2 \cap \dotsm \cap A_{i-1}\}  \\
                     &= \frac{n}{m} \cdot \frac{n-1}{m-1} \cdot \frac{n-2}{m-2} \cdots \frac{n-i+2}{m-i+2} \\
                     &\le \frac{n}{m}^{i - 1} \\
                     &= \alpha^{i -1}
    \end{aligned}
$$

- 探索 $> i$ 次，搜索失败，该情况不可能出现（$i$ 次搜索失败，就退出搜索），因此 $Pr\{X > i\} = 0$

综上所述，$Pr\{X \ge i\} \le \alpha^{i-1} $ 成立。计算失败搜索次数的期望

$$
    \begin{aligned}
        E[X] &= \sum_{i=0}^{\infty} i \cdot Pr\{X = i\} \\
             &= \sum_{i=0}^{\infty} i (Pr\{X \ge i\})  - Pr\{ X \ge i + 1\} \\
             &= Pr\{X \ge 1\} - Pr\{X \ge 2\} + 2[Pr\{X \ge 2\} - Pr\{X \ge 3\}]  + 3[Pr\{X \ge 3\} - Pr\{X \ge 4\}] + \dotsm \\
             &= \sum_{i=1}^{\infty} Pr\{X \ge i\} \\
             &\le \sum_{i=1}^{\infty} \alpha^{i-1} \\
             &= \sum_{i=0}^{\infty} \alpha^{i} \\
             &= \frac{1}{1 - \alpha} \\
    \end{aligned}
$$



