cmake_minimum_required(VERSION 3.10)
project(myJson)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

set(CMAKE_CXX_STANDARD 17)

add_executable(myJson JsonTest.cpp src/Json.h src/Json.cpp src/JsonValue.h src/JsonValue.cpp src/JsonEnum.h
              src/JsonParser.h src/JsonParser.cpp src/JsonStringify.h src/JsonStringify.cpp
        )