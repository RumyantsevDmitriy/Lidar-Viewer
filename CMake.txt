cmake_minimum_required(VERSION 3.15.0)
project(run)

find_package(freeglut REQUIRED)
if (NOT FREEGLUT_FOUND)
    message(WARNING "Could not find freeglut, install it manually")
endif()

add_executable(${PROJECT_NAME} LidarViewer.cpp)
set(CMAKE_CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} PUBLIC
  freeglut
)