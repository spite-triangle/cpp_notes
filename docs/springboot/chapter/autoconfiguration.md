# 自动配置

# 作用

**自动配置 `AutoConfiguration`** : 根据项目依赖的 `Jar` 包、环境变量和配置文件，在 `Spring` 容器启动时，自动创建好所需的 `Bean`，并赋予合理的默认值，让我们开箱即用
- `@Component/@Configuration`: 创建 `Bean`
- `@Condition` ： 控制 `Bean` 是否加载
- `@SpringBootApplication`: `spring` 的启动注解，用于注解启动类
- `@ComponentScan`：指定扫描包
- `@Import`：导入其他配置类
- `@EnableAutoConfiguration `: **自动配置**，自动加载依赖包的 `Bean`

# Component/Configuration

创建 `Bean` 的基础注解
- `@Component` ：标记一个类，将其变为一个 `Bean`

    ```java
    @Component
    @ConfigurationProperties(prefix = "person")
    public class Person {/* .. */}
    ```

- `@Configuration`: 让类内部的 `@Bean` 注解的方法返回对象变成 `Bean`，即创建 `Bean` 的工厂，且触发 `CGLIB` 代理，保证 `Bean` 的单例

    ```java
    /* 
        1. 标识配置类：内部的 @Bean 注解的方法会被 Spring 容器调用，并将返回值作为 Bean 对象管理。
        2. 保证 Bean 的单例语义：保证 @Bean 方法返回的对象在 Spring 容器中是单例
        3. 与 @Component 区别：@Component 只能将类标记为 Bean
    */
    @Configuration 
    public class StudentConfig {

        @Bean  // 将方法返回值注册为 Spring Bean，**bean 名称默认与方法名一致**
        @Conditional(StudentCondition.class) // 条件注解，只有当 StudentCondition 返回 true 时才创建 Bean
        public Student student() {
            return new Student();
        }

        @Bean
        public Student studentMonitor() {
            // 保证 Bean 的单例语义
            // 复用 student Bean 实例，即 studentMonitor 和 student 是同一个对象
            return student(); 
        }
    }
    ```

根据三层架构，对 `@Component` 注解进一步封装
- **表现层**：`@Controller` ，需配合 `@RequestMapping` 使用，创建 `url` 对应的处理器
- **业务层**：`@Service`，没有附加的技术特性，仅仅用于语义标注
- **数据访问层**: `@Repository`，标记该类作为数据仓库，`Spring` 会自动为该类启用持久化异常转换
  -  `Mybatis` 配合 `@Mapper` 使用

# Condition

## 原理

在 `spring 4.0` 新增加的功能，可用于加载 `Bean` 前，检测 `Bean` 依赖的是否存在，不存在则不加载。

1. 创建 `Configuration` 类

    ```java
    /* 
        1. 标识配置类：内部的 @Bean 注解的方法会被 Spring 容器调用，并将返回值作为 Bean 对象管理。
        2. 保证 Bean 的单例语义：保证 @Bean 方法返回的对象在 Spring 容器中是单例
        3. 与 @Component 区别：@Component 只能将类标记为 Bean
    */
    @Configuration 
    public class StudentConfig {

        @Bean  // 将方法返回值注册为 Spring Bean，**bean 名称默认与方法名一致**
        @Conditional(StudentCondition.class) // 条件注解，只有当 StudentCondition 返回 true 时才创建 Bean
        public Student student() {
            return new Student();
        }

        @Bean
        public Student studentMonitor() {
            // 保证 Bean 的单例语义
            // 复用 student Bean 实例，即 studentMonitor 和 student 是同一个对象
            return student(); 
        }
    }
    ```

2. 创建条件类

    ```java
    public class StudentCondition implements Condition {
        /* 
            context：提供了一些上下文信息，如 Environment、BeanFactory、ClassPathResource 等
            metadata：提供了被注解的类的信息，如方法参数等
         */
        @Override
        public boolean matches(ConditionContext context, AnnotatedTypeMetadata metadata) {
            // 获取环境信息
            Environment environment = context.getEnvironment();
            // 获取系统属性或环境变量，例如检查是否包含 "student" 属性
            String studentProperty = environment.getProperty("student.enabled");
            // 如果属性存在且为 true，则条件满足
            return "true".equals(studentProperty);
        }
    }
 
    ```

