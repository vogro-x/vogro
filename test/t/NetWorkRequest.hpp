#include "NetWorkRequest.h"

using namespace std;
using boost::asio::ip::tcp;

/// POST请求
string PostRequest(char* host, char* path, string form)
{
    long length = form.length();
    
    // 声明Asio基础: io_service（任务调度机）
    boost::asio::io_service io_service;
    
    // 获取服务器终端列表
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host,"http");
    tcp::resolver::iterator iter = resolver.resolve(query);
    
    // 尝试连接每一个终端，直到成功建立socket连接
    tcp::socket socket(io_service);
    boost::asio::connect(socket, iter);
    
    // 构建网络请求头
    // 指定 "Connection: close" 在获取应答后断开连接，确保获文件全部数据。
    boost::asio::streambuf request;
    ostream request_stream(&request);
    request_stream << "POST " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Type:application/x-www-form-urlencoded\r\n";
    request_stream << "Content-Length: " << length << "\r\n";
    request_stream << "Connection: close\r\n\r\n"; // 注意这里是两个空行
    request_stream << form; //POST 发送的数据本身不包含多于空行
    
    // 发送请求
    boost::asio::write(socket, request);
    
    // 读取应答状态. 应答缓冲流 streambuf 会自动增长至完整的行
    // 该增长可以在构造缓冲流时通过设置最大值限制
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
    
    // 检查应答是否OK.
    istream response_stream(&response);// 应答流
    string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    string status_message;
    getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        printf("无效响应\n");
    }
    if (status_code != 200)
    {
        printf("响应返回 status code %d\n", status_code);
    }
    
    // 读取应答头部，遇到空行后停止
    boost::asio::read_until(socket, response, "\r\n\r\n");
    
    // 显示应答头部
    string header;
    int len = 0;
    while (getline(response_stream, header) && header != "\r")
    {
        if (header.find("Content-Length: ") == 0) {
            stringstream stream;
            stream << header.substr(16);
            stream >> len;
        }
    }
    
    long size = response.size();
    
    if (size > 0) {
        // .... do nothing
    }
    
    // 循环读取数据流，直到文件结束
    boost::system::error_code error;
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
    {
        // 获取应答长度
        size = response.size();
        if (len != 0) {
            cout << size << "  Byte  " << (size * 100) / len << "%\n";
        }
        
    }
    if (error != boost::asio::error::eof)
    {
        throw boost::system::system_error(error);
    }
    
    cout << size << " Byte 内容已下载完毕." << endl;
    
    // 将streambuf类型转换为string类型返回
    istream is(&response);
    is.unsetf(ios_base::skipws);
    string sz;
    sz.append(istream_iterator<char>(is), istream_iterator<char>());
    
    // 返回转换后的字符串
    return sz;
}

/// GET请求
string GetRequest(char* host, char* path)
{
    // 声明Asio基础: io_service（任务调度机）
    boost::asio::io_service io_service;
    
    // 获取服务器终端列表
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host,"http");
    tcp::resolver::iterator iter = resolver.resolve(query);

    // 尝试连接每一个终端，直到成功建立socket连接
    tcp::socket socket(io_service);
    boost::asio::connect(socket, iter);
    
    // 构建网络请求头.
    // 指定 "Connection: close" 在获取应答后断开连接，确保获文件全部数据。
    boost::asio::streambuf request;
    ostream request_stream(&request);
    request_stream << "GET " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    
    // 发送请求
    boost::asio::write(socket, request);
    
    // 读取应答状态. 应答缓冲流 streambuf 会自动增长至完整的行
    // 该增长可以在构造缓冲流时通过设置最大值限制
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
    
    // 检查应答是否OK.
    istream response_stream(&response);
    string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    string status_message;
    getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        printf("响应无效\n");
    }
    if (status_code != 200)
    {
        printf("响应返回 status code %d\n", status_code);
    }
    
    // 读取应答头部，遇到空行后停止
    boost::asio::read_until(socket, response, "\r\n\r\n");
    
    // 显示应答头部
    
    string header;
    int len = 0;
    while (getline(response_stream, header) && header != "\r")
    {
        if (header.find("Content-Length: ") == 0) {
            stringstream stream;
            stream << header.substr(16);
            stream >> len;
        }
    }
    
    long size = response.size();

    if (size > 0) {
        // ... do nothing ...
    }
    
    boost::system::error_code error;  // 读取错误
    
    // 循环读取数据流，直到文件结束
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
    {
        // 获取应答长度
        size = response.size();
        if (len != 0) {
            cout << size << "  Byte  " << (size * 100) / len << "%" << endl;
        }
    }

    // 如果没有读到文件尾，抛出异常
    if (error != boost::asio::error::eof)
    {
        throw boost::system::system_error(error);
    }
    
    cout << size << " Byte 内容已下载完毕." << endl;
    
    // 将streambuf类型转换为string类型，并返回
    istream is(&response);
    is.unsetf(ios_base::skipws);
    string sz;
    sz.append(istream_iterator<char>(is), istream_iterator<char>());
    
    return sz;
}

/// GET请求(重载)
string GetRequest(string url)
{
    size_t index;
    
    // 去掉url中的协议头
    if (url.find("http://") != string::npos ) {
        url = url.substr(7);
    }
    printf("url:%s\n", url.c_str());
    
    // 截取host字符串
    index = url.find("/");
    char* host = new char[index];
    strcpy(host, url.substr(0, index).c_str());
    
    // 截取urlPath字符串
    char* urlPath = new char[url.length() - index + 1];
    strcpy(urlPath, url.substr(index, url.length() - index).c_str());
    
    return GetRequest(host, urlPath);
}