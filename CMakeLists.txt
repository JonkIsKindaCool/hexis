cmake_minimum_required(VERSION 3.23)

project(HaxScript)

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

add_executable(HaxScript ${SOURCE_FILES})
target_include_directories(HaxScript PRIVATE "include")