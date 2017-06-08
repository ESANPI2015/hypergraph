cmake_minimum_required(VERSION 2.8)

project(hypergraph)

include_directories(src)
include_directories(include)

SET(ENV{PKG_CONFIG_PATH}
    ${CMAKE_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}
    )

find_package(PkgConfig)
#pkg_check_modules(YAML REQUIRED yaml-0.1)
#include_directories(${YAML_INCLUDE_DIRS})
#link_directories(${YAML_LIBRARY_DIRS})
#add_definitions(-DYAML_SUPPORT ${YAML_CFLAGS_OTHER})
#set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} yaml)

pkg_check_modules(hypergraph_PKGCONFIG REQUIRED
    yaml-cpp
    )

include_directories(${hypergraph_PKGCONFIG_INCLUDE_DIRS})
#message(${hypergraph_PKGCONFIG_INCLUDE_DIRS})
link_directories(${hypergraph_PKGCONFIG_LIBRARY_DIRS})
add_definitions(${hypergraph_PKGCONFIG_CFLAGS_OTHER} -ggdb --pedantic -Wall -Werror -std=c++11)

set(SOURCES
    src/Hyperedge.cpp
    src/Hypergraph.cpp
    src/Conceptgraph.cpp
    #src/Relation.cpp
    #src/Concept.cpp
    )
set(HEADERS
    include/Hyperedge.hpp
    include/Hypergraph.hpp
    include/HyperedgeYAML.hpp
    include/Conceptgraph.hpp
    #include/Relation.hpp
    #include/Concept.hpp
    )

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# at first we have a minimal cmdline app, which exports simply everything into plaintext files
add_library(hypergraph STATIC ${SOURCES})
target_link_libraries(hypergraph
    ${hypergraph_PKGCONFIG_STATIC_LIBRARIES}
    )

add_executable(testHyperedge test/testHyperedge.cpp)
target_link_libraries(testHyperedge hypergraph)
install(TARGETS testHyperedge
RUNTIME DESTINATION bin)

#add_executable(testConceptgraph test/testConceptgraph.cpp)
#target_link_libraries(testConceptgraph hypergraph)
#install(TARGETS testConceptgraph
#RUNTIME DESTINATION bin)

# pkg-config, to be installed:
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
# pkg-config for use inside our buildtree
configure_file(${PROJECT_NAME}-uninstalled.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-uninstalled.pc @ONLY)
