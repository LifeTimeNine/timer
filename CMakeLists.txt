cmake_minimum_required(VERSION 3.5.0)
project(timer VERSION 0.1.0 LANGUAGES C CXX)

add_definitions(-DSPDLOG_COMPILED_LIB)

# 引入cmdline
include_directories(library/cmdline)
# 引入httplib
include_directories(library/httplib)
# 引入simpleini
include_directories(library/simpleini)
aux_source_directory(library/simpleini LIBRARY_SIMPLEINI)
# 引入spdlog
include_directories(library/spdlog/include)
aux_source_directory(library/spdlog/src LIBRARY_SPDLOG)
# 引入sqlite
include_directories(library/sqlite)
aux_source_directory(library/sqlite LIBRARY_SQLITE)

include_directories(include)
aux_source_directory(src SOURCE)

add_executable(timer main.cpp ${SOURCE} ${LIBRARY_SIMPLEINI} ${LIBRARY_SPDLOG} ${LIBRARY_SQLITE})
