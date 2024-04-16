# 标准库

# iomanip

| 控   制   符                     | 作           用                                          |
| -------------------------------- | -------------------------------------------------------- |
| `dec `                           | 设置整数为十进制                                         |
| `hex`                            | 设置整数为十六进制                                       |
| `oct`                            | 设置整数为八进制                                         |
| `setbase(n)`                     | 设置整数为n进制(n=8,10,16)                               |
| `setfill(c)`                     | 设置字符填充，c可以是字符常量或字符变量                  |
| `setprecision(n)`                | 设置浮点数的有效数字为n位                                |
| `setw(n)`                        | 设置字段宽度为n位                                        |
| `setiosflags(ios::fixed)`        | 设置浮点数以固定的小数位数显示                           |
| `setiosflags(ios::scientific)  ` | 设置浮点数以科学计数法表示                               |
| `setiosflags(ios::left)`         | 输出左对齐                                               |
| `setiosflags(ios::right)`        | 输出右对齐                                               |
| `setiosflags(ios::skipws)`       | 忽略前导空格                                             |
| `setiosflags(ios::uppercase)`    | 在以科学计数法输出E与十六进制输出X以大写输出，否则小写。 |
| `setiosflags(ios::showpos)`      | 输出正数时显示"+"号                                      |
| `setiosflags(ios::showpoint)`    | 强制显示小数点                                           |
| `resetiosflags() `               | 终止已经设置的输出格式状态，在括号中应指定内容           |


```cpp
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
 
int main() {
	int n=21;
	cout << n << endl;
	cout << "21八进制："<<setbase(8) << n <<" "<< oct << n <<  endl;//八进制输出
	cout << "21十六进制：" << setbase(16) << n << hex << n <<  endl;//十六进制输出
 
 
	cout << "固定字段宽为5位:";
	cout << setw(5) << n << endl;//默认为右对齐
	cout << "自动填充字符0：";
	cout << setfill('0') << setw(5) << n << endl;//字符填充
	cout << "左对齐：";
	cout << setiosflags(ios::left) << n << endl;//左对齐
	cout << "右对齐：";
	cout << setiosflags(ios::right) << n << endl;//右对齐
 
	double d = 3.14159;
	cout << "d=" << d << endl;
	cout << "浮点数六位有效小数：" ;
	cout << setprecision(6) << d <<endl;//四舍五入
	cout << "保留六位小数：";
	cout << fixed<<setprecision(6) << d << endl; // setiosflags(ios::fixed) 设置浮点数以固定的小数位数显示
	
}
```