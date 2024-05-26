# QIODevice

# 介绍

`QIODevice` 是 `QFile`、`QBuffer`、`QTcpSocket` 等数据流程操作的基类，定义了数据「读/写」相关的 `API`。

# 类型

对于 QIODevice 将数据划分为两大类：
- **随机设备** : `QFile` 、`QBuffer` 等设备，可以通过 `seek()` 寻找位置
- **顺序设备** : `QTcpSocket`、`QProcess` 等设备，数据放在缓冲区，只要读取就没了，不能随机读取

```cpp
    /* true 表示顺序设备 */
    virtual bool isSequential() const;

	virtual qint64 pos() const;
    virtual bool seek(qint64 pos);
    virtual bool atEnd() const;
    virtual bool reset();

    /**
     * 随机设备：数据的总字节数
     * 顺序设备：通过 bytesAvailable()
     */
    virtual qint64 size() const;

    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
```

# 读

```cpp
    qint64 read(char *data, qint64 maxlen);
    QByteArray read(qint64 maxlen);
    QByteArray readAll();
    qint64 readLine(char *data, qint64 maxlen);
    QByteArray readLine(qint64 maxlen = 0);
    virtual bool canReadLine() const;

    qint64 skip(qint64 maxSize);

    // 功能类似 read
    qint64 peek(char *data, qint64 maxlen);
    QByteArray peek(qint64 maxlen);
```

# 写

```cpp
	qint64 write(const char *data, qint64 len);
    qint64 write(const char *data);
    inline qint64 write(const QByteArray &data)
    { return write(data.constData(), data.size()); }

    // 撤销 getChar 的操作
    void ungetChar(char c);
    bool putChar(char c);
    bool getChar(char *c);
```

# 信号

```cpp
    // 有数据可读，但并不是完整全部的数据，例如 TCP 一次发送数据过多，操作系统则会接收多次，该信号也会触发多次
    void readyRead();

    // 数据写入
    void bytesWritten(qint64 bytes);

    // 带 channel 的读写
    void channelReadyRead(int channel);
    void channelBytesWritten(int channel, qint64 bytes);

    // 写端关闭
    void readChannelFinished();

    // 设备要关闭了
    void aboutToClose();
```

# 阻塞

QIODevice 中的 `read()` 与 `write()` 是「无阻塞」的，每次读/写操作都是马上返回。使用 `waitFor...()` 类接口才具有阻塞功能 ( **直接阻塞线程，并不会进入 `QEventLoop`** )。

```cpp
	virtual bool waitForReadyRead(int msecs);
    virtual bool waitForBytesWritten(int msecs);
```


# 事务

读取 `QTcpSocket` 时，数据块可以在任意时间点到达，因此一次读取的数据可能是不完整的，这样就可以使用「事务」保证数据完整性。

```cpp
void startTransaction();
void commitTransaction(); // 顺序设备，会清空缓冲区中的数据
void rollbackTransaction();
bool isTransactionStarted() const;
```

# 缓冲区

某些子类（QFile和QTcpSocket）是使用内存缓冲区实现的，用于中间存储数据。这减少了设备访问调用的数量，这些调用通常非常慢。缓冲使getChar()和putChar()等函数变得快速，因为它们可以在内存缓冲区上运行，而不是直接在设备本身上运行。但是，某些 I/O 操作不能很好地与缓冲区配合使用。例如，如果多个用户打开同一设备并逐个字符读取它，则当他们打算读取每个单独的块时，他们最终可能会读取相同的数据。出于这个原因，QIODevice 允许通过将 `Unbuffered` 标志传递给 `open()` 将关闭缓冲区。

```cpp

/* QIODevice 的数据缓冲区 */
class QRingBuffer
{
public:
    inline int getChar() {/* ........ */}
    inline void putChar(char c) {
        char *ptr = reserve(1);//返回地址
        *ptr = c;
    }
    Q_CORE_EXPORT qint64 read(char *data, qint64 maxLength);
    //..........................
private:
    QVector<QRingChunk> buffers; // 缓冲区，是一个动态数组
    qint64 bufferSize; // 缓冲区大小
    int basicBlockSize; // 数据块大小，默认 4096
};
```