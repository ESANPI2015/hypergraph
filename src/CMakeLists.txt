SET(ENV{PKG_CONFIG_PATH}
    ${CMAKE_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}
    )
# TODO: Use cmake to pull and build yaml-cpp
if(NOT TARGET yaml-cpp)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../ext/yaml-cpp yaml-cpp)
endif(NOT TARGET yaml-cpp)

find_package(PkgConfig)
pkg_check_modules(${PROJECT_NAME}_PKGCONFIG REQUIRED
    yaml-cpp
    )
include_directories(${${PROJECT_NAME}_PKGCONFIG_INCLUDE_DIRS})
#message(${hypergraph_PKGCONFIG_INCLUDE_DIRS})
link_directories(${${PROJECT_NAME}_PKGCONFIG_LIBRARY_DIRS})
add_definitions(${${PROJECT_NAME}hypergraph_PKGCONFIG_CFLAGS_OTHER} --pedantic -Wall -Werror)

set(SOURCES
    Hyperedge.cpp
    Hypergraph.cpp
    HypergraphYAML.cpp
    HypergraphDB.cpp
    Conceptgraph.cpp
    CommonConceptGraph.cpp
    )
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    ${${PROJECT_NAME}_PKGCONFIG_STATIC_LIBRARIES}
    )
