cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(remote_target_server)

set(SOURCES
	connection.h
	DeviceSpec.h
	main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		remote_target_proto
)
