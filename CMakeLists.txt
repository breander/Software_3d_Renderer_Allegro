cmake_minimum_required(VERSION 3.5)

project(SOFTWARE_3D_RENDERER_ALLEGRO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(SOURCE_FILES main.cpp)
add_executable(SOFTWARE_3D_RENDERER_ALLEGRO ${SOURCE_FILES})

INCLUDE_DIRECTORIES(  /opt/homebrew/Cellar/allegro/5.2.8.0_2/include )
LINK_DIRECTORIES(  /opt/homebrew/Cellar/allegro/5.2.8.0_2/lib )

file(GLOB LIBRARIES "/opt/homebrew/Cellar/allegro/5.2.8.0_2/lib/*.dylib")
message("LIBRARIES = ${LIBRARIES}")

TARGET_LINK_LIBRARIES(SOFTWARE_3D_RENDERER_ALLEGRO  ${LIBRARIES})