cmake_minimum_required(VERSION 3.20)

project(AudioProject)

add_executable(
    recorder

    ./recorder.cpp
)

target_include_directories(
    recorder
    PRIVATE
    include
)

target_link_libraries(
    recorder
    PRIVATE
    portaudio
)
