cmake_minimum_required(VERSION 3.20)
project(vstream_detection_example_cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++")


set(COMPILE_OPTIONS -Wall -Wextra -Wconversion -O3 -fconcepts -Wno-ignored-qualifiers -Wno-extra -Wno-stringop-truncation -Wno-reorder)

find_package(Threads)

find_package(HailoRT REQUIRED)

find_package (OpenCV 4.5.4 EXACT REQUIRED PATHS /usr/include/ )
#find_package(OpenCV REQUIRED)

message(STATUS "Found OpenCV: " ${OpenCV_INCLUDE_DIRS})

file(GLOB SOURCES
./*.cpp
./common/*.cpp
)

#link_libraries(stdc++fs)
add_executable(${PROJECT_NAME} ${SOURCES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME} Threads::Threads HailoRT::libhailort)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} stdc++)
