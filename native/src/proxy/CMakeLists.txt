cmake_minimum_required(VERSION 3.20)
project(MmmojoProxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# 代理 DLL
add_library(mmojo_proxy SHARED
    mmmojo_proxy.cpp
)

target_include_directories(mmojo_proxy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 禁用 Control Flow Guard (CFG) - 因为代理 DLL 的函数表不完整
# target_compile_options(mmojo_proxy PRIVATE /guard:cf)
# target_link_options(mmojo_proxy PRIVATE /guard:cf /guard:ehcont)

# 设置输出名称为 mmojo_64.dll
set_target_properties(mmojo_proxy PROPERTIES
    OUTPUT_NAME "mmmojo_64"
    PREFIX ""
)

