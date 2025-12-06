cmake_minimum_required(VERSION 3.29)

find_package(Vulkan REQUIRED)
project(app LANGUAGES C)

add_subdirectory("thirdParty/glfw")
add_subdirectory("thirdParty/cglm")

set(CMAKE_C_STANDARD 23)
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_executable(${PROJECT_NAME} ${MY_SOURCES} ${MY_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

if(APPLE)
    find_library(Cocoa Cocoa REQUIRED)
    find_library(IOKit IOKit REQUIRED)
    find_library(CoreVideo CoreVideo REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Cocoa} ${IOKit} ${CoreVideo})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan glfw cglm)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)
