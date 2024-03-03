# WSL

# 网络代理

1. `powershell` 管理员模式下执行下列命令，使得 `WSL2` 能通过防火墙

```term
triangle@LEARN:~$ New-NetFirewallRule -DisplayName "WSL" -Direction Inbound  -InterfaceAlias "vEthernet (WSL)"  -Action Allow
```

2. 代理工具开启「局域网代理」
3. 设置代理

```term
triangle@LEARN:~$ ipconfig.exe
    ...
以太网适配器 vEthernet (WSL):

   连接特定的 DNS 后缀 . . . . . . . :
   本地链接 IPv6 地址. . . . . . . . : fe80::b578:9760:b8f3:fee3%55
   IPv4 地址 . . . . . . . . . . . . : 172.18.160.1
   子网掩码  . . . . . . . . . . . . : 255.255.240.0
   默认网关. . . . . . . . . . . . . :
   ...
triangle@LEARN:~$ export all_proxy=htttp:/172.18.160.1:port
```


