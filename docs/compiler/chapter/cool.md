# cool

# 语法

## 基本规则

```cool
class Main{
    i: IO <- new IO;
    main() : Int {  {
        i.out_string("Hello World!\n"); 
        1; 
    }  };

    add(a: Int, b: Int){
        a + b
    };
}; 
```
- 源码文件 `*.cl`
- 所有定义需要以 `;` 结尾，包括类、方法、属性
- 属性
    - 属性定义 `属性名 : 类型 <- 属性值 ;`
    - `Object` 类型是所有类型的基类型
- 函数
    - 函数定义 `函数名 ( 参数名1 : 类型 , 参数名2 : 类型 ) : 返回类型 { { 表达式1; 表达式2; } };`
    - 函数没有显示的返回调用但需要指明返回类型，以代码块 `{}` 最后一句作为返回值
    - 如果只有一个表达式，可以对函数进行简化 `函数名 () : 返回类型 { 表达式1 } };`

## 局部变量

```cool
class Main inherits IO{
    main() : Object {  
        let hello : String <- "hello ",
            World : String <- "World!",
            newline : String <- "\n"
        in 
            out_string( hello.concat(World.concat(newline)) ) 
    };

    number(a:Int, b:Int, c:Int) : Object {
        let hundred : Int <- a * 100,
            ten : Int <- b * 10,
            val : Int <- a + b + c,
            conv : A2I <- new A2I
        in 
            out_string( conv.i2a(val).concat("\n") ) 
    };
}; 
```

- 局部变量表中，底层的变量可以访问之前定义的变量

## 继承

```cool
class Main inherits IO{
    main() : Object {  
        self.out_string("Hello World!\n") 
    };

    toString(obj: Object) : String {
        let str : String <-
            case obj of
                i: Int => (new A2I).i2a(i);
                s: String => s;
                o: Object => { abort(); ""; };
            esac
        in
            str
    }
}; 
```

- 可以使用 `self.` 来调用成员函数或属性，当然也可以不写
- 使用 `abort()` 会终止程序

# 编译执行

```term
triangle@LEARN:~$ coolc hello.cl // 编译结果为 .s 文件
triangle@LEARN:~$ ls
hello.cl hello.s
triangle@LEARN:~$ spim hello.s // 模拟运行 .s 文件
```

# 案例

## 输入输出


```cool
class Main {
    main() : Object {  
        (new IO).out_string( (new IO).in_string().concat("\n") ) 
    };
}; 
```

## 类型转换

```cool
class Main inherits A2I{
    io : IO <- new IO;
    main() : Object {  {
        io.out_string(i2a(a2i(io.in_string())).concat("\n"));
    } };
}; 
```

```term
triangle@LEARN:~$ coolc demo.cl atoi.cl // A2I 由 atoi.cl 实现
```


## 阶乘

```cool
class Main inherits A2I{
    main() : Object { 
        (new IO).out_string(i2a( fact(10) ).concat("\n"))
    };

    fact(i: Int) : Int {
        if (i = 0) then 
            1 
        else 
            i * fact(i - 1) 
        fi
    };

    factLoop(i : Int) : Int {
        let fact : Int <- 1 
        in {
            while (not (i = 0)) loop
                {
                    fact <- fact * 1;
                    i <- i - 1;
                }
            pool;
            fact;
        }
    };
}; 
```
