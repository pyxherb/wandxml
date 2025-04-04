cmake_minimum_required(VERSION 3.13)
project(wandxml VERSION 0.1.0)

add_subdirectory("wandxml")
add_subdirectory("example")

# Generate the version file for the config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/wandxmlConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
)

# Create config file
configure_package_config_file(
    cmake/wandxmlConfig.cmake.in wandxmlConfig.cmake
    INSTALL_DESTINATION lib/cmake/wandxml
)

# Install config files
install(
    FILES   
       ${CMAKE_CURRENT_BINARY_DIR}/wandxmlConfig.cmake
       ${CMAKE_CURRENT_BINARY_DIR}/wandxmlConfigVersion.cmake
    DESTINATION
        lib/cmake/${PROJECT_NAME}
)

# Exporting Targets from the Build Tree
install(EXPORT wandxmlTargets
    FILE wandxmlTargets.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)
