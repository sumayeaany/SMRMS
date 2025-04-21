cmake_minimum_required(VERSION 3.10)
project(SMRMS C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/main.c
    src/patient.c
)

add_executable(smrms ${SOURCES})
