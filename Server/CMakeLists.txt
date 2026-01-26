# 自动包含 Server/src 下所有 C 文件
file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# 包含 sqlite3 源文件
file(GLOB SQLITE_SOURCES "${CMAKE_SOURCE_DIR}/vendor/sqlite/*.c")

add_executable(Server ${SERVER_SOURCES} ${SQLITE_SOURCES})

# 包含头文件路径
target_include_directories(Server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/IM/src
    ${CMAKE_SOURCE_DIR}/vendor/sqlite/include
)

target_link_libraries(Server PRIVATE IM)

# 跨平台库
if (WIN32)
    target_compile_definitions(Server PRIVATE HAVE_STRUCT_TIMESPEC WIN32 _WIN32)
    target_include_directories(Server PRIVATE "${CMAKE_SOURCE_DIR}/vendor/pthread/include")
    target_link_libraries(Server PRIVATE ws2_32)
    target_link_libraries(Server PRIVATE "${CMAKE_SOURCE_DIR}/vendor/pthread/lib/x86/pthreadVC2.lib")
        add_custom_command(TARGET Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Server>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/vendor/pthread/dll/x86/pthreadVC2.dll"
            $<TARGET_FILE_DIR:Server>
        COMMENT "Copying pthreadV2.dll to output directory"
    )
else()
    find_package(Threads REQUIRED)
    target_link_libraries(Server PRIVATE Threads::Threads)
endif()

target_compile_options(Server PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# 设置可执行文件输出目录
set_target_properties(Server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Server"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Server"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Server"
)
