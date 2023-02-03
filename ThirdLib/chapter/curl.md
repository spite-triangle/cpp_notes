# curl

[Linux下的libcurl库的使用(学习总结)](https://blog.csdn.net/weixin_39688262/article/details/117293939)
[libcurl 使用方法简介](https://blog.csdn.net/myvest/article/details/82899788)
[curl库的用法](https://blog.csdn.net/Quellaaa/article/details/103149911)


```cpp

#include <stdio.h>

#include <curl/curl.h>
#include <curl/easy.h>

size_t WriteCallback(void *ptr, size_t size, size_t count, void *user)
{
    int nWritten = fwrite(ptr, size, count, reinterpret_cast<FILE*>(user));
    return nWritten;
}

int main(int argc, char const *argv[])
{
    // 创建文件流，二进制形式
    FILE* file = fopen("./data/test.zip","wb");

    // 初始化句柄
    CURL* pHandle = curl_easy_init();

    // 设置 url
    curl_easy_setopt(pHandle, CURLOPT_URL, "http://file.zip");

    // 设置 WriteCallback 中的 user
    curl_easy_setopt(pHandle, CURLOPT_WRITEDATA, (void*)file);

    // 设置下载回调
    curl_easy_setopt(pHandle, CURLOPT_WRITEFUNCTION, WriteCallback);

    // 进度显示到控制台
    curl_easy_setopt(pHandle, CURLOPT_NOPROGRESS, 0L);

    // 执行下载
    curl_easy_perform(pHandle);

    // 销毁句柄
    curl_easy_cleanup(pHandle);

    fclose(file);
    return 0;
}
```