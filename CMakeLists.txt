cmake_minimum_required(VERSION 3.10)
project(SMRMS C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
        src/main.c
        src/patient.c
        src/appointment.c
        include/appointment.h
        include/patient.h
        include/medicine.h
        src/medicine.c
)

add_executable(smrms ${SOURCES})

# Copy only the executable to project root after building
add_custom_command(TARGET smrms POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:smrms> ${CMAKE_SOURCE_DIR}/
        COMMENT "Copying executable to project root"
)

enable_testing()

add_executable(test_patient tests/test_patient.c src/patient.c)
target_include_directories(test_patient PRIVATE include)

add_test(NAME TestPatient COMMAND test_patient)

