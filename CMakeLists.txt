cmake_minimum_required(VERSION 3.5)
project(testing_app LANGUAGES CXX)

set(SDL2_DIR ${CMAKE_CURRENT_LIST_DIR}/SDL2/cmake)
set(SDL2_image_DIR ${CMAKE_CURRENT_LIST_DIR}/SDL2_image/cmake)
set(SDL2_ttf_DIR ${CMAKE_CURRENT_LIST_DIR}/SDL2_ttf/cmake)
set(SDL2_mixer_DIR ${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_image_INCLUDE_DIRS: ${SDL2_image_INCLUDE_DIRS}")
message(STATUS "SDL2_ttf_INCLUDE_DIRS: ${SDL2_ttf_INCLUDE_DIRS}")
message(STATUS "SDL2_mixer_INCLUDE_DIRS: ${SDL2_mixer_INCLUDE_DIRS}")

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_image_INCLUDE_DIRS} ${SDL2_ttf_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})

file(GLOB SOURCES include/*.cpp)

add_executable(tester ${SOURCES})

target_link_libraries(tester SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)

set_target_properties(tester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET tester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DIR}/../lib/SDL2.dll
        ${SDL2_image_DIR}/../lib/SDL2_image.dll
        ${SDL2_ttf_DIR}/../lib/SDL2_ttf.dll
        ${SDL2_mixer_DIR}/../lib/SDL2_mixer.dll
        $<TARGET_FILE_DIR:tester>
)