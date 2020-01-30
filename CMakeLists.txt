cmake_minimum_required(VERSION 2.8)

# set the project name
project(fillspectrum)

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

#Find required packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#Find ROOT install.
find_package (ROOT REQUIRED)
mark_as_advanced(FORCE GENREFLEX_EXECUTABLE ROOTCINT_EXECUTABLE ROOT_CONFIG_EXECUTABLE)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

# Source and Header files
include_directories(include)
include_directories(src)
include_directories(HFCalculations/include)
include_directories(HFCalculations/source)

#file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
#file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cc)
#file(GLOB headers ${CMAKE_SOURCE_DIR}/include/*.hpp)
#file(GLOB headers ${CMAKE_SOURCE_DIR}/include/*.h)
#file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
#file(GLOB headers ${CMAKE_SOURCE_DIR}/include/*.hpp)
file(GLOB HFsources ${CMAKE_SOURCE_DIR}/HFCalculations/source/*.cpp)
file(GLOB HFheaders ${CMAKE_SOURCE_DIR}/HFCalculations/include/*.hpp)

file(STRINGS src/files.txt sources)
file(STRINGS include/files.txt headers)

message( ${sources} )
message( ${headers} )
message( ${HFsources} )
message( ${HFheaders} )
message( ${ROOT_LIBRARIES} )

# add the executable
add_executable(fillspectrum FillSpectrum.cc ${sources} ${HFsources} ${headers} ${HFheaders})
target_link_libraries(fillspectrum ${ROOT_LIBRARIES})

install(TARGETS fillspectrum DESTINATION bin)