add_library(VideoElement STATIC VideoElement.cpp)

target_include_directories(VideoElement PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
)