# win32serviceutil


# 简介

## SCM

**服务管理器`SCM(service control manager)`** : 是 `windows` 平台专门用于后台服务管理的工具，类似 `linux` 中的 `systemctl`。想要托管给 `windows` 的后台服务，均需要注册到 `SCM` 中，并在 `计算机\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\` 能查看到相应设置。

```term
triangle@LEARN:~$ sc // cmd 上提供的管理命令
triangle@LEARN:~$ sc query <service name> // 查看服务
```

## win32serviceutil


- `pywin32` : 对 `windows` 底层接口的封装
- `win32service` : 对 `pywin32` 中 `service` 相关接口的封装
- `win32serviceutil`: 对 `win32service` 的使用封装
- `pythonservice.exe` : 在 `python` 中，会首先将 `pythonservice.exe` 注册到 `SCM` 中，作为启动 `python` 服务的程序壳；然后通过 `pythonservice.exe` 提供 `pywin32` 服务；最后在通过 `win32serviceutil` 实现用户自己的服务。

# win32serviceutil

- `service.py`

```python
import os
import os.path
import sys
import win32service
import win32serviceutil
import servicemanager
import multiprocessing

def main():
    """ 服务程序 """
    import time
    time.sleep(600)  


class ProcessService(win32serviceutil.ServiceFramework):
    _svc_name_ = "SleepService"             # 服务名
    _svc_display_name_ = "Sleep Service"    # 别名
    _svc_description_ = "Sleeps for 600"    # 描述

    # NOTE - 可实现加载 venv 环境
    _exe_name_ = sys.executable # python.exe from venv
    _exe_args_ = '-u -E "' + os.path.abspath(__file__) + '"'

    proc = None

    def __init__(self,args):
        win32serviceutil.ServiceFramework.__init__(self, args)

        # 获取工作目录
        self._cwd = os.path.dirname(os.path.abspath(__file__))

    def SvcStop(self):
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)
        if self.proc:
            self.proc.terminate()

    def SvcDoRun(self):
        """ 服务程序入口 """

        # NOTE - 设定工作目录
        os.chdir(self._cwd)

        # 启动服务程序
        self.proc = multiprocessing.Process(target=main)
        self.proc.start()        
        self.proc.join()

def start():
    if len(sys.argv)==1:
        servicemanager.Initialize()
        servicemanager.PrepareToHostSingle(ProcessService)
        servicemanager.StartServiceCtrlDispatcher()
    else:
        win32serviceutil.HandleCommandLine(ProcessService)

if __name__ == '__main__':
    try:
        start()
    except (SystemExit, KeyboardInterrupt):
        raise
    except:
        import traceback
        traceback.print_exc()
```

```term
triangle@LEARN:~$ python service.py install // 安装服务
triangle@LEARN:~$ python service.py start // 启动服务
triangle@LEARN:~$ python service.py stop // 停止服务
triangle@LEARN:~$ python service.py remove // 卸载服务
```

> [!note]
> 当 `SvcDoRun()` 退出后，则服务并停止运行


# 重启

默认情况下 `SvcDoRun()` 退出（包括正常、异常情况），服务便会停止，为了应当故障情况，需要对服务添加额外设置

```term
triangle@LEARN:~$ sc failure /help // 必须在 cmd 下执行
描述:
        更改失败时操作
用法:
        sc <server> failure [service name] <option1> <option2>...

选项:
        reset=   <重置失败计数为 0 (可以是无限)
                  之前无错的时间长度(以秒计)>
                  (必须与 actions= 一起使用)
        reboot=  <失败时重新启动之前的消息广播>
        command= <失败时运行的命令行>
        actions= <失败操作和其延迟时间(以微秒计)，
                  以 / (斜杠) 分隔 -- 如 run/5000/reboot/800
                  有效的操作是 <run|restart|reboot> >
                  (必须与 reset= option 一起使用)
triangle@LEARN:~$ sc failureflag /help // 必须在 cmd 下执行
用法:
        sc <server> failureflag [service name] [flag]
flag:
    0           默认，服务以 SERVICE_STOPPED 状态退出，不会触发 failure 的 actions
    1           服务以 SERVICE_STOPPED 状态退出且退出码 >0 时会触发 actions
triangle@LEARN:~$ sc qfailure  <service name> // 查看配置
triangle@LEARN:~$ sc qfailureflag <service name> // 查看配置
```

> [!note]
> 无论 `SvcDoRun()` 内部是否抛异常，服务均会以 `SERVICE_STOPPED` 状态退出，但抛出异常时，退出码 `>0`


