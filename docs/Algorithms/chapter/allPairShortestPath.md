# 全对最短路径

# 问题

## 描述

**全对最短路径 `all pair Shortest paths`:** 已知有向图 $G(V,E)$，点的集合为 $V \in \{1,2,\dotsm,n\}$，每条边的权重为 $w:E\rightarrow R$，求解 $n \times n$ 的矩阵，矩阵元素表示两个点间的最短距离 $\delta(i,j) \quad i,j \in V$


## 思路

求解全部最短路径，最基本的想法就是把图中每一个点都当作「源点」，然后跑一遍最短路径算法
- 无权图： $|V|$ 次广度优先搜索，耗时 $O(VE)$
- 非负权图：$|V|$ 次 `Dijkstra`，耗时 $O(VE + V^2 \lg V)$
- 无负环图：$|V|$ 次 `Bellman-Ford`，耗时 $O(V^2E)$


> 当有向图为稠密的连通图时 $E = \Theta(V^2)$ ，使用 $|V|$ 次 `Bellman-Ford` 耗时就变为 $O(V^4)，后面介绍的算法也是针对这个问题进行改进。


# Bellman-Ford 改进

## 最优子结构

定义 $d_{ij}^{(m)}$ 表示使用 $\le m$ 条边时，从点 $i$ 到点 $j$的距离。当 $m = k$ 时 $d_{ij}^{(k)} = \delta(i,j)$，那么 $m \ge k$ 的值肯定都一样 (因为 $\delta(i,j)$ 是图 $G$ 中的客观结果)
$$
    d_{ij}^{k} = \dotsm = d_{ij}^{n-1} = d_{ij}^{n} = \dotsm = \delta(i,j)
$$

使用邻接矩阵 $A_{n \times n}$ 来表示有向图 $G$，即 $a_{ij} = w(i,j)$，并借助 `Bellman-Ford` 的思路可进一步得到

$$
    \begin{aligned}
        d_{ij}^{(0)} &= \begin{cases}
            0 \quad \text{if i=j} \\
            \infty \quad\text{else}
        \end{cases}\\
        d_{ij}^{(m)} &= min \{ d_{ik}^{(m-1)} + a_{kj} \}, m \in \{1,2,3,\dotsm,n-1\}
    \end{aligned}
$$

根据递推式便能通过 `for` 循环嵌套的方式实现最暴力的算法

```python
# range(a,b) 的取值范围 [a,b]
for m in range(1, n):
    for i in range(1, n):
        for j in range(1, n):
            for k in range(1, n):
                # relax 操作
                if d[i][j] > d[i][k] + a[k][j]:
                    d[i][j] = d[i][k] + a[k][j]
```

## 矩阵乘法

> [!tip]
> 上述关于 $i,j,k$ 的嵌套循环与「矩阵乘法」类似 
$$
\begin{aligned}
    C &= A B \\
    c_{ij} &= \sum_k a_{ik} \cdot b_{kj}
\end{aligned}
$$

模仿矩阵乘法利用符号 $\otimes$  代表 $d_{ij}^{(m)} = min \{ d_{ik}^{(m-1)} + a_{kj} \}$ 运算，那么可以得到

$$
    D^{(m)} = D^{(m - 1)} \otimes A
$$

如果在把上式展开
$$
    \begin{aligned}
        D^{(m)} &=  D^{(m - 2)} \otimes A \otimes A \\
        D^{(m)} &=  D^{(m - 3)} \otimes A \otimes A \otimes A \\
        \dotsm\\
        D^{(m)} &= D^{(0)} \otimes A^{{m}} 
    \end{aligned}
$$

根据 $\otimes$ 运算定义，$D^{(0)}$ 其实就是单位矩阵

$$
    D^{(m)} = A^{{m}} 
$$

> [!note]
> **最终可知 $D^{(m)}$ 其实就是对 $A$ 做 $m$ 次关于 $\otimes$ 幂运算。**

## 负环检测

- **方案一** : 计算 $D^{(m + 1)}$ 比较 $D^{(m)}$ 值是否改变
- **方案二** : 看 $D^{(m)}$ 的对角线是否存在负数


# Floyd-Warshall 算法


定义 $c_{ij}^{(k)}$ 表示从 $i$ 到点 $j$ 的距离，$k$ 表示路径所经过的点数。当 $k = b$ 时 $c_{ij}^{(b)} = \delta(i,j)$，那么 $k \ge b$ 的值肯定都一样 (因为 $\delta(i,j)$ 是图 $G$ 中的客观结果)
$$
    c_{ij}^{b} = \dotsm = c_{ij}^{n-1} = c_{ij}^{n} = \dotsm = \delta(i,j)
$$

仿照前文思路，使用邻接矩阵 $A_{n \times n}$ 来表示有向图 $G$，即 $a_{ij} = w(i,j)$， 得到递推式

$$
    \begin{aligned}
        c_{ij}^{(0)} &= a_{ij} \\
        c_{ij}^{(k)} &= min \{c_{ij}^{(k-1)}, c_{ik}^{(k-1)} + c_{kj}^{(k-1)} \}
    \end{aligned}
$$

从式子中可以看出只涉及三个下标 $i,j,k$ 因此算法只有三层循环，时间复杂度 $\Theta(V^3)$

```python
# 初始化
c = A
# range(a,b) 的取值范围 [a,b]
for k in range(1, n):
    for i in range(1, n):
        for j in range(1, n):
                # relax 操作
                if c[i][j] > c[i][k] + c[k][j]:
                    c[i][j] = c[i][k] + c[k][j]
```

# Johnson 算法

我们会使用 `Bellman-Ford` 算法，而不使用 `Dijkstra` 算法，是因为 `Dijkstra` 不支持负数权值，若能调整图的权值，使其全部 $ \ge 0$，那么就能满足 `Dijkstra` 的使用条件。因此对有向图的权值进行重映射，对于边 $(u,v) \in E$ 的权值通过映射函数 $h(x)$ 改写为如下形式

$$
    w_h(u,v) = w(u,v) + h(u) - h(v)
$$

路径 $p = \{v_0,v_1,\dotsm, v_k\}$ 距离

$$
    \begin{aligned}
        W_h(p) &= \sum_{i=1}^{k} w_h(v_{i-1},v_i) \\
               &= \sum_{i=1}^{k} (w(v_{i-1}, v_i) + h(v_{i-1}) - h(v_i)) \\
               &= \sum_{i=1}^{k} w(v_{i-1}, v_i) + h(v_0) - h(v_k) \\
               &= W(p) + h(v_0) - h(v_k)
    \end{aligned}
$$

可以看出对边进行权值重映射后，对于路径距离的影响就是对端点进行映射。**现在的目的便是获得能够使所有边权值都能 $ \ge 0$  的 $h(x)$**，即

$$
    w_h(u,v) = w(u,v) + h(u) - h(v) \ge 0
$$

变换上述不等式

$$
    h(v) - h(u) \le w(u,v)
$$

这样就能得到关于 $h(x)$ 的约束图，然后通过求解约束图问题就能找到 $h(v_i) = \delta(s, v_i)$，约束图成立的条件也是图中不能存在负环。`Johnson` 算法的流程便是
1. 根据 `Bellman-Ford` 算法求解约束图问题获取 $h(x)$
2. 根据 $h(x)$ 重新映射权值得到 $w_h(u,v)$
3. 对 $w_h(u,v)$ 执行 $|V|$ 次的  `Dijkstra` 算法
4. 将获取的 $\delta_h(i,j)$ 根据 $h(x)$ 还原成 $\delta(i,j)$

最终算法时间复杂度  $O(VE + V^2 \lg V)$






