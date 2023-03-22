# 操作系统


# 系统加载

## CPU 初始化

**CPU Reset** : CPU 存在一个初始状态，即寄存器存在一个初始值
- PC : 0xfff0，加载第一条指令的入口地址，开始执行的这个程序是 ROM 里面的程序
- EFLAGS : 0x0002，中断关闭 


## Firmware

**固件 (Firmware)** ：ROM 里面的程序，由硬件厂商提供，用于支持启动操作系统
- Legacy BIOS
- UEFI (unified extensible firmware interface)

**Legacy BIOS**：
- 查看存在操作系统的磁盘，主引导区的 512 字节以 `0xaa55` 结束
- 将第一导引位磁盘的主引导扇区里的512个字节搬运到内存的 `0x7c00` 位置，且 PC 设置为 `0x7c00`
- 主引导扇区的程序就开始搬运操作系统到内存

**UEFI**: 几个大厂商达成的协议，都按照这个标准来加载硬件，硬件厂商也可以根据这套协议添加自己的硬件，安全性更高。

>[!tip|style:flat]
> 操作系统启动顺序：
>
> CPU 启动 --> 固件 --> boot loader --> 操作系统

# 操作系统状态机





