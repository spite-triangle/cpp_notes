# Makefile

# 语法规则

```makefile
# 获取所有的 cpp
SRC=$(wildcard *.c)

# 替换 .cpp 为 .o
OBJ=$(SRC:.c=.o)

# 将 all，clean 定义为伪目标，这样就不会和实际文件名重名
.PHONY: all clean


all: a.out

# a.out 依赖所有 .o 目标，即执行完 .o 目标再执行 a.out
a.out: $(OBJ)
	gcc $^ -o $@

# 批量生成 .o 的目标
%.o: %.c
	gcc  -c $< -o $@

clean:
	rm ./*.d ./*.o a.out
```

# 依赖文件

对于 `makefile` 而言，其只能检测目标依赖的文件是否发生改变。然后再写 `makefile` 的时候，一般只会将 `*.c` 或 `*.cpp` 文件写入依赖，而并不涉及 `*.h` 。这就会导致当头文件发生改变时，进行项目编译，make 并不能识别出头文件变化，导致出现一系列问题。为解决该问题，gcc 编译器可以自动生成源文件的依赖文件，即 `*.d`，用于 `makefile` 能够识别头文件的变化。

```term
triangle@LEARN:~$ gcc -MD -c main.c // MD 告诉编译器生成 .d 文件
triangle@LEARN:~$ ll
-rwxrwxrwx 1 triangle triangle  102 Sep  2 14:45 main.c*
-rwxrwxrwx 1 triangle triangle   48 Sep  2 14:52 main.d*
-rwxrwxrwx 1 triangle triangle 1560 Sep  2 14:52 main.o*
triangle@LEARN:~$ cat main.d
main.o: main.c /usr/include/stdc-predef.h add.h
```

让 `makefile` 自动维护这些东西

```makefile
# 获取所有的 cpp
SRC=$(wildcard *.c)

# 替换 .cpp 为 .o
OBJ=$(SRC:.c=.o)

# .d 文件
DEP=$(SRC:.c=.d)

.PHONY: all clean
all: a.out

# - 当语句执行错误，会忽略继续执行
# 包含所有的 .d 文件
-include $(DEP)

# a.out 依赖所有 .o 目标，即执行完 .o 目标再执行 a.out
a.out: $(OBJ)
	gcc $^ -o $@

# 批量生成 .o 的目标
%.o: %.c
	gcc -MD -c $< -o $@

clean:
	rm ./*.d ./*.o a.out
```

> [!tip]
> ```term
> triangle@LEARN:~$ make -nB target // 查看命令序列的展开
> ```
