# Create the library with the correct name
add_library(VideoElement INTERFACE)

# Add include directories with INTERFACE keyword
target_include_directories(VideoElement 
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
