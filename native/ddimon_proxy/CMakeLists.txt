cmake_minimum_required(VERSION 3.20)
project(DdiMonProxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# spdlog
find_package(spdlog CONFIG REQUIRED)

# DdiMon Client library
add_library(ddimon_client STATIC
    ../ddimon_client/ddimon_client.cpp
)
target_include_directories(ddimon_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ddimon_client
)
target_link_libraries(ddimon_client PRIVATE
    spdlog::spdlog
)

# DdiMon Proxy DLL
add_library(ddimon_proxy SHARED
    ddimon_proxy.cpp
)
target_include_directories(ddimon_proxy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ddimon_client
)
target_link_libraries(ddimon_proxy PRIVATE
    ddimon_client
    spdlog::spdlog
)

# 设置输出名称
set_target_properties(ddimon_proxy PROPERTIES
    OUTPUT_NAME "mmmojo_64"
    PREFIX ""
)
