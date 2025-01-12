find_package(peff REQUIRED)

file(GLOB HEADERS *.h)
file(GLOB SRC *.cc)

add_library(wandxml SHARED)
target_compile_definitions(wandxml PRIVATE IS_WANDXML_BASE_BUILDING=1)
set_target_properties(wandxml PROPERTIES CXX_STANDARD 17)
target_sources(wandxml PRIVATE ${HEADERS} ${SRC})
target_include_directories(wandxml PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/> $<INSTALL_INTERFACE:include/>)
target_link_libraries(wandxml PUBLIC peff_base peff_utils peff_containers)
set_target_properties(wandxml PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    ARCHIVE FALSE
)

add_library(wandxml_static STATIC)
target_compile_definitions(wandxml_static PRIVATE WANDXML_STATIC_LINK=1 IS_WANDXML_BASE_BUILDING=1)
set_target_properties(wandxml_static PROPERTIES CXX_STANDARD 17)
target_sources(wandxml_static PRIVATE ${HEADERS} ${SRC})
target_link_libraries(wandxml_static PUBLIC peff_base_static peff_utils_static peff_containers_static)
target_include_directories(wandxml_static PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/> $<INSTALL_INTERFACE:include/>)
set_target_properties(wandxml_static PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
)

install(TARGETS wandxml wandxml_static
    EXPORT wandxmlTargets
    LIBRARY DESTINATION lib/
    ARCHIVE DESTINATION lib/
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/peff/base/
)
