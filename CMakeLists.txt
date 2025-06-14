cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp "src/menu.cpp" "resource.h")
target_compile_features(main PRIVATE cxx_std_17)

target_compile_definitions(main PRIVATE SFML_STATIC)

target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio SFML::System)



if (WIN32)
    target_link_options(main PRIVATE "/SUBSYSTEM:WINDOWS")
endif()
