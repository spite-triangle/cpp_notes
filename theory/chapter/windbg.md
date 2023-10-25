
# dump 

## 生成

- `cpp`接口生成

该方式生成的是 `mini dump` 文件，且某些异常可能会捕获不到，例如数组越界。

```cpp
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp.lib")
/ <summary>
/// 生成dmp文件
/// </summary>
/// <param name="exceptionPointers">异常信息</param>
/// <param name="path">文件路径（包括文件名）</param>
/// <returns></returns>
static int GenerateDump(EXCEPTION_POINTERS* exceptionPointers, const std::string& path)
{
	HANDLE hFile = ::CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION minidumpExceptionInformation;
		minidumpExceptionInformation.ThreadId = GetCurrentThreadId();
		minidumpExceptionInformation.ExceptionPointers = exceptionPointers;
		minidumpExceptionInformation.ClientPointers = TRUE;
		bool isMiniDumpGenerated = MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MINIDUMP_TYPE::MiniDumpNormal,
			&minidumpExceptionInformation,
			nullptr,
			nullptr);
		CloseHandle(hFile);
		if (!isMiniDumpGenerated)
		{
			printf("MiniDumpWriteDump failed\n");
		}
	}
	else
	{
		printf("Failed to create dump file\n");
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

// 注册异常处理函数，采用的是 SEH 机制
LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(
  [in] LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
);
```
- [CrashRpt](https://crashrpt.sourceforge.net/)

通过在 hook 接口 `CreateThread()` ，为每一个线程挂载一个异常 `unhandled exception filter`。这就导致 CrashRpt 没有修改到的线程，就不能捕获到相应异常信息。

- [Breakpad](https://github.com/google/breakpad)

实现了客户端与服务端，支持对 dump 的远程收集

![Breakpad|c,60](../../image/theory/Breakpad.jpg)

- 调试器中生成

- 任务管理器生成 `full dump`


## 分析

- 异常类型

```cpp

```

- 命令

```term
triangle@LEARN:~$ .ecxr // debuger切换到程序崩溃的线程 
triangle@LEARN:~$ kn // 函数调用堆栈
```