#递归当前目录下所有cpp文件
file(GLOB_RECURSE FW ./ *.cpp)
#生成动态库
add_library(fw ${FW})