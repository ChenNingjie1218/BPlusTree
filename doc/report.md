# B+ Tree 任务报告

## 1 项目背景
B+Tree 原理、意义、应用场景等。
### 1.1 B+Tree原理
1. 完美平衡（每个叶节点都在同一层）
2. 除根节点外，其他节点都包含`n`个key，其中`⌈m/2⌉-1⩽n⩽m-1`
3. 每个有`n`个key的内部节点,都会有`n+1`个非空孩子
4. **每个内部节点仅保存关键字来索引**
5. **所有键值对内容都在叶子节点上** 

### 1.2 B+Tree的引入
如果设计一个查询系统，考虑选择怎样的数据结构
- 数组
只能顺序查询，时间复杂度为`O(n)`
- 哈希表
用哈希表(链地址法)存储**需要解决冲突**，且**数据分布不均匀**，最差的情况又退回成数组形式
- 二叉树
为加快查询速度，用二叉树存储，但如果插入顺序是有序的，会形成**单链**，从而使查询又退回数组形式
- 平衡二叉树
解决了二叉树的问题，但以牺牲建树的时间为代价。但对于插入操作过多的情况，将**消耗大量时间调整树的平衡**。
- 红黑树
最长子树不超过最短子树的**2倍**即可。减少了调整树的次数，但**数据量特别大**时，树太**高**
- B树
为解决树太高的问题，使一个节点存储多个数据。节点中保存数据量n，数据为键值对key-value,内部节点保存孩子指针p~n~。假设一个磁盘块16k，一个数据的value占1k，忽略其他成员，则一个节点只能放16个数据，3层B树只能存4096(16^3^)个数据。因此**无法存储大量数据**，如果**增加层高**，又回到了**树太高**的问题。
- B+树
每个内部节点仅保存关键字的索引及孩子指针，将数据键值对都存放在叶子节点中。

### 1.3 B+Tree的意义
1. 查询速度快：查找任何关键字都是`O(logn)`
2. 树高低：每个节点存储大量数据

### 1.4 B+Tree的应用场景
MySQL

## 2 详细设计
### 2.1 总体框架
![Alt text](res/B+%E6%A0%91%E7%B1%BB%E5%9B%BE.png)

### 2.2 关键流程

介绍插入、删除、查找、序列化、反序列化、并发控制等关键流程。
使用流程图、数据布局图等辅助说明。

#### 2.2.1 插入
![Alt text](res/%E6%8F%92%E5%85%A5.png)
#### 2.2.2 删除
![Alt text](res/%E5%88%A0%E9%99%A4.png)
#### 2.2.3 查找
![Alt text](res/%E6%9F%A5%E6%89%BE.png)
#### 2.2.4 范围查找
![Alt text](res/%E8%8C%83%E5%9B%B4%E6%9F%A5%E6%89%BE.png)
## 3 代码实现

### 3.1 目录组织

介绍一下各目录的作用。
.
├── build
│ 
├── doc
│   └── res
├── include
├── src
└── test

### 3.2 接口

公共接口。
```
template<typename T>
void interface();


```
### 3.3 关键代码

重要或有意思的代码片段。

### 3.4 程序分析

内存泄漏分析，gprof/perf 代码性能分析。

## 4 测试

### 4.1 单元测试

#### 4.1.1 插入测试
##### 4.1.1.1 空树插入
测试用例:
```
create 3
bfs
insert 1 1
bfs

```
输出结果:
```
---------------创建一课空的B+树--------------
 [ ]
---------------向B+树中插入<1, 1>--------------
 [ <1, 1> ]
```
##### 4.1.1.2 插入非空树且不导致叶子节点分裂
测试用例:
```
create 3
insert 1 1
insert 2 2
insert 3 3
bfs
insert 0 0
bfs

```

输出结果:
```
 [2]
 [ <1, 1> ] [ <2, 2> <3, 3> ]
 ---------------向B+树中插入<0, 0>--------------
 [2]
 [ <0, 0> <1, 1> ] [ <2, 2> <3, 3> ]
```
##### 4.1.1.3 插入非空树且导致叶子节点分裂
测试用例：
```
create 3
insert 1 1
insert 2 2
insert 3 3
insert 0 0
bfs
insert 4 4
bfs

```

