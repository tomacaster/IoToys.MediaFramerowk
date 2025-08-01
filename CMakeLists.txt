cmake_minimum_required(VERSION 3.28)
project(IoToysMultimediaFramework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYLIBS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_BASE REQUIRED gstreamer-base-1.0)

add_subdirectory(common)
add_subdirectory(VideoElement)
add_subdirectory(VideoPlayer)
add_subdirectory(VideoPlayer/Adapters)
add_executable(IoToysMultimediaFramework
    main.cpp

)

target_compile_options(IoToysMultimediaFramework PRIVATE
    -fmodules-ts
    ${GSTREAMER_CFLAGS_OTHER}
    ${GSTREAMER_BASE_CFLAGS_OTHER}
)

target_link_libraries(IoToysMultimediaFramework PRIVATE
    VideoElement
    VideoPlayer
    Adapters
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER_BASE_LIBRARIES}
)

target_link_options(IoToysMultimediaFramework PRIVATE
    ${GSTREAMER_LDFLAGS_OTHER}
    ${GSTREAMER_BASE_LDFLAGS_OTHER}
)

target_include_directories(VideoElement PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
)
