cmake_minimum_required(VERSION 3.10)
project(SMRMS C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/main.c
    src/patient.c
)

add_executable(smrms ${SOURCES})

enable_testing()

add_executable(test_patient tests/test_patient.c src/patient.c)
target_include_directories(test_patient PRIVATE include)

add_test(NAME TestPatient COMMAND test_patient)

