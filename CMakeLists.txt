cmake_minimum_required(VERSION 3.25)

project(parser)

set(SOURCE_FILES 
ParserVariant.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})