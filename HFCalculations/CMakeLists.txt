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

#Find ROOT install.
find_package (ROOT REQUIRED)
mark_as_advanced(FORCE GENREFLEX_EXECUTABLE ROOTCINT_EXECUTABLE ROOT_CONFIG_EXECUTABLE)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

# Source and Header files
include_directories(include)
include_directories(source)
include_directories(../include)
include_directories(../src)

file(GLOB sources ${CMAKE_SOURCE_DIR}/source/*.cpp)
file(GLOB headers ${CMAKE_SOURCE_DIR}/include/*.hpp)
file(STRINGS ./srcfiles.txt chi2sources)
file(STRINGS ./incfiles.txt chi2headers)

# add the executable
add_executable(hfreader reader.cpp ${sources} ${headers})
add_executable(chi2analyzer source/Chi2Analyzer.cc ${sources} ${chi2sources} ${headers} ${chi2headers})
add_executable(hfsummary source/HFSummary.cc ${sources} ${chi2sources} ${headers} ${chi2headers})
add_executable(doorway source/Doorway.cc ${sources} ${chi2sources} ${headers} ${chi2headers})
target_link_libraries(chi2analyzer ${ROOT_LIBRARIES})
target_link_libraries(hfsummary ${ROOT_LIBRARIES})
target_link_libraries(doorway ${ROOT_LIBRARIES})
install(TARGETS hfreader chi2analyzer hfsummary doorway DESTINATION bin)