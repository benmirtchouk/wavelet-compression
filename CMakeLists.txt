cmake_minimum_required(VERSION 3.10)

project(subbandcoding)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(EasyBMP)

add_executable(subbandcoding main.cpp)
set_target_properties(
  subbandcoding
  PROPERTIES
    SUFFIX ".tsk"
)

target_sources(subbandcoding
  PRIVATE
    wavelet.cpp
    matrix.cpp
)

target_link_libraries(subbandcoding PUBLIC easybmplib)
target_include_directories(subbandcoding PUBLIC
  "${PROJECT_BINARY_DIR}"
  "${PROJECT_SOURCE_DIR}"
  "${PROJECT_SOURCE_DIR}/EasyBMP"
)