cmake_minimum_required(VERSION 3.5.0)
project(timer VERSION 0.1.0 LANGUAGES C CXX)

# 引入cmdline
include_directories(library/cmdline)
# 引入simpleini
include_directories(library/simpleini)
aux_source_directory(library/simpleini LIBRARY_SIMPLEINI)
# 引入spdlog
include_directories(library/spdlog/include)
aux_source_directory(library/spdlog/src LIBRARY_SPDLOG)

include_directories(include)

set(SOURCE
  ${LIBRARY_SIMPLEINI}
  ${LIBRARY_SPDLOG}
  src/config.cpp
)

add_executable(timer main.cpp ${SOURCE})
