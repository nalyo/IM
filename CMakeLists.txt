cmake_minimum_required(VERSION 3.22)
project(IMProject C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加子项目
add_subdirectory(IM)
add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(Plugin)