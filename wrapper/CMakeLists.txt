#递归当前目录下所有cpp文件
file(GLOB_RECURSE WRAPPER ./ *.cpp)
#生成动态库
add_library(wrapper ${WRAPPER})