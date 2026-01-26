# 自动包含 Client/src 下所有 C 文件
file(GLOB_RECURSE CLIENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(Client ${CLIENT_SOURCES})

target_include_directories(Client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/IM/src
)

target_link_libraries(Client PRIVATE IM)

# 跨平台库
if (WIN32)
    target_compile_definitions(Client PRIVATE HAVE_STRUCT_TIMESPEC WIN32 _WIN32)
    target_include_directories(Client PRIVATE "${CMAKE_SOURCE_DIR}/vendor/pthread/include")
    target_link_libraries(Client PRIVATE ws2_32)
    target_link_libraries(Client PRIVATE "${CMAKE_SOURCE_DIR}/vendor/pthread/lib/x86/pthreadVC2.lib")
        add_custom_command(TARGET Client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Client>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/vendor/pthread/dll/x86/pthreadVC2.dll"
            $<TARGET_FILE_DIR:Client>
        COMMENT "Copying pthreadV2.dll to output directory"
    )
else()
    find_package(Threads REQUIRED)
    target_link_libraries(Client PRIVATE Threads::Threads)
endif()

target_compile_options(Client PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)
# 设置可执行文件输出目录
set_target_properties(Client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Client"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Client"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Client"
)
