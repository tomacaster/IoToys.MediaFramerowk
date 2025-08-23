add_library(common INTERFACE)

# Dodaj podfolder enums
add_subdirectory(enums)
add_subdirectory(adapters)
add_subdirectory(EventBus)

target_include_directories(common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
