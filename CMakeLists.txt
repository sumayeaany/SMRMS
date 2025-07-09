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
        include/report.h
        src/report.c
        include/emergency.h
        src/emergency.c
        src/auth.c
        include/auth.h
        include/prescription.h
        src/prescription.c
)

add_executable(smrms ${SOURCES})

# Copy only the executable to project root after building
add_custom_command(TARGET smrms POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:smrms> ${CMAKE_SOURCE_DIR}/
        COMMENT "Copying executable to project root"
)


