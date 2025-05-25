# 配置

# 配置文件

```term
triangle@LEARN:~$ vim /etc/nginx/nginx.conf    
```

![alt|c,50](../../image/nginx/config.png)

配置文件结构
- **全局块(main)**：设置 Nginx 服务的整体运行配置指令
- **Event 块** ： 用于配置 Nginx 服务与用户之间连接的属性
- **http 块** : 用于配置 Nginx 的业务功能
  - 全局配置
  - Server 块 ： 配置一个 Nginx 服务
  - location : 路由配置，将 Nginx 反向代理服务器接收的请求转发到目标服务
  
```conf
# ===================== 全局块 ==========================

worker_processes  1; # 工作进程的数量

# ===================== Event 块 ==========================

events {
    worker_connections  1024; # 每个工作进程连接数
}

# ===================== http 块 ==========================

http {
    include       mime.types;
    default_type  application/octet-stream;

    # 日志格式
    log_format  access  '$remote_addr - $remote_user [$time_local] $host "$request" '
                  '$status $body_bytes_sent "$http_referer" '
                  '"$http_user_agent" "$http_x_forwarded_for" "$clientip"';
    access_log  /srv/log/nginx/access.log  access; # 日志输出目录
    gzip  on;
    sendfile  on;

    # 链接超时时间，自动断开
    keepalive_timeout  60;

    # 虚拟主机
    server {
        listen       8080;
        server_name  localhost; # 浏览器访问域名

        charset utf-8;
        access_log  logs/localhost.access.log  access;

        # 路由
        location / {
            root   www; # 访问根目录
            index  index.html index.htm; # 入口文件
        }
    }

    # 引入其他的配置文件
    include servers/*;
    include /etc/nginx/conf.d/*.conf;
}
```
http://127.0.0.1:8080/

# 反向代理

![alt|c,50](../../image/nginx/example_reverse.png)

```conf
worker_processes  auto;
error_log  /var/log/nginx/error.log notice;
pid        /run/nginx.pid;

events {
    worker_connections  1024;
}

http {
    include       /etc/nginx/mime.types;
    default_type  application/octet-stream;
    log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                      '$status $body_bytes_sent "$http_referer" '
                      '"$http_user_agent" "$http_x_forwarded_for"';
    access_log  /var/log/nginx/access.log  main;
    sendfile        on;
    keepalive_timeout  65;

    server{
        ; 代理服务
        server_name 172.29.224.1;
        listen 8080;

        ; 代理服务 '/' 路由跳转到的链接
        location / {
                proxy_pass https://www.bilibili.com/;
        }
    }
}
```
 
在浏览器中输入 `http://172.29.224.1:8080/` 便会通过 `nginx` 跳转到 `https://www.bilibili.com/`
