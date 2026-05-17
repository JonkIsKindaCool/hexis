cmake_minimum_required(VERSION 3.23)

project(HexiScript)

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

add_executable(HexiScript ${SOURCE_FILES})
target_include_directories(HexiScript PRIVATE "include")