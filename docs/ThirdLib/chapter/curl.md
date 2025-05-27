# curl

- [Linux下的libcurl库的使用(学习总结)](https://blog.csdn.net/weixin_39688262/article/details/117293939)
- [libcurl 使用方法简介](https://blog.csdn.net/myvest/article/details/82899788)
- [curl库的用法](https://blog.csdn.net/Quellaaa/article/details/103149911)


> [!note]
> `libcurl` 只能作为客户端使用

# 基础流程

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

# TCP 协议

```cpp
#include <curl/curl.h>

int main(){

    curl_socket_t sockfd;

    // 初始化句柄
    CURL* curl =  curl_easy_init();

    // 设置 curl
    curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1");
    curl_easy_setopt(curl, CURLOPT_PORT, 7102);
    // it tells the library to perform all 
    // the required proxy authentication and connection setup,
    // but no data transfer, and then return.
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);

    // 执行
    curl_easy_perform(curl);

    // 获取 socket 
    long sockextr;
    curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
    sockfd = (curl_socket_t)sockextr;

    // 发送
    curl_easy_send(curl, request, strlen(request), &iolen);

    // 接收
    curl_easy_recv(curl, buf, 1024, &iolen);

    // 关闭
    curl_easy_cleanup(curl);
}
```

# http

## get


```cpp
#include <string>
#include <iostream>
#include <curl/curl.h>


struct RESPONSE_HEAD{
    std::string strContentType;
    int nStatusCode;
};

// NOTE - windows 没有该函数，linux 有
int strncasecmp(const char *s1, const char *s2, size_t n) {
    for(size_t i = 0; i < n; i++) {
        int c1 = tolower((unsigned char)s1[i]);
        int c2 = tolower((unsigned char)s2[i]);
        
        if(c1 != c2) {
            return c1 - c2;
        }
        
        if(c1 == '\0') {
            return 0;
        }
    }
    return 0;
}

// 一次回调，buffer 只接收一行头信息
static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    size_t realsize = size * nitems;
    auto pData = reinterpret_cast<RESPONSE_HEAD*>(userdata);

    // 检查Content-Type是否为允许的类型
    if(strncasecmp(buffer, "Content-Type:", 13) == 0) {
        if(strstr(buffer + 13, "application/json")){
            pData->strContentType = "application/json";
        }
    }
    
    // 检查HTTP状态码
    if(strncasecmp(buffer, "HTTP/", 5) == 0) {
        pData->nStatusCode = atoi(buffer + 9); // 跳过"HTTP/1.1 "
    }
    return realsize;
}

// 回调函数
// - 可用于下载，传入文件流 std::ofstream
// - 也可用于接收响应体，传入字符流 std::string
static size_t WriteData(char* buffer, size_t size, size_t nmemb, void* userp) {
    auto resp = static_cast<std::string*>(userp);
    resp->append(buffer, size*nmemb);
    return size * nmemb;
}


void get(){
    CURL* curl;

    // 设置curl选项
    curl_easy_setopt(curl, CURLOPT_URL, "url");

    // 请求头
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "accept: application/octet-stream");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // 请求
    curl_easy_setopt(pHandle, CURLOPT_HTTPGET, 1);

    // 响应头
    RESPONSE_HEAD stHead;
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &stHead);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

    // 响应
    std::string resp = "";
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 跟随重定向
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);

    // 超时设置
    curl_easy_setopt(pHandle, CURLOPT_TIMEOUT_MS, 10000);
    curl_easy_setopt(pHandle, CURLOPT_CONNECTTIMEOUT_MS, 10000);

    // 执行
    auto res = curl_easy_perform(curl);

    curl_slist_free_all(headers); // 清理头
    curl_easy_cleanup(curl); 
}
```


## post

```cpp
#include <string>
#include <fstream>
#include <iostream>
#include <curl/curl.h>


struct RESPONSE_HEAD{
    std::string strContentType;
    int nStatusCode;
};

// NOTE - windows 没有该函数，linux 有
int strncasecmp(const char *s1, const char *s2, size_t n) {
    for(size_t i = 0; i < n; i++) {
        int c1 = tolower((unsigned char)s1[i]);
        int c2 = tolower((unsigned char)s2[i]);
        
        if(c1 != c2) {
            return c1 - c2;
        }
        
        if(c1 == '\0') {
            return 0;
        }
    }
    return 0;
}

// 一次回调，buffer 只接收一行头信息
static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    size_t realsize = size * nitems;
    auto pData = reinterpret_cast<RESPONSE_HEAD*>(userdata);

    // 检查Content-Type是否为允许的类型
    if(strncasecmp(buffer, "Content-Type:", 13) == 0) {
        if(strstr(buffer + 13, "application/json")){
            pData->strContentType = "application/json";
        }
    }
    
    // 检查HTTP状态码
    if(strncasecmp(buffer, "HTTP/", 5) == 0) {
        pData->nStatusCode = atoi(buffer + 9); // 跳过"HTTP/1.1 "
    }
    return realsize;
}

// 回调函数
// - 可用于下载，传入文件流 std::ofstream
// - 也可用于接收响应体，传入字符流 std::string
static size_t WriteData(char* buffer, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outFile = static_cast<std::ofstream*>(userp);
    outFile->write(buffer, size * nmemb);

    // 必须要 flush ，否则可能异常
    outFile->flush();
    return size * nmemb;
}

// 进度回调函数
static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, 
                          curl_off_t ultotal, curl_off_t ulnow) {
    // 计算下载百分比
    if (dltotal > 0) {
        double progress = (double)dlnow / (double)dltotal * 100.0;
        std::cout << "\r下载进度: " << progress << "% (" << dlnow << "/" << dltotal << " bytes)";
        std::cout.flush();
    }
    return 0;
}


void post(){
    CURL* curl;

    // 设置curl选项
    curl_easy_setopt(curl, CURLOPT_URL, "url");

    // 请求头
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "accept: application/octet-stream");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // 请求
    std::string json = R"(
            {
                "files": [
                    "string"
                ]
            }
        )";
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json.length());

    // 响应头
    RESPONSE_HEAD stHead;
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &stHead);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

    // 响应
    auto outFile  = std::ofstream("xx","wb");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 跟随重定向
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);

    // 超时设置
    curl_easy_setopt(pHandle, CURLOPT_TIMEOUT_MS, 10000);
    curl_easy_setopt(pHandle, CURLOPT_CONNECTTIMEOUT_MS, 10000);

    // 执行
    auto res = curl_easy_perform(curl);

    curl_slist_free_all(headers); // 清理头
    curl_easy_cleanup(curl); 
    outFile.close();
}
```


## form


`MIME (Multipurpose Internet Mail Extensions)` 是描述消息内容类型的标准，用来表示文档、文件或字节流的性质和格式。请求体能包含文本、图像、音频、视频以及其他应用程序专用的数据。
- `text/plain` 
- `text/html`
- `image/jpeg`
- `video/mpeg`
- `application/json`
- .....

当发送 `multipart/form-data` 时，需要使用 `mime` 相关接口。

```cpp
#include <string>
#include <iostream>
#include <curl/curl.h>

void form(){
    CURL* curl;

    // 创建multipart表单
    curl_mime* mime = curl_mime_init(curl);

    // 所有的 part 数据都是键值对形式
    // name = data
    curl_mimepart* part = nullptr;
    
    // 添加文件
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "files");              // 字段名
    curl_mime_filename(part, "test.txt");       // 文件名，头信息
    curl_mime_filedata(part, "xx/xx/test.txt"); // 字段文件数据，也可以通过 curl_mime_data_cb(....) 控制发送细节

    // 添加数据
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "json_data");          // 字段名
    curl_mime_type(part, "application/json");   // 字段数据类型
    curl_mime_data(part, "{}");                 // 字段数据，本质上就是字符串

    curl_easy_setopt(curl, CURLOPT_URL, "url");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    
    // 执行请求
    auto res = curl_easy_perform(curl);
    
    // 清理资源
    curl_mime_free(mime);
    curl_easy_cleanup(curl);
}
```