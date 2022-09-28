# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.0)

# project name and languages
project(DDBTest C RC)

##############################################################################

# DDBTest.exe
add_executable(DDBTest WIN32 DDBTest.c DDBTest_res.rc)
target_link_libraries(DDBTest comctl32)

##############################################################################
