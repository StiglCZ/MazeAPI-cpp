cmake_minimum_required(VERSION 3.31)
add_executable(maze_api_cpp)
project(maze_api)

# Add libjpeg
find_package(JPEG REQUIRED)
target_link_libraries(maze_api_cpp jpeg)

aux_source_directory(src SOURCES)

target_sources(maze_api_cpp PRIVATE ${SOURCES})

install(TARGETS maze_api_cpp DESTINATION .)
