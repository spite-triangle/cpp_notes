# 日志

# 基础使用

```python
import logging

# 配置基础日志
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    filename='./logs/a.log',
    filemode='a'
)

# 日志打印
logging.debug('调试信息')
logging.info('常规信息')
logging.warning('警告信息')
logging.error('错误信息')
logging.critical('严重错误')
```

# 新建 logger

```python
import logging

# 通过 getLogger() 可以创建新的 logger 且配置会继承 basicConfig()
logger = logging.getLogger(__name__)

# 创建新的 handler
handler = logging.FileHandler(filename='./logs/b.log', mode='w')

# 创建格式化器
formatter = logging.Formatter(
    '[%(asctime)s] %(levelname)-8s %(name)s:%(lineno)d - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
handler.setFormatter(formatter)

# 新创建的 logger 增加新的日志 handler
# NOTE - 这里是添加新的 handler ，因此 basicConfig() 配置的 handler 也会记录日志
logger.addHandler(handler)

# 日志打印
logger.debug('调试信息')
logger.info('常规信息')
logger.warning('警告信息')
logger.error('错误信息')
logger.critical('严重错误')
```

`logger.addHandler(handler)`  是添加新的 `handler` ，因此 `basicConfig()` 配置的 `handler` 也会记录日志

```python
# 清空所有 handler
handler.handlers.clear()
# 删除特定 handler 
handler.removeHandler(hdl)
```

# handler

```python
import logging
import logging.handlers as handlers

# 控制台
logging.StreamHandler()

# 本地文件
logging.FileHandler("app.log")

# 占位
logging.NullHandler()

# 按文件大小切割日志
handlers.RotatingFileHandler("app.log", maxBytes=10*1024*1024, backupCount=3)

# 按时间切割日志
handlers.TimedRotatingFileHandler("app.log", when="midnight", interval=1, backupCount=7)

# 邮件发送
handlers.SMTPHandler(
    mailhost=("smtp.example.com", 587),
    fromaddr="alerts@example.com",
    toaddrs=["admin@example.com"],
    subject="App Error",
    credentials=("username", "password")
)

# socket 发送
handlers.SocketHandler("localhost", 9020)

# linux 系统日志服务
handlers.SysLogHandler(address="/dev/log")

# http
handlers.HTTPHandler("example.com:80", "/log_endpoint", method="POST")

# 缓存日志，到一定量后触发其他 handler
handlers.MemoryHandler(capacity=100, target=FileHandler("app.log"))

# 监视日志文件是否被外部修改
handlers.WatchedFileHandler("app.log")
```

# 配置文件

```python
import logging.config

LOGGING_CONFIG = {
    "version": 1,
    "disable_existing_loggers": False,
    # 格式化
    "formatters": {
        "standard": {
            "format": "%(asctime)s %(name)s %(levelname)s %(message)s"
        }
    },
    # handler
    "handlers": {
        "console": {
            # 可以通过 class 自定义 handler
            "class": "logging.StreamHandler",
            "formatter": "standard",
            "level": "INFO"
        },
        "file": {
            "class": "logging.handlers.RotatingFileHandler",
            "filename": "./logs/app.log",
            "maxBytes": 10485760,
            "backupCount": 5,
            "formatter": "standard",
            "encoding": "utf8"
        }
    },
    # logger
    "root": {
        "handlers": ["console", "file"],
        "level": "INFO"
    }
}

logging.config.dictConfig(LOGGING_CONFIG)

logging.info('test')
```

# 坑

## 删日志

### 问题

`logging` 在单进程中没问题，**但是在多进程中，利用 `TimedRotatingFileHandler` 或 `RotatingFileHandler` 自动切割日志文件时，会导致日志丢失**。
1. 进程 A 、B 采用 `RotatingFileHandler` 方式往 `app.log` 中写入日志，且每 `20 MB` 分割一个日志
2. 在 `app.log` 文件大小为 `20 MB` 时 ，进程A，进程B 同时调用 `RotatingFileHandler`
3. 假设进程 A 先执行执行 
   1. 检查 `app_1.log` 是否存在，**若存在就删除**
   2. 重命名 `app.log` 为 `app_1.log`
   3. 往 `app.log` 写日志
