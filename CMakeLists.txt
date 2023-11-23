# 设置最低的CMake版本要求
cmake_minimum_required(VERSION 3.12)

# 设置项目名称
project(MychatServer)

# 查找boost库
find_package(Boost REQUIRED COMPONENTS system thread)

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/chatserversrc/*.cpp"
    "${PROJECT_SOURCE_DIR}/base/*.cpp"
    "${PROJECT_SOURCE_DIR}/base/*.hpp"
    "${PROJECT_SOURCE_DIR}/utils/*.cpp"
    "${PROJECT_SOURCE_DIR}/utils/*.hpp")

set(CMAKE_BUILD_TYPE Debug)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

# 链接 Boost 库
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
endif()

