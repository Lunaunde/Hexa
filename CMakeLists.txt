# 需求的最低cmake版本
cmake_minimum_required(VERSION 3.12)

# 工程名
project(OpenGL_Lecturn)

# 支持的C++版本
set(CMAKE_CXX_STANDARD 17)

# 设置assets目录
file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

#加入全局预编译宏
add_definitions(-DDEBUG)

# 包含目录
include_directories(
	SYSTEM ${CMAKE_SOURCE_DIR}/thirdParty/include
)

link_directories(
	SYSTEM ${CMAKE_SOURCE_DIR}/thirdParty/lib
	${CMAKE_SOURCE_DIR}/thirdParty/lib/al
)

add_subdirectory(wrapper)
add_subdirectory(application)
add_subdirectory(glframework)
add_subdirectory(game)

# 本工程所有cpp文件编译链接
add_executable(Hexa "main.cpp" "glad.c" "thirdParty/include/stb_image.h")

target_link_libraries(Hexa glfw3.lib OpenAL32.lib wrapper application fw game)