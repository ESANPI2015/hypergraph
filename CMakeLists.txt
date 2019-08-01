cmake_minimum_required(VERSION 3.2)
project(hypergraph)
set(CMAKE_CXX_STANDARD 11)
include_directories(include)
add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PUBLIC include)
add_subdirectory(tools)
enable_testing()
add_subdirectory(test)

# pkg-config, to be installed:
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
# pkg-config for use inside our buildtree
configure_file(${PROJECT_NAME}-uninstalled.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-uninstalled.pc @ONLY)
