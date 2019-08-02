cmake_minimum_required(VERSION 3.2)
project(hypergraph)

# Use a .in file to get a correct installation of yaml-cpp
configure_file(CMakeLists.yaml-cpp.txt.in ext/yaml-cpp/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ext/yaml-cpp" )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ext/yaml-cpp" )
add_subdirectory("${CMAKE_BINARY_DIR}/yaml-cpp-src"
                 "${CMAKE_BINARY_DIR}/yaml-cpp-build")

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
