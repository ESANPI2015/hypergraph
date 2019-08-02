add_definitions(--pedantic -Wall -Werror)
set(SOURCES
    Hyperedge.cpp
    Hypergraph.cpp
    HypergraphYAML.cpp
    HypergraphDB.cpp
    Conceptgraph.cpp
    CommonConceptGraph.cpp
    )
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} yaml-cpp)
