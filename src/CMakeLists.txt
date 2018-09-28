PROJECT(Vogro)

#要求CMake的最低版本为2.8
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#添加头文件搜索路径
INCLUDE_DIRECTORIES(/usr/local/include)

#添加库文件搜索路径
LINK_DIRECTORIES(/usr/local/lib)


# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(test-suite TEST_SUITE_SRCS)
aux_source_directory(server VOGRO_SERVER_SRCS)

# 生成链接库
add_library (VogroTestSuiteLib utils.hpp ${TEST_SUITE_SRCS})
add_library(VogroServerLib utils.hpp ${VOGRO_SERVER_SRCS})

target_link_libraries(VogroTestSuiteLib pthread boost_system)
target_link_libraries(VogroServerLib pthread boost_system)