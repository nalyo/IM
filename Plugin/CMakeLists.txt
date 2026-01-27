cmake_minimum_required(VERSION 3.15)

project(AutoReplyPlugin C)

# 自动包含当前目录所有 C 文件
file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# 构建动态库
add_library(AutoReplyPlugin SHARED ${PLUGIN_SOURCES})

# 包含客户端头文件和 IM 库头文件
target_include_directories(AutoReplyPlugin PRIVATE
    "${CMAKE_SOURCE_DIR}/Client/src"
    "${CMAKE_SOURCE_DIR}/IM/src"
)

# 链接 IM 静态库
target_link_libraries(AutoReplyPlugin PRIVATE IM)

# 设置编译选项
target_compile_options(AutoReplyPlugin PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# -------------------------------
# 跨平台输出插件到 Client/bin/plugins
# -------------------------------
# 设置插件输出目录
set(PLUGIN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/Client/plugins")
file(MAKE_DIRECTORY ${PLUGIN_OUTPUT_DIR})

set_target_properties(AutoReplyPlugin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}          # Release/Debug 都统一输出
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PLUGIN_OUTPUT_DIR}
    OUTPUT_NAME "AutoReplyPlugin"
)

# 创建目录
file(MAKE_DIRECTORY ${PLUGIN_OUTPUT_DIR})

# Windows 需要定义 DLL 导出符号
if(WIN32)
    target_compile_definitions(AutoReplyPlugin PRIVATE BUILDING_DLL)
    # 设置输出目录为插件目录
    set_target_properties(AutoReplyPlugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
        OUTPUT_NAME "AutoReplyPlugin"  # DLL 名称
    )
else() # Linux/macOS
    set_target_properties(AutoReplyPlugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
        OUTPUT_NAME "AutoReplyPlugin"  # 去掉默认的 lib 前缀
    )
endif()

# 可选：打印生成路径方便调试
message(STATUS "Plugin will be output to: ${PLUGIN_OUTPUT_DIR}")
