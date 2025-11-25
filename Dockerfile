FROM alpine AS base
WORKDIR /code
RUN apk update && apk add --no-cache libjpeg-turbo-dev make cmake gcc g++
COPY ./maze_api /code/maze_api

FROM base AS build
RUN cmake -D CMAKE_BUILD_TYPE=Release -B ./build/Release -S ./maze_api
RUN cmake --build ./build/Release --config Release
RUN mkdir -pv /code/install
RUN cmake --install ./build/Release --prefix /code/install --verbose
RUN ls -lRa /code/install

FROM base AS publish
COPY --from=build /code/install /install
EXPOSE 8080