3. 使用测试

    ```java
    @SpringBootApplication
    public class DemoApplication {

        public static void main(String[] args) {
            // 获取 spring IOC 容器
            ConfigurableApplicationContext ctx = SpringApplication.run(DemoApplication.class, args);

            // 获取 bean
            Object obj = ctx.getBean("student");
            System.out.println(obj);
        }
    }
    ```

> [!tip]
> `Spring Boot` 基于 `@Conditional` 二次封装，提供了许多可直接使用的 `@ConditionalOn*` 注解

## web 切换

在 `Spring Boot` 中，通过 `pom.xml` 配置不同的 `web` 服务包来切换不同的 `Web` 框架
- `spring-boot-starter-web`: 默认使用 `Tomcat`
- `spring-boot-starter-tomcat`
- `spring-boot-starter-jetty`
- `spring-boot-starter-undertow`

1. 在 `pom.xml` 中首先排除 `spring-boot-starter-web` 依赖中的 `spring-boot-starter-tomcat`
2. 添加相应的 `Web` 服务器依赖

```xml
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-webmvc</artifactId>
        <!-- 排除默认的 tomcat -->
        <exclusions>
            <exclusion>
                <groupId>org.springframework.boot</groupId>
                <artifactId>spring-boot-starter-tomcat</artifactId>
            </exclusion>
        </exclusions>
    </dependency>

    <!-- 添加 jetty 依赖 -->  
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-jetty</artifactId>
    </dependency>
```

**`web` 切换的本质，就是通过 `Conditon` 检测实际依赖的是哪个 `web` 服务器，根据 `web` 服务器类型，启动对应的 `Bean`。**

# import

默认情况下启动类 (`main`定义类) 的 `@SpringBootApplication` 注解只会加载**启动类所在包及其子包中的`Bean`。**

```java
@Target({ElementType.TYPE})            // 目标元素类型：类、接口（包括注解类型）、枚举声明
@Retention(RetentionPolicy.RUNTIME)    // 元数据保留策略：运行时可见
@Documented                            // 是否生成文档：是
@Inherited                             // 是否可以被子类继承：是
@SpringBootConfiguration               // @Configuration 注解封装
@EnableAutoConfiguration               // NOTE - 自动配置
@ComponentScan(                        // NOTE - 扫描注解，只扫面启动类(`main`定义类)所在包及其子包中的 `Bean`
   excludeFilters = {@Filter(
   type = FilterType.CUSTOM,
   classes = {TypeExcludeFilter.class}
), @Filter(
   type = FilterType.CUSTOM,
   classes = {AutoConfigurationExcludeFilter.class}
)}
)
public @interface SpringBootApplication {/* ..... */}
```

要想加载除启动类所在包之外的其他包中的 `Bean`，有以下几种方式：
- `@ComponentScan`：指定扫描包
- `@Import`：导入其他配置类

```java
@SpringBootApplication
@ComponentScan("com.example.other")  // 扫描包
@Import(OtherConfig.class) // 导入类
public class DemoApplication {
    public static void main(String[] args) {
        SpringApplication.run(DemoApplication.class, args);
    }
}
```

`@Import` 支持的导入方式
- `Bean` : 导入 `Bean` 类，类标记 `@Component` 可省略
- `Configuration` 类：导入 `Configuration` 类，类标记 `@Configuration` 可省略
- `ImportSelector` 接口：实现该接口的类需要重写 `selectImports()` 方法，返回一个字符串数组，表示要导入的配置类全限定名
- `ImportBeanDefinitionRegistrar` 接口：实现该接口的类需要重写 `registerBeanDefinitions()` 方法，在其中手动注册 `Bean` 定义

# EnableAutoConfiguration

`@Enable*` 注解就是对 `@Import` 进一步封装，快速导入一系列特定的`Bean`。

```java
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@AutoConfigurationPackage
@Import({AutoConfigurationImportSelector.class})  // 封装 Import
public @interface EnableAutoConfiguration{/* ... */}
```

**`Spring Boot` 启动时，通过 `EnableAutoConfiguration` 中的 `@AutoConfigurationPackage` 来导入其他依赖包的 `Bean`。** 具体 `Bean` 则由 `META-INF/spring.factories` 配置，而 `Bean` 是否初始化，则通过`Condition` 控制。**因此，要想 `@AutoConfigurationPackage` 加载自动加载包依赖的 `Bean`，就是提供`META-INF/spring.factories` 配置**

> [!note]
> `EnableAutoConfiguration` 干的事情，本质上就是自动加载依赖包暴露的 `Bean`。

