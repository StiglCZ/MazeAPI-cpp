FROM alpine AS base
WORKDIR /code
RUN apk update && apk add --no-cache libjpeg-turbo-dev make cmake gcc g++
COPY . /code/

FROM base AS build
RUN cmake -D CMAKE_BUILD_TYPE=Release -B ./build/Release -S .
RUN cmake --build ./build/Release --config Release
RUN mkdir -pv /code/install
RUN cmake --install ./build/Release --prefix /code/install --verbose

FROM base AS publish
COPY --from=build /code/install /install
EXPOSE 8080

CMD [ "/install/maze_api_cpp" ]