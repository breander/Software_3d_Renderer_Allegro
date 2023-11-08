cmake_minimum_required(VERSION 3.5)

find_package(PkgConfig)
pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_color-5)

project(SOFTWARE_3D_RENDERER_ALLEGRO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(SOURCE_FILES main.cpp)
add_executable(SOFTWARE_3D_RENDERER_ALLEGRO ${SOURCE_FILES})

target_include_directories(SOFTWARE_3D_RENDERER_ALLEGRO PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
target_compile_options(SOFTWARE_3D_RENDERER_ALLEGRO PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
target_link_libraries(SOFTWARE_3D_RENDERER_ALLEGRO ${ALLEGRO5_LIBRARIES})
target_link_directories(SOFTWARE_3D_RENDERER_ALLEGRO PUBLIC ${ALLEGRO5_LIBRARY_DIRS})