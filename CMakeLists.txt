cmake_minimum_required(VERSION 3.18)
project(NESFDM LANGUAGES CXX VERSION 1.0.2)

set(CMAKE_CXX_STANDARD 17)
# 针对本机 CPU架构进行优化 (开启 AVX/AVX2 等指令集)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-march=native -ffast-math)
endif()
if(MSVC)
    add_compile_options(/arch:AVX2)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(NESFDMCore STATIC
    src/ModelParameters.cpp include/ModelParameters.h
    src/RungeKutta4.cpp include/RungeKutta4.h
    src/NESSolver.cpp include/NESSolver.h
    src/NESFDMUtils.cpp include/NESFDMUtils.h
    src/NESSweeper.cpp include/NESSweeper.h
)

target_include_directories(NESFDMCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test apps/app_test_sandbox.cpp)
add_executable(fdmnes apps/app_fdm_nes.cpp)
target_include_directories(fdmnes PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(test PRIVATE NESFDMCore)
target_link_libraries(fdmnes PRIVATE NESFDMCore) 