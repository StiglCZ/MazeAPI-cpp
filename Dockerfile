FROM alpine AS build
WORKDIR /code/
RUN apk add --no-cache libjpeg-turbo-dev make cmake gcc g++
COPY . /code/
RUN cmake -D CMAKE_BUILD_TYPE=Release -B ./build/Release -S .
RUN cmake --build ./build/Release --config Release
RUN mkdir -pv /code/install
RUN cmake --install ./build/Release --prefix /code/install --verbose

FROM alpine AS publish
RUN apk add --no-cache libjpeg-turbo libstdc++ libgcc
COPY --from=build /code/install /install
EXPOSE 8080

CMD [ "/install/maze_api_cpp" ]
