选中 first - last ，右键，选择 set time range 可以缩小范围

选中需要的项，右键，include item，只显示选中项

# 单线程


1. 首先查看 CallTree 界面，分析所有线程
2. 缩小范围，通过 first - last 缩小范围；通过 `cpu=xxxms` 区分哪个线程是真正干活的
3. 重点关注 `inc` 指标，找到哪个函数在使劲耗时

by name 总结
- `BLOCKED_TIME` : 阻塞时间
- `HARD_FAULT` : 故障时间，例如操作系统处理页中断的时间
- `DISK_TIME`： 操作磁盘的时间。但是等待的时间都被算到了 「阻塞」时间上
- `CPU_TIME` : cpu 运行时间。选中该项，然后执行 `include item` 就能把界面变成 CPU Stack view 的形式

# 多线程

整体分析方法和 「单线程」一样，唯一区别是线程关注区间不好确认。这个就需要分析 `Thread Time Stack (with task)`。

 `Thread Time Stack (with task)` 的整体界面和 `Thread Time Stack` 一样，唯一区别是将工作线程的 `Inc` 指标全部算到了任务发起线程的头上，例如线程池中的线程都是在处理任务，而这些任务确实其他线程发起的，因此 prefview 就把这些工作线程的 `Inc` 全算到了发起任务的线程头上。

好处就是将子线程的性能消耗都归纳到了主线程中，这样就能像单线程一样分析性能消耗。

> [!note]
> 但仅在使用 System.Diagnostics.Threading.Tasks.Task 类生成并发活动时才有效。c++ 就别指望用上了