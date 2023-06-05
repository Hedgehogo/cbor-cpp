cmake_minimum_required(VERSION 2.8)
project(cbor_cpp)

set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE src "lib/*.hpp" "lib/*.cpp")

add_library(${PROJECT_NAME} ${src})

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        )


file(GLOB_RECURSE test-src "tests/*.hpp" "tests/*.cpp")

add_executable(${PROJECT_NAME}_tests ${test-src})

target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME})


install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib/cbor DESTINATION ${CMAKE_INSTALL_PREFIX}/include PATTERN "*.hpp")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
        )

install(EXPORT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
        )
