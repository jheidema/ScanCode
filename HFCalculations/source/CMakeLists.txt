
set(readerSources HFReader.cpp FileReader.cpp)
add_library(readerObj OBJECT ${readerSources})
add_library(readerSt STATIC $<TARGET_OBJECTS:readerObj>)

add_executable(hfreader reader.cpp)
target_link_libraries(hfreader readerSt)

install(TARGETS hfreader DESTINATION bin)