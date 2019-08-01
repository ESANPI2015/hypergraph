cmake_minimum_required(VERSION 3.2)

add_executable(merge merge.cpp)
target_link_libraries(merge ${PROJECT_NAME})
install(TARGETS merge RUNTIME DESTINATION bin)

add_executable(query query.cpp)
target_link_libraries(query ${PROJECT_NAME})
install(TARGETS query RUNTIME DESTINATION bin)
