# 配置

# application 配置

[application](https://docs.spring.io/spring-boot/appendix/application-properties/index.html) 为`Spring Boot`应用程序的默认配置，通常位于 `src/main/resources` 目录下，以下三种配置等效，优先顺序如下：
1. `application.properties` 
2. `application.yml`
3. `application.yaml`

```yaml
server:
  address: localhost
  port: 8080
```

> [!tip]
> `vscode` 中可安装 `Spring Boot Tools` 插件，提供对 Spring Boot 配置文件的语法高亮和智能提示。

`Spring Boot` 也会加载多个位置的配置文件，所有位置的配置都会读取，且优先级如下：
1. `file:./config/` : `jar` 包运行时工作目录下的 `./config`
2. `file:./`: `jar` 包运行时工作目录
3. `classpath:/config/`: 对于 `maven` 项目，`src/main/resources/config/`
4. `classpath:/` : 对于 `maven` 项目，`src/main/resources/`

此外，运行 `jar` 时也能通过 `--spring.config.location=<config_path>` 指定运行时的配置文件


# yaml

- 基本语法
  - 使用缩进表示层级关系，通常使用两个空格进行缩进
  - 使用冒号 `: ` 分隔键值对，冒号后面必须有一个空格
  - 注释以 `#` 开头
  - 多行字符串使用
    -  `|` 表示保留换行，不支持转义
    -  `>` 表示折叠换行，不支持转义，但换行会替换为空格
  - 大小写敏感
- 数据类型
  - 对象
  - 数组，使用 `- ` 表示列表项
  - 字符串
    - 单引号 `'` , 不支持转义
    - 双引号 `"` , 支持转义
  - 数字
  - 布尔值
  - `null`

    ```yaml
    # 对象
    person:
    name: John Doe
    age: 30

    # 数组
    fruits:
    - apple
    - banana
    - orange

    # 字符串
    greeting: "Hello, World!"
    multiline: |
    Line 1\n
    Line 2\t
    multilineFolded: > 
    Line 1\n
    Line 2\t

    # 数字
    pi: 3.14159

    # 布尔值
    enabled: true

    # null
    empty: null
    ```

- 引用变量
    ```yaml
    var: value
    ref: ${var}
    ```

# 配置读取

在 `application` 也可以自定义配置， `Spring Boot` 提供了三种方式：
- 使用 `@Value` 注解读取单个属性值
- 使用 `@ConfigurationProperties` 注解读取一组相关属性值
- 通过 `Environment` 接口读取属性值

1. 在 `application.yaml` 中自定义配置

    ```yaml
    name: "xxx"
    person:
    age: 30
    addresse:
    - "123 Main St"
    - "Apt 4B"
    ```

2. 使用 `@Value` 注解读取单个属性值

    ```java
    @RestController
    public class HelloController {

        /* 将 name 绑定到 configeName */
        @Value("${name}")
        private String configeName;

        @Value("${person.addresse[0]}")
        private String personFirstAddress;

        /* 
            - @Autowired 的作用是将 Spring 容器中的 Bean 赋值给成员变量
            - 使用 @Autowired 都尽量要写构造器，不然有警告
         */
        @Autowired
        private Environment env;
        
        @Autowired
        private Person person;

        public HelloController(Environment env, Person person) {
            this.env = env;
            this.person = person;
        }

        @RequestMapping("/hello")
        public String hello() {

            System.out.println("Config Name: " + configeName);
            System.out.println("Person First Address: " + personFirstAddress);
            System.out.println("Environment Property: " + env.getProperty("name"));
            System.out.println("Person Object: " + person);

            return "Hello, World!";
        }
    }
    ```

3. 使用 `@ConfigurationProperties` 注解读取一组相关属性值

    ```java
    @Component  // 将该类注册为 Spring Bean
    @ConfigurationProperties(prefix = "person")
    public class Person {
        private String name;  // 命名要与配置中一致，与 `prefix` 构成完整属性名 `person.name`
        private Integer age;
        private String[] addresse; // 绑定数组

        public Person() {
        }

        public Person(String name, Integer age, String[] addresse) {
            this.name = name;
            this.age = age;
            this.addresse = addresse;
        }

        @Override
        public String toString() {
            return "Person{" +
                    "name='" + name + '\'' +
                    ", age=" + age +
                    ", addresse=" + Arrays.toString(addresse) +
                    '}';
        }

        public String getName() {
            return name;
        }
        public void setName(String name) {
            this.name = name;
        }
        public Integer getAge() {
            return age;
        }
        public void setAge(Integer age) {
            this.age = age;
        }
        public String[] getAddresse() {
            return addresse;
        }
        public void setAddresse(String[] addresse) {
            this.addresse = addresse;
        }
    }
    ```

4. 对于配置的 `@ConfigurationProperties` 还能使用 `spring-boot-configuration-processor` 依赖为 `application.yaml` 提供智能提示

    ```xml
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-configuration-processor</artifactId>
            <optional>true</optional>
        </dependency>
    ```

# profiles

在 `Spring Boot` 中，可以通过配置文件来区分不同的环境（如开发、测试、生产等），这些配置文件通常以 `.properties` 或 `.yml` 结尾，并且位于 `src/main/resources` 目录下。

- 多文件组织: 配置文件命名 `application-{profile}.yml`
    ```
    .
    ├── application-dev.yaml    # 开发环境配置
    ├── application-prod.yaml   # 生产环境配置
    ├── application-test.yaml   # 测试环境配置
    └── application.yaml        # 主配置
    ```
- `yaml` 使用 `---` 分隔不同配置块

    ```yaml
    # 主配置，激活子配置
    spring:
      profiles:
        active:
        - prod 

    # 其他配置
    ---
    server:
      port: 8080
    
    spring:
      config:
        activate:
          on-profile:
          - dev
    ---
    server:
      port: 8081
    spring:
      config:
        activate:
          on-profile:
          - test
    
    ---
    server:
      port: 8082
    spring:
      config:
        activate:
          on-profile:
          - prod
    ```

配置激活也有多种方案
- 主 `application` 配置中设置 `spring.profiles.active` 
- `VM` 参数 `-Dspring.profiles.active=dev`
- 命令行参数 `--spring.profiles.active=dev`

```
2026-06-21 17:03:40.728 [debug] [NES]  DISABLED
2026-06-21 17:03:40.729 [info] [GHOST] ===== START speculative=false =====
2026-06-21 17:03:40.729 [debug] [GHOST] prefix=4426ch suffix=127ch [0ms]
2026-06-21 17:03:40.729 [debug] [GHOST] prefix_tail="# 配置\r\n\r\n# application 配置\r\n\r\n[application](https://docs.spring.io/spring-…"
2026-06-21 17:03:40.729 [debug] [GHOST] suffix_head="    ├── application-prod.yaml   # 生产环境配置\n    ├── application-test.yaml   # 测试环境…"
2026-06-21 17:03:40.729 [debug] [GHOST] cache_miss [0ms]
2026-06-21 17:03:40.729 [info] [GHOST] async_wait — checking in-flight requests
2026-06-21 17:03:40.729 [info] [GHOST] ASYNC_REUSE result=128ch total=1ms
```