输出结果：
```
 [2]
 [ <0, 0> <1, 1> ] [ <2, 2> <3, 3> ]
---------------向B+树中插入<4, 4>--------------
 [2 3]
 [ <0, 0> <1, 1> ] [ <2, 2> ] [ <3, 3> <4, 4> ]
```
##### 4.1.1.4 插入非空树且导致内部节点分裂
测试用例:
```
create 3
insert 1 1
insert 2 2
insert 3 3
insert 0 0
insert 4 4
insert 5 5
insert 6 6
bfs
insert 7 7
bfs

```
输出结果:
```
 [3]
 [2] [4 5]
 [ <0, 0> <1, 1> ] [ <2, 2> ] [ <3, 3> ] [ <4, 4> ] [ <5, 5> <6, 6> ]
---------------向B+树中插入<7, 7>--------------
 [3 5]
 [2] [4] [6]
 [ <0, 0> <1, 1> ] [ <2, 2> ] [ <3, 3> ] [ <4, 4> ] [ <5, 5> ] [ <6, 6> <7, 7> ]
```
##### 4.1.1.5 插入非空树且导致顶部节点分裂
测试用例:
```
create 3
insert 1 1
insert 2 2
insert 3 3
insert 4 4
bfs
insert 5 5
bfs

```
输出结果：
```
 [2 3]
 [ <1, 1> ] [ <2, 2> ] [ <3, 3> <4, 4> ]
---------------向B+树中插入<5, 5>--------------
-------------------顶层节点满了---------------
 [3]
 [2] [4]
 [ <1, 1> ] [ <2, 2> ] [ <3, 3> ] [ <4, 4> <5, 5> ]
```
#### 4.1.2 查找测试
测试用例:
```
create 3
insert 2 2
insert 4 4
insert 3 3
insert 1 1
insert 5 5
insert 6 6
bfs

```
输出结果：
```
 [4]
 [3] [5]
 [ <1, 1> <2, 2> ] [ <3, 3> ] [ <4, 4> ] [ <5, 5> <6, 6> ]
```
##### 4.1.2.1 查找一个存在的关键字
测试用例:
```
search 2
search 3
search 6
```
输出结果:
```
关键字2对应的值为2
关键字3对应的值为3
关键字6对应的值为6
```
##### 4.1.2.2 查找一个不存在的关键字
测试用例:
```
search 0
search 9
```
输出结果:
```
没有该关键字
没有该关键字
```
##### 4.1.2.3 查找一个存在的关键字，且该关键字在路径上
测试用例:
```
search 4
search 5
```
输出结果:
```
关键字4对应的值为4
关键字5对应的值为5
```
#### 4.1.3 范围查找测试
测试用例:
```
create 5
insert 10 10
insert 20 20
insert 30 30
insert 40 40
insert 50 50
insert 60 60
insert 70 70
bfs

```
输出结果:
```
 [30 50]
 [ <10, 10> <20, 20> ] [ <30, 30> <40, 40> ] [ <50, 50> <60, 60> <70, 70> ]
```
##### 4.1.3.1 该范围在关键字域内部且在一个叶子节点上
测试用例:
```
rangesearch 45 65
rangesearch 100 55
rangesearch 25 35
```
输出结果:
```
 <50, 50> <60, 60>
 <60, 60> <70, 70>
 <30, 30>
```
##### 4.1.3.2 该范围在关键字域内部且跨越多个叶子节点
测试用例:
```
rangesearch 15 55
rangesearch 15 35
```
输出结果:
```
  <20, 20> <30, 30> <40, 40> <50, 50>
  <20, 20> <30, 30>
```
##### 4.1.3.3 左边界小于最小关键字且右边界在第一个叶子节点内
测试用例:
```
rangesearch 5 15 
rangesearch 5 25
```
输出结果:
```
 <10, 10>
 <10, 10> <20, 20>
```
##### 4.1.3.4 左边界小于最小关键字且右边界跨越多个叶子节点
测试用例:
```
rangesearch 0 35
rangesearch 0 55
rangesearch 0 65
```
输出结果:
```
 <10, 10> <20, 20> <30, 30>
 <10, 10> <20, 20> <30, 30> <40, 40> <50, 50>
 <10, 10> <20, 20> <30, 30> <40, 40> <50, 50> <60, 60>
```
##### 4.1.3.5 该范围涵盖所有关键字
测试用例:
```
rangesearch 0 100
```
输出结果:
```
 <10, 10> <20, 20> <30, 30> <40, 40> <50, 50> <60, 60> <70, 70>
```
##### 4.1.3.6 右边界大于最大关键字且左边界跨越多个叶子节点
测试用例:
```
rangesearch 35 100
rangesearch 15 100
```
输出结果:
```
 <40, 40> <50, 50> <60, 60> <70, 70>
 <20, 20> <30, 30> <40, 40> <50, 50> <60, 60> <70, 70>
```
##### 4.1.3.7 右边界大于最大关键字且左边界在最后一个叶子节点内
测试用例:
```
rangesearch 50 100
rangesearch 65 100
rangesearch 70 100
```
输出结果:
```
 <50, 50> <60, 60> <70, 70>
 <70, 70>
 <70, 70>
```
##### 4.1.3.8 右边界小于最小关键字
测试用例:
```
rangesearch 0 5
```
输出结果:
```
没有该范围的关键字
```
##### 4.1.3.9 左边界大于最大关键字
测试用例:
```
rangesearch 100 200
```
输出结果:
```
没有该范围的关键字
```
#### 4.1.4 删除测试
##### 4.1.4.1 删除不在路径上的关键字
测试用例:
```
create 3
insert 5 5
insert 3 3
insert 7 7
insert 1 1
insert 9 9
insert 11 11
bfs
delete 1
bfs
delete 11
bfs
```

