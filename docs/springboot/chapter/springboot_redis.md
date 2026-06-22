# Redis

# 开发环境

1. 使用 `Spring Initializr` 创建项目
2. 添加依赖
  - `spring web`
  - `spring-boot-starter-data-redis`
3. `application`配置

    ```yaml
    spring:
    data:
        redis:
        host: 127.0.0.1  # Redis服务器地址[reference:7][reference:8]
        port: 6379       # Redis服务器端口[reference:9][reference:10]
        database: 0      # Redis数据库索引（默认为0）[reference:11]
        password:        # Redis服务器密码（默认为空）[reference:12]
        timeout: 5000ms  # 连接超时时间[reference:13]
        lettuce:
            pool:
            max-active: 8   # 连接池最大连接数[reference:14]
            max-idle: 8     # 连接池最大空闲连接[reference:15]
            min-idle: 0     # 连接池最小空闲连接[reference:16]
    ```

#  直接使用

```java
@Component
public class TestRedis {

    // 或者使用 RedisTemplate<String, String>
    @Autowired
    private StringRedisTemplate redis;

    public TestRedis(StringRedisTemplate redis) {
        this.redis = redis;
    }

    public void use(){
        redis.boundValueOps("name").get();
        redis.boundValueOps("name").set("xx");

        // 直接根据类型执行操作，更方便
        redis.opsFor*();
        redis.opsForValue().set("user:1", "{\"name\":\"Alice\"}");
        redis.opsForValue().get("user:1");
    }
}
```

# 数据库缓存

1. 配置 `spring-boot-starter-cache` 依赖

    ```xml
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-cache</artifactId>
    </dependency>
    ```

2. 入口类启动 `@EnableCaching`

    ```java
    @SpringBootApplication
    @EnableCaching
    public class MyApplication { ... }
    ```

3. 配置 `application` 启动 `redis` 缓存

    ```yaml
    spring:
      cache:
        type: redis
    ```

4. 使用缓存注解

    ```java
    // @Cacheable : 如果缓存中有数据，此方法将不会被执行
    // 对于 redis 而言，value 与 key 的组合结果结果就是 redis 的 key 
    // 例如传入 id 是 1，则实际 key 为 "users::1"
    @Cacheable(value = "users", key = "#id")
    public User getUserById(Long id) {
        return userRepository.findById(id);
    }
    
    // @CachePut: 方法返回结果会更新缓存
    @CachePut(value = "users", key = "#user.id")
    public User updateUser(User user) {
        userRepository.update(user);
        return user;
    }

    // @CacheEvict: 方法返回后，删除缓存
    @CacheEvict(value = "users", key = "#id")
    public void deleteUserById(Long id) {
        userRepository.deleteById(id);
    }
    ```
