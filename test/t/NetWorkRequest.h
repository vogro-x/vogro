#include <stdio.h>
#include <iostream>
#include <fstream>
// #include "json.h"
#include <boost/asio.hpp>

using namespace std;

/// GET请求
string GetRequest(char* host, char* path);
string GetRequest(string url);

/// POST请求
string PostRequest(char* host, char* path, string form);