输出结果:
```
 [7]
 [5] [9]
 [ <1, 1> <3, 3> ] [ <5, 5> ] [ <7, 7> ] [ <9, 9> <11, 11> ]

 [7]
 [5] [9]
 [ <3, 3> ] [ <5, 5> ] [ <7, 7> ] [ <9, 9> <11, 11> ]

 [7]
 [5] [9]
 [ <3, 3> ] [ <5, 5> ] [ <7, 7> ] [ <9, 9> ]
```
##### 4.1.4.2 删除在路径上的关键字
测试用例:
```
create 3
insert 10 10
insert 20 20
insert 30 30
insert 40 40
insert 50 50
insert 35 35
bfs
delete 30
bfs

```
输出结果:
```
 [30]
 [20] [40]
 [ <10, 10> ] [ <20, 20> ] [ <30, 30> <35, 35> ] [ <40, 40> <50, 50> ]

----------------已删除<30, 30>-------------

 [35]
 [20] [40]
 [ <10, 10> ] [ <20, 20> ] [ <35, 35> ] [ <40, 40> <50, 50> ]
```
##### 4.1.4.3 删除在最后一段路径上的关键字
测试用例:
```
create 3
insert 5 5
insert 3 3
insert 7 7
insert 1 1
insert 9 9
insert 11 11
insert 15 15
insert 20 20
bfs
delete 15
bfs

```
输出结果:
```
 [7 11]
 [5] [9] [15]
 [ <1, 1> <3, 3> ] [ <5, 5> ] [ <7, 7> ] [ <9, 9> ] [ <11, 11> ] [ <15, 15> <20, 20> ]

----------------已删除<15, 15>-------------

 [7 11]
 [5] [9] [20]
 [ <1, 1> <3, 3> ] [ <5, 5> ] [ <7, 7> ] [ <9, 9> ] [ <11, 11> ] [ <20, 20> ]
```
##### 4.1.4.4 删除导致叶子节点合并
测试用例:
```
create 3
insert 3 3
insert 2 2
insert 4 4
insert 1 1
insert 0 0
insert 5 5
insert 6 6
bfs
delete 3
bfs

```
输出结果:
```
 [3]
 [1] [4 5]
 [ <0, 0> ] [ <1, 1> <2, 2> ] [ <3, 3> ] [ <4, 4> ] [ <5, 5> <6, 6> ]

----------------已删除<3, 3>-------------
-------------------跟右兄弟合并----------------------

 [4]
 [1] [5]
 [ <0, 0> ] [ <1, 1> <2, 2> ] [ <4, 4> ] [ <5, 5> <6, 6> ]

```
##### 4.1.4.5 删除导致内部节点合并
测试用例:
```
create 3
insert 10 10
insert 20 20
insert 30 30
insert 40 40
insert 50 50
delete 50
bfs
delete 40
bfs
```
输出结果:
```
 [30]
 [20] [40]
 [ <10, 10> ] [ <20, 20> ] [ <30, 30> ] [ <40, 40> ]

----------------已删除<40, 40>-------------
-------------------跟左兄弟合并----------------------
-------------------跟左兄弟合并----------------------

 [20 30]
 [ <10, 10> ] [ <20, 20> ] [ <30, 30> ]
```
##### 4.1.4.6 删除导致顶层节点为空
测试用例:
```
create 3
insert 10 10
insert 20 20
insert 30 30
insert 40 40
insert 50 50
bfs
delete 10
bfs
```
输出结果:
```
 [30]
 [20] [40]
 [ <10, 10> ] [ <20, 20> ] [ <30, 30> ] [ <40, 40> <50, 50> ]

----------------已删除<10, 10>-------------
-------------------跟右兄弟合并----------------------
-------------------跟右兄弟合并----------------------
s
 [30 40]
 [ <20, 20> ] [ <30, 30> ] [ <40, 40> <50, 50> ]
```
##### 4.1.4.7 删除后需要向右兄弟借
测试用例:
```
create 3
insert 10 10
insert 20 20
insert 30 30
bfs
delete 10
bfs
```
输出结果:
```
 [20]
 [ <10, 10> ] [ <20, 20> <30, 30> ]

----------------已删除<10, 10>-------------
-------------------叶子节点找右兄弟借20----------------------

 [30]
 [ <20, 20> ] [ <30, 30> ]
```
##### 4.1.4.8 删除后需要向左兄弟借
测试用例:
```
create 3
insert 10 10
insert 20 20
insert 30 30
insert 0 0
delete 20
bfs
delete 30
bfs

```
输出结果:
```
 [30]
 [ <0, 0> <10, 10> ] [ <30, 30> ]

----------------已删除<30, 30>-------------
-------------------叶子节点找左兄弟借10----------------------

 [10]
 [ <0, 0> ] [ <10, 10> ]
```
### 4.2 性能测试

