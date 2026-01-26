# 自动包含 IM/src 下所有 C 文件
file(GLOB_RECURSE IM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(IM STATIC ${IM_SOURCES})

target_include_directories(IM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/pthread/include
)

# 编译警告
target_compile_options(IM PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# 跨平台宏和库
if (WIN32)
    target_compile_definitions(IM PUBLIC HAVE_STRUCT_TIMESPEC WIN32)
    target_link_libraries(IM PUBLIC ws2_32)
else()
    target_link_libraries(IM PUBLIC pthread)
endif()
