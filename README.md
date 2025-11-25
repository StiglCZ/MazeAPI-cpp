# Maze API

Rewritten the maze-api, now using jpeg and c++!

# Running
## Docker
 - `docker build .`
 - `docker run -it -d -p 8080:8080 <dockerimage-id>`
## Without docker(unix)
 - `RUN cmake -D CMAKE_BUILD_TYPE=Release -B ./build/Release -S .`
 - `./build/Release/maze-api-cpp`
 - Done
 
