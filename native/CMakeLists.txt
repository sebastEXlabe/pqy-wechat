cmake_minimum_required(VERSION 3.20)
project(PQY-Native LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# MinHook
set(MINHOOK_DIR ${CMAKE_SOURCE_DIR}/../third-party/minhook)
add_library(minhook STATIC
    ${MINHOOK_DIR}/src/buffer.c
    ${MINHOOK_DIR}/src/hook.c
    ${MINHOOK_DIR}/src/trampoline.c
    ${MINHOOK_DIR}/src/hde/hde64.c
)
target_include_directories(minhook PUBLIC ${MINHOOK_DIR}/include)

# spdlog
add_subdirectory(${CMAKE_SOURCE_DIR}/../third-party/spdlog spdlog)

# nlohmann/json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${CMAKE_SOURCE_DIR}/../third-party/json json)

# 主库
add_library(pqy_hook SHARED
    src/hook/hook_manager.cpp
    src/hook/crc_bypass.cpp
    src/hook/process_bypass.cpp
    src/hook/debugger_bypass.cpp
    src/hook/freq_monitor.cpp
    src/hook/remote_cmd_bypass.cpp
    src/utils/logger.cpp
)

target_include_directories(pqy_hook PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(pqy_hook PRIVATE
    minhook
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    kernel32
    psapi
)

# 设置导入库输出目录
set_target_properties(pqy_hook PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib
)

# 代理 DLL
set(PROXY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy)
add_library(mmojo_proxy SHARED
    ${PROXY_DIR}/mmmojo_proxy.cpp
)
target_include_directories(mmojo_proxy PRIVATE
    ${PROXY_DIR}
)
target_link_libraries(mmojo_proxy PRIVATE
    spdlog::spdlog
)
set_target_properties(mmojo_proxy PROPERTIES
    OUTPUT_NAME "mmmojo_64"
    PREFIX ""
)

# 测试程序（使用 LoadLibrary 动态加载）
add_executable(hook_test tests/hook_test.cpp)
target_include_directories(hook_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