4. 进程B 接着执行
   1. 检查 `app_1.log` 是否存在，**若存在就删除**
   2. 重命名 `app.log` 为 `app_1.log`
   3. 往 `app_1.log` 写日志

**由此可知，进程A备份的 `app_1.log` 会被进程B删除**

### TimedRotatingFileHandler

```python
# handlers.TimedRotatingFileHandler 源码
class TimeRotatingFileHandler(BaseRotatingHandler):
    ...
    def doRollover(self):
        ....

        # 获取文件名
        dfn = self.rotation_filename(self.baseFilename + "." +
                                     time.strftime(self.suffix, timeTuple))
        # 检测文件是否存在，存在则删除
        if os.path.exists(dfn):
            os.remove(dfn)
        
        # 将当前日志文件重命名为 dfn
        self.rotate(self.baseFilename, dfn)
        ....
```

将上述 `TimedRotatingFileHandler` 的源码修改为


```python
class TimeRotatingFileHandler(BaseRotatingHandler):
    ...
    def doRollover(self):
        ....

        # dfn = self.rotation_filename(self.baseFilename + "." +
        #                              time.strftime(self.suffix, timeTuple))
        # if os.path.exists(dfn):
        #     os.remove(dfn)
        # self.rotate(self.baseFilename, dfn)

        dfn = self.rotation_filename(self.baseFilename + "." +
                                     time.strftime(self.suffix, timeTuple))
        # dfn 不存在，才将当前日志文件重命名为 dfn
        if not os.path.exists(dfn):
            self.rotate(self.baseFilename, dfn)
        ....
```

**该方式只能解决 `TimeRotatingFileHandler` 的问题，因为 `dfn` 按照日期命名，不会二次覆盖。**


## 无法续写

### 问题

采用 `RotatingFileHandler` 、`TimedRotatingFileHandler` 、`FileHandler` 等在本地文件中记录日志的 handler 时，**若在程序运行过程中，修改了指定的日志文件（删除、修改、重命名），logging 则无法往文件中写入日志。**


### 文件检测

检测打开的文件对象 `f` 的 `stat` 与文件系统中文件的 `stat` 是否一致，不一致则重新调用 `open()` 打开文件
 
```python
import os
import stat

# 获取打开的文件标识
f = open('./logs/app.log', mode='a', encoding='utf-8')
sres = os.fstat(f.fileno())

while True:
    # 读取文件标识
    try:
        curr = os.stat('./logs/app.log')
    except: FileNotFoundError:
        curr = None

    # 检测文件是否被别人修改
    if not curr or curr[stat.ST_DEV] != sres[stat.ST_DEV] or curr[stat.ST_INO] != sres[stat.ST_INO]:
        # 重新打开
        f = open('./logs/app.log', mode='a', encoding='utf-8')

        # 更新状态
        sres = os.fstat(f.fileno())
    
    do_something(f)
```

## 多进程写

`logging` 非进程安全库，因此多进程同时写，会导致日志记录混乱


# 解决方案

## 自定义 handler

自定义 `TimedRotatingFileHandler`，解决思路
1. 继承 `TimedRotatingFileHandler` 
2. 重写 `doRollover()` 解决日志误删问题
3. 重写 `emit()` 解决日志文件被修改，无法写问题；以及添加进程锁，防止多进程同时写
4. 使用自定义的 `handler` 写日志

## concurrent-log-handler

由第三方库提供多进程安全的 handler

```term
triangle@LEARN:~$ pip install concurrent-log-handler
```


## loguru

### 基本用法

使用 `loguru` 库替代 `logging` 进行日志记录

