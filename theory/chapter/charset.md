# 字符集

# ASCII

**ASCII 码**：利用一个字节长度 `0000 0000 ~ 0111 1111` 储存 128 个字符
- 可见字符：数字，字母，共 95 个
- 控制字符：控制打印输出的，例如换行 `\n`，共 33 个

<p style="text-align:center;"><img src="../../image/theory/ascii.png" width="75%" align="middle" /></p>

**扩展 ASCII 码**：后来发现 128 个不够用，又把剩下的 `1000 0000 ~ 1111 1111` 都给使用上 

**码位**：用来编号一个字符的数字

**编码**：码位在计算机中，如何表示。对于 ASCII 码，码位与编码大小一样，取值范围 `0 ~ 255`

# ANSI

## 定义

**ANSI**：由于 ASCII 码只能表示英文，像中文、日文、韩文等，一个字节的 ASCII 码就不够用，这样不同国家和地区就对 ASCII 码的字节数进行拓展，利用多个字节来表示编写适用于自己国家或者地区的语言字符集，例如简中 GB2312，繁中 Big5等。**这些使用多个字节来代表一个字符的延伸编码方式，全部统称为 ANSI 编码**。 <span style="color:blue;font-weight:bold"> ANSI 编码之间互不兼容，各自管各自的。 </span>


windows 操作系统系统会根据地区切换 ANSI 编码，以适应各个地区的语言：在简体中文Windows操作系统中，ANSI 编码代表 GBK (CP 936)编码；在繁体中文Windows操作系统中，ANSI编码代表Big5；在日文Windows操作系统中，ANSI 编码代表 Shift_JIS 编码
- [字符集](https://blog.csdn.net/x356982611/article/details/80285930)

```term
triangle@LEARN:~$ chcp <.CPID> // 切换 powershell 的显示字符集
```

> [!tip]
> `fopen` 识别什么样的字符编码，其实是和操作系统底层实现相关的，简体中文 windows 需要 GB2312 编码，linux 操作系统则需要 UTF-8 编码

## GB2312

**GB2312**：使用分区管理，设计了 94 个区，每个区有 94 个码位，共 8836 个码位
- 01 - 09 区：除汉字以外的 682 个字符
- 10 - 15 区：空白区
- 16 - 55 区：3755 个一级汉字，按拼音排序
- 56 - 87 区：3008 个二级汉字，按照部首/笔画排序
- 88 - 94 区：空白区 

<p style="text-align:center;"><img src="../../image/theory/gb2312_block.png" width="75%" align="middle" /></p>

**码位：** 区号 + 行号 + 列号。例如 16 区的「半」，行号为 7，列号为 5，则码位为 `1675`

**编码：** 将区号与行列号拆分开，分别加上 `0xA0` (高字节、低字节均大于 `127`，用来兼容原版ASCII编码)， 然后再拼接到一起。
<p style="text-align:center;"><img src="../../image/theory/gb2312_encode.png" width="25%" align="middle" /></p>

## GBK

GB2312 表示的字符还不够用，继续扩充
- **GBK**：去掉 GB2312 中低字节大于 `127` 的限制，又扩展了两万多个汉字和符号
- **GB18030**：继续扩充 GBK，新增少数民族的字符

# Unicode

## 定义

**Unicode：** 一套编码准则，目标是囊括世界上所有的字符。ASCII 只能表示英文字符，GBK 拓展了中文字符，但是世界上还有俄文，日文等，大家都定义一套字节的字符编码，用起来就太乱了，Unicode 就想统一所有的字符编码。

Unicode 只是一套准则，还是需要通过具体的字符集进行实现：
- **UCS-2**：码位与编码一样，字节大小为两个字节，可以表示 65536 个字符
- **UCS-4**：码位与编码一样，字节大小为四个字节，可以表示 43 亿个字符
- **UTF-8**：为变长度编码，比上面两个节约空间，被广泛引用

## UTF-8


<div style="margin: 0 auto;padding:1rem;width: 42rem;">

| UCS-4 码位                | UTF-8 编码                          | 字节  |
| ------------------------- | ----------------------------------- | :---: |
| 0x0000 0000 ~ 0x0000 007F | xxxxxxxx                            |   1   |
| 0x0000 0080 ~ 0x0000 07FF | 110xxxxx 10xxxxxx                   |   2   |
| 0x0000 0800 ~ 0x0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx          |   3   |
| 0x0001 0000 ~ 0x0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx |   4   |

</div>

首先获取到字符在 UCS-4 中的码位，然后根据上面的表，将码位二进制填入到对应位置，形成 UTF-8 编码

<p style="text-align:center;"><img src="../../image/theory/utf8_encode.png" width="50%" align="middle" /></p>

## DOM

**DOM**：byte order mark，字节顺序标记。利用多字节来编码一个字符，这就需要字符解码与编码时，明确字节的读写顺序，即区分大端、小端。**DOM 的作用就是标记出字节的大小端顺序**。

- **UCS**：UCS的编码是固定大小的多字符集，且直接二进制编号表示字符，因此就需要区别大小端。UCS 利用字符「零宽无间断间隔 (Zero Width No-Break Space)」来区分大小端：`0xFEFF` 表示大端；`0xFFFE` 表示小端
- **UTF-8 with DOM**：UTF-8 的编码方式不用标记大小端，就能根据其结构正确解析多字节。UTF-8的「零宽无间断间隔 (Zero Width No-Break Space)」字符就被用来标识 UTF-8 这种编码: `0xEFBBBF` 。**只有 windows 才这么玩，linux 不支持**。



# 字符

彻底解密C++宽字符

https://www.jianshu.com/p/9901f2cc303c

