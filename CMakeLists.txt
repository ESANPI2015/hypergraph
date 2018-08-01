cmake_minimum_required(VERSION 2.8)

project(hypergraph)

include_directories(src)
include_directories(include)

SET(ENV{PKG_CONFIG_PATH}
    ${CMAKE_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}
    )

if(NOT TARGET yaml-cpp)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ext/yaml-cpp yaml-cpp)
endif(NOT TARGET yaml-cpp)

find_package(PkgConfig)
pkg_check_modules(hypergraph_PKGCONFIG REQUIRED
    yaml-cpp
    libgit2
    )

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 11)
include_directories(${hypergraph_PKGCONFIG_INCLUDE_DIRS})
#message(${hypergraph_PKGCONFIG_INCLUDE_DIRS})
link_directories(${hypergraph_PKGCONFIG_LIBRARY_DIRS})
add_definitions(${hypergraph_PKGCONFIG_CFLAGS_OTHER} --pedantic -Wall -Werror)

set(SOURCES
    src/Hyperedge.cpp
    src/Hypergraph.cpp
    src/HypergraphYAML.cpp
    src/HypergraphDB.cpp
    src/Conceptgraph.cpp
    src/CommonConceptGraph.cpp
    )
set(HEADERS
    include/Hyperedge.hpp
    include/Hypergraph.hpp
    include/HypergraphYAML.hpp
    include/HypergraphDB.hpp
    include/Conceptgraph.hpp
    include/CommonConceptGraph.hpp
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

add_executable(testConceptgraph test/testConceptgraph.cpp)
target_link_libraries(testConceptgraph hypergraph)
install(TARGETS testConceptgraph
RUNTIME DESTINATION bin)

add_executable(testCommonConceptGraph test/testCommonConceptGraph.cpp)
target_link_libraries(testCommonConceptGraph hypergraph)
install(TARGETS testCommonConceptGraph
RUNTIME DESTINATION bin)

add_executable(testMap test/testMap.cpp)
target_link_libraries(testMap hypergraph)
install(TARGETS testMap
RUNTIME DESTINATION bin)

add_executable(simplify tools/simplify.cpp)
target_link_libraries(simplify hypergraph)
install(TARGETS simplify
RUNTIME DESTINATION bin)

add_executable(merge tools/merge.cpp)
target_link_libraries(merge hypergraph)
install(TARGETS merge
RUNTIME DESTINATION bin)

add_executable(query tools/query.cpp)
target_link_libraries(query hypergraph)
install(TARGETS query
RUNTIME DESTINATION bin)

add_executable(testHypergraphDB test/testHypergraphDB.cpp)
target_link_libraries(testHypergraphDB hypergraph)
install(TARGETS testHypergraphDB
RUNTIME DESTINATION bin)

# pkg-config, to be installed:
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
# pkg-config for use inside our buildtree
configure_file(${PROJECT_NAME}-uninstalled.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-uninstalled.pc @ONLY)