```python
import sys
from loguru import logger

# 清除之前的日志处理器
logger.remove()

# 添加日志处理器，打印到控制台
logger.add(
    sys.stderr,  # 控制台输出
    format="<green>{time:YYYY-MM-DD HH:mm:ss}</> | <level>{level: <8}</> | <cyan>{name}</>:<cyan>{function}</> - <level>{message}</>",
    level="DEBUG",
    colorize=True  # 智能禁用颜色（当非TTY时）
)

# 添加日志处理器，日志分割
logger.add(
    sink="logs/app_{time:YYYYMMDD}.log",
    format="{time:YYYY-MM-DD HH:mm:ss} {extra[value]} {level} From {module}.{function} : {message}",
    encoding="utf-8",
    level="debug", # 打印日志的最低等级
    rotation="100 MB",  # 自动分割文件大小，时间
    compression="zip",  # 旧日志压缩
    retention="30 days",  # 过期清理
    enqueue=True,  # 多进程安全
    backtrace=True,  # 堆栈追踪深度控制
    diagnose=False,  # 生产环境关闭敏感信息
    serialize=False, # 以 json 格式输出日志
    catch=True, # 日志内部发生异常不会抛出
)

logger.warning('warning')
logger.debug('debug')
logger.info('info')
logger.error('error')
```

`format` 可配置参数

| Key       | Description                                           | Attributes               |
| --------- | ----------------------------------------------------- | ------------------------ |
| elapsed   | The time elapsed since the start of the program       | See datetime.timedelta   |
| exception | The formatted exception if any, None otherwise        | type, value, traceback   |
| extra     | The dict of attributes bound by the user (see bind()) | None                     |
| file      | The file where the logging call was made              | name (default), path     |
| function  | The function from which the logging call was made     | None                     |
| level     | The severity used to log the message                  | name (default), no, icon |
| line      | The line number in the source code                    | None                     |
| message   | The logged message (not yet formatted)                | None                     |
| module    | The module where the logging call was made            | None                     |
| name      | The `__name__` where the logging call was made        | None                     |
| process   | The process in which the logging call was made        | name, id (default)       |
| thread    | The thread in which the logging call was made         | name, id (default)       |
| time      | The aware local time when the logging call was made   | See datetime.datetime    |

### 配置文件

```python
config = {
    "handlers": [
        {"sink": sys.stdout, "format": "{time} - {message}"},
        {"sink": "file.log", "serialize": True},
    ],
    "extra": {"user": "someone"}
}
logger.configure(**config)
```

### 绑定

```python
import sys
from loguru import logger 

logger.add(sys.stdout, format="{time:YYYY-MM-DD HH:mm:ss} {extra[value]} {level} From {module}.{function} : {message}")

# 给 {extra[value]} 绑定参数，生成新的 logger
extra_log = logger.bind(value='xxxxxx')
extra_log.info("绑定参数")
# 2025-05-21 21:03:00 xxxxxx INFO From logger.<module> : 绑定参数
```


### 异常捕获

- **装饰器**

```python
from loguru import logger 

# 该装饰器会直接记录依赖信息
@logger.catch
def fcn():
    1/ 0

fcn()
```

- **捕获**

```python
try:
    1/ 0
except Exception as e:
    logger.exception(f'{e}')
```



### 简单封装

```python
from pathlib import Path
from loguru import logger as loguru_logger

class Logger:
    _instance = None

    def __init__(self, log_dir: str = "./log", log_file: str = "log.log"):
        self._logger = loguru_logger
        self._log_dir = log_dir
        self._log_file = log_file
        self._init_log()

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(Logger, cls).__new__(cls)
        return cls._instance

    def _init_log(self):
        dir = Path(self._log_dir)
        if not dir.exists():
            Path.mkdir(dir)

        loguru_logger.add(
            Path(self._log_dir, self._log_file).resolve(),
        )

    def logger(self):
        return self._logger

logger = Logger().logger()
```





