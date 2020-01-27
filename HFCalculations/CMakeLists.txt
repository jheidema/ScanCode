cmake_minimum_required(VERSION 2.8)

# set the project name
project(hfreader)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Release CACHE STRING
"Build type, options are: None Debug Release RelWithDebInfo MinSizeRel."
FORCE)
message(STATUS "Build type not defined, using default: ${CMAKE_BUILD_TYPE}")

file(MAKE_DIRECTORY install)
get_filename_component(INSTALL_DIR ${CMAKE_SOURCE_DIR}/install REALPATH)
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR} CACHE PATH "Install Prefix" FORCE)
#set(CMAKE_INSTALL_PREFIX /home/jheidema/In134/HFCalculations/install)


# Source and Header files
include_directories(include)
include_directories(source)

file(GLOB sources ${CMAKE_SOURCE_DIR}/source/*.cpp)
file(GLOB headers ${CMAKE_SOURCE_DIR}/include/*.hpp)


# add the executable
add_executable(hfreader reader.cpp ${sources} ${headers})
install(TARGETS hfreader DESTINATION bin)