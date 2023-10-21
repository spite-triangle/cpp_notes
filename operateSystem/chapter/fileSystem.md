# 文件系统

# 概念

## 文件

> [!note]
> 磁盘是操作系统最频繁接触的设备，每时每刻都有不同的程序需要读写磁盘的中的文件。这就不能简单的通过驱动来控制进程的并发，实现磁盘的在不同进程中访问。

为了解决该问题，操作系统就为进程提供「虚拟磁盘」，进程对磁盘数据的读写就不再是磁盘驱动，而是抽象出来的「文件系统」中的「文件」
- 提供合理的 API 实现多个应用程序共享磁盘中的数据
- 数据隔离，防止一部分数据异常导致整个磁盘数据异常

在程序的视角，「文件」就是操作系统的虚拟磁盘
- 命名管理：可以通过文件路径查找到对应磁盘数据
- 数据管理：对于文件的读写操作，就是随机读写 `std::vector<char>`

## 文件系统

**文件系统：** 操作系统用于管理「虚拟磁盘」的管理器。每一个文件都在文件系统中进行了的登记，操作系统通过文件系统能对文件进行增删改查。文件系统管理文件采用的是树状结构
- windows: 每一个设备驱动就是一棵目录树
- linux: 只有一个根目录 `/`，其他设备可以被「挂载」到某一个目录上

```term
triangle@LEARN:~$ tree .
.
├── Videos
│   └── bilibili
│       └── load_log
├── Workspace
│   └── driver
│       ├── Makefile
│       └── hello.c
└── test.h
```

## 文件挂载

操作系统在添加新磁盘设备 (例如u盘、移动硬盘) 和加载 `.img、.iso` 镜像文件时
- windows: 单独虚拟出一个盘符来进行管理
- linux: 将设备或者镜像文件挂载到一个文件夹

```term
triangle@LEARN:~$ sudo mount game.img /mnt // 将 game.img 挂载到 /mnt 文件目录
```
对于镜像文件的挂载而言，直观结果就是使用了一个虚拟磁盘去挂载了另外一个虚拟磁盘。
- `game.img` 是一个虚拟磁盘
- `/mnt` 也是一个虚拟磁盘

这种利用一个假的去表示另外一个假的，在逻辑上就有点毛病。linux 的处理方案则是进行了一次套壳，为 `game.img` 创建一个 `loop` 设备，`/mnt` 直接访问的是 `loop` 设备，然后通过 `loop` 把数据读写转发到 `game.img` 

```term
triangle@LEARN:~$ lsblk
NAME MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
sda    8:0    0 363.3M  1 disk
sdb    8:16   0     2G  0 disk [SWAP]
sdc    8:32   0     1T  0 disk /mnt/wslg/distro
# loop 设备
loop0  7:1    0     1M  0 loop /mnt
```

## 链接

```term
triangle@LEARN:~$ ln a.txt b.txt // 硬链接，不能链接目录，不能跨文件系统
triangle@LEARN:~$ ln -s a.txt b.txt // 软链接
triangle@LEARN:~$ ll
-rw-r--r--  2 triangle triangle     0 Oct 14 15:17 a.txt
-rw-r--r--  2 triangle triangle     0 Oct 14 15:17 b.txt
lrwxrwxrwx  1 triangle triangle     5 Oct 14 15:17 c.txt -> a.txt
```

## 文件系统API

### 管理API

- `mkdir` : 创建目录
- `rmdir` : 删除目录，且是空文件夹
- `unlink` : 删除链接
- `getdents/readdir` : 读取文件夹信息
- `link` : 链接
- `chdir`: 修改进程工作目录

### 文件API

- `fd`: 文件描述符，指向一个虚拟磁盘
- `open`
- `mmap`
- `dup`
- `read`
- `write`

# 文件系统的实现

## 基础接口

真正的磁盘是块设备，那么最基本的磁盘驱动接口就只有了
- 读： `bread(int id, char * buf)`
- 写： `bwrite(int id, const char * buf)`


> [!note]
> 这就导致 cpu 访问磁盘不能对字节进行随机访问，只能一块一块的获取数据

所有的文件系统 API 都是基于这个两个接口实现，对这两个接口进行进一步的封装

