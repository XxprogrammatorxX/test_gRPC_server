cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(remote_target_proto)

find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)

set(SOURCES
	remote_target.proto
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_BINARY_DIR}
		${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		protobuf::libprotobuf
		gRPC::grpc
		gRPC::grpc++
)


get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE cpp)
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
