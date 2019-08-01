cmake_minimum_required(VERSION 3.2)

# TODO: Use cmake to get catch.hpp

add_executable(testHypergraph testHypergraph.cpp)
target_link_libraries(testHypergraph ${PROJECT_NAME})
install(TARGETS testHypergraph RUNTIME DESTINATION bin)
add_test(testHypergraph testHypergraph)
add_custom_command(TARGET testHypergraph POST_BUILD COMMAND testHypergraph)

add_executable(testConceptgraph testConceptgraph.cpp)
target_link_libraries(testConceptgraph ${PROJECT_NAME})
install(TARGETS testConceptgraph RUNTIME DESTINATION bin)
add_test(testConceptgraph testConceptgraph)
add_custom_command(TARGET testConceptgraph POST_BUILD COMMAND testConceptgraph)

add_executable(testCommonConceptGraph testCommonConceptGraph.cpp)
target_link_libraries(testCommonConceptGraph ${PROJECT_NAME})
install(TARGETS testCommonConceptGraph RUNTIME DESTINATION bin)
add_test(testCommonConceptGraph testCommonConceptGraph)
add_custom_command(TARGET testCommonConceptGraph POST_BUILD COMMAND testCommonConceptGraph)

add_executable(testMap testMap.cpp)
target_link_libraries(testMap ${PROJECT_NAME})
install(TARGETS testMap RUNTIME DESTINATION bin)
add_test(testMap testMap)
add_custom_command(TARGET testMap POST_BUILD COMMAND testMap)

add_executable(testHypergraphDB testHypergraphDB.cpp)
target_link_libraries(testHypergraphDB ${PROJECT_NAME})
install(TARGETS testHypergraphDB RUNTIME DESTINATION bin)
add_test(testHypergraphDB testHypergraphDB)
add_custom_command(TARGET testHypergraphDB POST_BUILD COMMAND testHypergraphDB)