![alt|c,40](../../image/operationSystem/fileSystemStructure.jpg)

## FAT

### 块管理

**方案一**: 将同一文件相关的块串联成一个链表。一个块分为数据和指针两个区域，指针会指向下一个块用来链接链表，也当作用于区分当前块是否空闲的标记位
- **不能以块为单位随机访问**

![block link|c,70](../../image/operationSystem/blockLink.jpg)

**方案二**：将块划分为两种类型，一种用于存储文件相关的块指针，一种用于存储数据。
- 数据块指针存放集中，数据指针块完蛋，文件就完蛋 

![block classification](../../image/operationSystem/blockClassification.jpg)

### FAT 实现

FAT 文件系统采用了第二种设计方案，且为了防止存放指针的数据块损坏，会对这些数据块进行备份

- [RTFM](https://jyywiki.cn/pages/OS/manuals/MSFAT-spec.pdf)

![fat|c,50](../../image/operationSystem/fat.jpg)

FAT-12/16/32 : 数字指的是指针的长度，而 `FAT (File Allocation Table)` 指的就是存放指针的块

```cpp
if (CountofClusters < 4085) {
  // Volume is FAT12 (2 MiB for 512B cluster)
} else if (CountofCluster < 65525) {
  // Volume is FAT16 (32 MiB for 512B cluster)
} else {
  // Volume is FAT32
}
```
通过数据块指针 (FAT Entry) 的值来标记文件的开始、结束、数据块是否损坏等

![fat Entry](../../image/operationSystem/fatEntry.jpg)

**「目录」在文件系统则是一个特殊的文件**。在 FAT 中，目录文件存放的是一组 Directory Entry (描述了文件名，文件权限等)

![directory entry](../../image/operationSystem/directoryEntry.jpg)



### 实操

```term
triangle@LEARN:~$ yes | head -c 104857600 > fs.img // 先创建 100m 文件
triangle@LEARN:~$ mkfs.fat -f 4 -s 8 -S 512 -v fs.img // 格式化文件为 FAT 文件系统

# -f 4:  FAT 表有四份
# -S 512: 一个 sector (扇区) 的长度
# -s 8:  一个 cluster (簇) 中有 8 个 sector

mkfs.fat 4.1 (2017-01-24)
fs.img has 64 heads and 32 sectors per track,
hidden sectors 0x0000;
logical sector size is 512, 
using 0xf8 media descriptor, with 204800 sectors;
drive number 0x80;
filesystem has 4 16-bit FATs and 8 sectors per cluster.
FAT size is 104 sectors, and provides 25543 clusters.
There are 8 reserved sectors.
Root directory contains 512 slots and uses 32 sectors.
Volume ID is 7a0e5078, no volume label.
triangle@LEARN:~$ file fs.img
fs.img: DOS/MBR boot sector, code offset 0x3c+2, OEM-ID "mkfs.fat", sectors/cluster 8, reserved sectors 8, FATs 4, root entries 512, Media descriptor 0xf8, sectors/FAT 104, sectors/track 32, heads 64, sectors 204800 (volumes > 32 MB), serial number 0x7a0e5078, unlabeled, FAT (16 bit)
triangle@LEARN:~$ sudo mount fs.img /mnt/img // 将虚拟文件系统挂载到操作系统
triangle@LEARN:~$ sudo touch /mnt/img/test.txt // 往文件系统添加内容
```

## ext2/UNIX文件系统

FAT 特性
- 文件元数据都存储在目录文件中，无法实现链接
- 小文件不错，但大文件的数据块随机访问不行，因为文件的存储结构本质是一个链表

针对上述两个主要问题，ext2/UNIX文件系统进行了改进: 单独划分一个磁盘区域，用于存储所有文件的元数据，即 `inode`，建立数据结构管理 `inode`。并且根据 `inode` 重新设计了一套管理数据块的数据结构

![inode|c,60](../../image/operationSystem/inode.jpg)

- 小文件直接在 `inode` 中存放数据块索引，方便快速查询
- 大文件则创建一级、二级、三级索引

目录文件则存储 `inode` 的索引

![ext2 folder](../../image/operationSystem/ext2Folder.jpg)






