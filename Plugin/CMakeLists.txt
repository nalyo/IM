project(AutoReplyPlugin C)

# 自动包含当前目录所有 C 文件
file(GLOB PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# 构建动态库
add_library(AutoReplyPlugin SHARED ${PLUGIN_SOURCES})

# 包含客户端头文件和 IM 库头文件
target_include_directories(AutoReplyPlugin PRIVATE
    "${CMAKE_SOURCE_DIR}/Client/src"
    "${CMAKE_SOURCE_DIR}/IM/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/cJSON"       
    "${CMAKE_CURRENT_SOURCE_DIR}/curl/include" 
)

# 链接 IM 静态库
target_link_libraries(AutoReplyPlugin PRIVATE IM)
target_link_libraries(AutoReplyPlugin PRIVATE logc)
# 添加 cJSON
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cJSON")
target_link_libraries(AutoReplyPlugin PRIVATE cjson)
# ===============================
# curl 最小化配置（必须在 add_subdirectory 前）
# ===============================

# 关闭一堆没必要的第三方库
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
set(CURL_ZSTD OFF CACHE BOOL "" FORCE)
set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)
set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)

# 只要 libcurl，不要 curl.exe
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)

# Windows 下用系统 TLS
set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
# 添加 libcurl
# 假设 libcurl 自带 CMakeLists.txt 或你写了子模块 CMake
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/curl")
target_link_libraries(AutoReplyPlugin PRIVATE libcurl)

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
