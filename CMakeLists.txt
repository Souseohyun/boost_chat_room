# 设置最低的CMake版本要求
cmake_minimum_required(VERSION 3.12)

# 设置项目名称
project(MychatServer)

# 查找 nlohmann_json 库
find_package(nlohmann_json 3.2.0 REQUIRED)

# 查找boost库   CMake提供了boost库的“包配置文件”
find_package(Boost REQUIRED COMPONENTS system thread headers)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# 查找MySQL客户端库
find_library(MYSQL_LIB
             NAMES mysqlclient
             PATHS /usr/lib /usr/lib/x86_64-linux-gnu)

# 查找MySQL客户端库的头文件路径
find_path(MYSQL_INCLUDE_DIR
          NAMES mysql.h
          PATHS /usr/include /usr/include/mysql)



# 查找MySQL Connector/C++库
find_library(MYSQLCPPCONN_LIB
             NAMES mysqlcppconn
             PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
# 查找MySQL Connector/C++头文件路径
find_path(MYSQLCPPCONN_INCLUDE_DIR
          NAMES cppconn/driver.h
          PATHS /usr/include /usr/include/mysql)

# 如果找不到MySQL库或头文件，输出错误信息
if(NOT MYSQL_LIB OR NOT MYSQL_INCLUDE_DIR)
    message(FATAL_ERROR "MySQL library or header files not found")
endif()
# 如果找不到MySQL Connector/C++库或头文件，输出错误信息
if(NOT MYSQLCPPCONN_LIB OR NOT MYSQLCPPCONN_INCLUDE_DIR)
    message(FATAL_ERROR "MySQL Connector/C++ library or header files not found")
endif()


file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/chatserversrc/*.cpp"
    "${PROJECT_SOURCE_DIR}/base/*.cpp"
    "${PROJECT_SOURCE_DIR}/utils/*.cpp"
    "${PROJECT_SOURCE_DIR}/mysqlapi/*.cpp"
    )

set(CMAKE_BUILD_TYPE Debug)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

## 链接 Boost 库
##if(Boost_FOUND)
##    include_directories(${Boost_INCLUDE_DIRS})
##    target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
##endif()


# 链接MySQL客户端库和MySQL Connector/C++库
include_directories(${MYSQL_INCLUDE_DIR} ${MYSQLCPPCONN_INCLUDE_DIR})
##target_link_libraries(${CMAKE_PROJECT_NAME} ${MYSQL_LIB} ${MYSQLCPPCONN_LIB})


# 链接所有库
target_link_libraries(${CMAKE_PROJECT_NAME} 
  PRIVATE 
  Boost::system 
  Boost::thread 
  Threads::Threads 
  OpenSSL::SSL 
  OpenSSL::Crypto
  nlohmann_json::nlohmann_json
  ${MYSQL_LIB}
  ${MYSQLCPPCONN_LIB}
)

####################################################################
# Image Server CMake

file(GLOB SRC_IMAGE_FILES
    "${PROJECT_SOURCE_DIR}/chatserversrc/ChatServer.cpp"
    "${PROJECT_SOURCE_DIR}/chatserversrc/TcpServer.cpp"
    "${PROJECT_SOURCE_DIR}/chatserversrc/ChatSession.cpp"
    "${PROJECT_SOURCE_DIR}/chatserversrc/TcpSession.cpp"
    "${PROJECT_SOURCE_DIR}/base/*.cpp"
    "${PROJECT_SOURCE_DIR}/utils/*.cpp"
    "${PROJECT_SOURCE_DIR}/mysqlapi/*.cpp"
    "${PROJECT_SOURCE_DIR}/imageserversrc/*.cpp"
    )

add_executable(ImageServer  ${SRC_IMAGE_FILES})


target_link_libraries(ImageServer
  PRIVATE 
  Boost::system 
  Boost::thread 
  Threads::Threads 
  OpenSSL::SSL 
  OpenSSL::Crypto
  nlohmann_json::nlohmann_json
  ${MYSQL_LIB}
  ${MYSQLCPPCONN_LIB}
)