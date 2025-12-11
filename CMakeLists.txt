cmake_minimum_required(VERSION 3.18)
project(NESFDM LANGUAGES CXX VERSION 1.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(NESFDMCore STATIC
    src/ModelParameters.cpp include/ModelParameters.h
    src/RungeKutta4.cpp include/RungeKutta4.h
    src/NESSolver.cpp include/NESSolver.h
    src/NESFDMUtils.cpp include/NESFDMUtils.h
)

target_include_directories(NESFDMCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test apps/app_test_sandbox.cpp)
add_executable(DoubleNesScan apps/app_double_scan.cpp)
target_include_directories(DoubleNesScan PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(test PRIVATE NESFDMCore)
target_link_libraries(DoubleNesScan PRIVATE NESFDMCore)