## 5 总结

总结困难问题的解决思路，以及收获与心得体会。
- 模板的头文件包括声明及定义导致只有`.h`文件，但是`Main.cpp`里面找不到头文件
解决方案1：不知道为啥`${}`括起来的几种形式，`include_directories`识别不到正确的位置，改成**相对路径**得以解决

- 设计时，想把`key`设置成一个`pair`，简化插入删除等操作。叶子节点存**键值对**，内部节点存**键和左边孩子的指针**。该`key`成员第二个类型模版参数列表只能放在类外，导致叶子节点跟内部节点并**没有继承同一个基类**。(实例化的不一样，`BNode<T, BNode<T>*>` 和 `BNode<T, uint64_t*>`)最后放弃了该设计。

- 查找成功标志的问题
想要查找成功返回一个`pair`键值对，查找失败返回`NULL`。但是`NULL`并不能作为`pair`的返回值，而`value`部分并不能作为**标志**
解决方案：节点中不存**值的内容**，改为存指向内容的**指针**，返回的`pair`中`value`部分为空标识查找失败

- 分裂的优化
最初方案：利用拷贝构造函数直接拷贝一整个节点，然后去首或去尾
优化方案1：编写特殊的构造函数，只拷贝需要的部分（可不可以移动过去？）

- 合并的优化
最初方案：将另一节点数据拷贝过去
优化方案1：将另一节点数据移动过去

- 范围查找的优化
最初方案：遍历完该节点，遍历右兄弟的时候，走的同样的逻辑，导致又二分查找了一次
优化方案1：横向遍历的时候不再二分查找，传入默认参数`continueFlag`

- OPP优化
最初方案：建树、插入、查找、范围查找、BFS等大操作都是面向过程，且`MAX_SIZE`没有封装
优化方案1：将上述操作放入一个类中实现面向对象

- `MAX_SIZE`访问的问题
封装后可以实现一个B+树对象，一个度。但是，节点类如何访问到该成员？（在节点类内丢失了树类对象，无法使用`get`方法获取）
解决方案1：在节点类需要用到的地方，都传入该参数，但会使节点类的各个成员函数参数列表显得很复杂。(采用)
解决方案2：为节点类添加成员，虽然可以解决问题，但是浪费内存。