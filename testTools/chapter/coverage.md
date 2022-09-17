# 覆盖率测试

# 基本概念

- **覆盖率**：程序执行过程中，被执行的源代码占全部源代码的比例。**与`gTest`测试案例进行配合，就能知道自己有多少代码被检测。**
- **作用：** 衡量有多少代码参与了案列测试，如果覆盖率太低，就表明运行状态不明的代码太多，存在的风险也就越多。
- **工具**：
  - `gcov`：gcc自带的一个可以生成代码覆盖率数据的工具
  - `cov`：对 gcov 的结果进行图像化显示

# 测试流程

- [关于代码覆盖lcov的使用](https://www.jianshu.com/p/a42bbd9de1b7)

1. 在`gcc`编译指令中添加 `-fprofile-arcs -ftest-coverage -lgcov`
2. 编译程序得到 `.gcno` 文件
3. 运行程序得到 `.gcda` 文件 
4. 利用 `gcov` 工具得到源码对应的覆盖率结果 `.gcov` 文件
    ```term
    triangle@LEARN:~$ gcov source.cpp
    ```
5. 生成`.info` 文件
    ```term
    triangle@LEARN:~$ lcov -c -i -d [.gcda 所在文件夹] -o part1.info //  -c 捕获，-i初始化，-d应用目录，-o输出文件
    triangle@LEARN:~$ lcov -a part1.info -a part2.info -o all.info // -a 合并
    ```
6. 生成 html
    ```term
    triangle@LEARN:~$ genhtml -o [输出文件夹名] all.info
    ```
