#设置项目名称
PROJECT(Vogro)

#要求CMake的最低版本为2.8
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_COMPILE_OPTIONS(-std=c++11 -O3)

#添加头文件搜索路径
INCLUDE_DIRECTORIES(/usr/local/include
        /usr/include/boost
        /usr/include/openssl
        ./middleware
        ./handler
        ../src
        ../src/vogro
        )


#添加库文件搜索路径
LINK_DIRECTORIES(/usr/local/lib /usr/lib)

#用于将当前目录下的所有源文件的名字保存在变量 DIR_SRCS 中
#AUX_SOURCE_DIRECTORY(client.cpp CLIENT_SRCS)
AUX_SOURCE_DIRECTORY(server.cpp SERVER_SRCS)


#用于指定从一组源文件 source1 source2 … sourceN(在变量DIR_SRCS中定义)
#编译出一个可执行文件且命名为CMake_Tutorial1
ADD_EXECUTABLE(vogroServer main.cpp)
ADD_EXECUTABLE(testClient test/test.cpp)


#添加编译可执行程序所需要的链接库、如果有多个中间用空格隔开
#第一个参数是可执行程序名称，第二个开始是依赖库
#在这里根据名字boost_thread去寻找libboost_thread.a文件
TARGET_LINK_LIBRARIES(vogroServer pthread boost_system ssl crypto)
TARGET_LINK_LIBRARIES(testClient pthread boost_system)