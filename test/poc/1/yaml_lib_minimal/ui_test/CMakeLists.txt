cmake_minimum_required(VERSION 3.16)
project(yaml_lib_minimal/build_Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find Qt5
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Option 1: If yaml_lib_minimal/build is installed system-wide
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(SIMPLEYAMLUI yaml_lib_minimal/build)

# Option 2: If yaml_lib_minimal/build is in a subdirectory or yaml_lib_minimal/build directory
# Add the path where yaml_lib_minimal/build was built
set(SIMPLEYAMLUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../yaml_lib_minimal/build" CACHE PATH "Path to yaml_lib_minimal/build yaml_lib_minimal/build directory")
message (STATUS  "$SIMPLEUI_DIF")
# Find yaml-cpp (needed for linking)
find_package(yaml-cpp REQUIRED)

# Create the example executable
add_executable(example_app example_usage.cpp)


# Option 1: Link against installed library
# target_link_libraries(example_app
#     Qt5::Core
#     Qt5::Widgets
#     ${SIMPLEYAMLUI_LIBRARIES}
# )
# target_include_directories(example_app PRIVATE ${SIMPLEYAMLUI_INCLUDE_DIRS})

# Option 2: Link against library in yaml_lib_minimal/build directory
target_link_libraries(example_app
    Qt5::Core
    Qt5::Widgets
    ${SIMPLEYAMLUI_DIR}/libSimpleYamlUI.so  # or .a for static
    yaml-cpp
)

target_include_directories(example_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../yaml_lib_minimal/build  # For yaml_lib_minimal/build.h
)

# Copy the UI definition file to yaml_lib_minimal/build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ui.yaml
               ${CMAKE_CURRENT_BINARY_DIR}/ui.yaml
               COPYONLY)

# Set RPATH so the executable can find the library
set_target_properties(example_app PROPERTIES
    INSTALL_RPATH "${SIMPLEYAMLUI_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Install rules (optional)
install(TARGETS example_app
    RUNTIME DESTINATION bin
)

install(FILES ui.yaml
    DESTINATION share/yaml_lib_minimal/build/examples
)
