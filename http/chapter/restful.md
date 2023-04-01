# restful 规范

# restful

## 简介

REST (Representational State Transfe) 只是一种基于HTTP协议的接口设计规范，并不是什么具体的程序库、协议等。

## 思想

![alt|c,25](../../image/http/http.png)

- **资源为核心**：HTTP协议的核心内容就是客户端发送请求，服务端给出响应，其内容就是资源，要么获取资源，要么操作资源。
- **state**：资源的状态，在客户端与服务端的请求-响应的过程中，操作的是资源的状态
- **Representational**：对如何操作资源进行描述
- **transfer**：请求-响应的过程中，资源状态的变化

> [!tip]
> 基于 restful 设计的接口，就是利用`url`描述资源，**请求报文**中描述对资源的操作。

# 接口设计

**未采用 Restful 接口设计**：

```
查询：http://127.0.0.1/user/query
插入：http://127.0.0.1/user/insert
更新：http://127.0.0.1/user/update
删除：http://127.0.0.1/user/delete
```
上面是对`user`资源进行操作的四种接口，在 `url` 链接中，直接标注了数据要进行的操作。假如现在需要添加对`group`资源的操作，那么还得定义
```
查询：http://127.0.0.1/group/query
插入：http://127.0.0.1/group/insert
更新：http://127.0.0.1/group/update
删除：http://127.0.0.1/group/delete
```
如果还有其他资源，那么最终的接口数将疯狂增加。


**采用 Restful 接口设计**：

```
资源：http://127.0.0.1/user/
资源：http://127.0.0.1/group/
```

`url` 只对资源进行描述，且具体的操作放到了「请求报文」中

```
// 查询
GET /user HTTP/1.1
Host: localhost

// 插入
POST /user HTTP/1.1
Host: localhost
Content-type: application/x-www-form-urlencoded // 请求主体的类型
Content-length: 10 // 请求主体的长度
// 第一空行
// 第二空行
us=12
```
这样就能大大减少接口数量。