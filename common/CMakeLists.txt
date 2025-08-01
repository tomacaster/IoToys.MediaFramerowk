add_library(common INTERFACE)

# Dodaj podfolder enums
add_subdirectory(enums)

target_include_directories(common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
