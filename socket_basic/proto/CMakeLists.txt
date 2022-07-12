cmake_minimum_required(VERSION 2.8)

#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_FLAGS  "-g -fexec-charset=GBK -std=c++11")
find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS helloworld.proto yy.proto)
add_library(helloworld ${PROTO_SRCS} )
#add_executable(main ${SRC_LIST} ${PROTO_SRCS} ${PROTO_HDRS})
#target_link_libraries(main ${Protobuf_LIBRARIES})
