cmake_minimum_required(VERSION 3.31)
add_executable(maze_api_cpp)

# Add libjpeg
find_package(JPEG REQUIRED)
target_link_libraries(maze_api_cpp jpeg)

target_sources(maze_api_cpp
  PRIVATE
  mazejpg.cc
  maze.cc
  image.cc
)
