find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)

add_library(VideoPlayer STATIC VideoPlayer.cpp)

target_include_directories(VideoPlayer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
    ${GSTREAMER_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
)

target_compile_options(VideoPlayer PRIVATE
    ${GSTREAMER_CFLAGS_OTHER}
    ${GLIB_CFLAGS_OTHER}
)

target_link_libraries(VideoPlayer PRIVATE
    ${GSTREAMER_LIBRARIES}
    ${GLIB_LIBRARIES}
)