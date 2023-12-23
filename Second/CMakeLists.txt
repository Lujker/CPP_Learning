cmake_minimum_required(VERSION 3.20)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
set(PROJECT_NAME ip_filter)
set(PROJECT_NAME_CLI ip_filter_cli)
set(PROJECT_NAME_TEST test_ip_filter)

project(${PROJECT_NAME} VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in version.h)

add_executable(${PROJECT_NAME_CLI} ip_filter.cpp)
add_library(${PROJECT_NAME} lib.cpp)
add_executable(${PROJECT_NAME_TEST} test_ip_filter.cpp)

set_target_properties(${PROJECT_NAME_CLI} ${PROJECT_NAME} ${PROJECT_NAME_TEST} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(${PROJECT_NAME}
    PRIVATE "${CMAKE_BINARY_DIR}"
)
set_target_properties(${PROJECT_NAME_TEST} PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME_CLI} PRIVATE
    ${PROJECT_NAME}
)
target_link_libraries(${PROJECT_NAME_TEST}
    ${Boost_LIBRARIES}
    ${PROJECT_NAME}
)

if (MSVC)
    target_compile_options(${PROJECT_NAME_CLI} PRIVATE
        /W4
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
    )
    target_compile_options(${PROJECT_NAME_TEST} PRIVATE
        /W4
    )
else ()
    target_compile_options(${PROJECT_NAME_CLI} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(${PROJECT_NAME_TEST} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS ${PROJECT_NAME_CLI} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT vladzzp111@gmail.com)

include(CPack)

enable_testing()

add_test(${PROJECT_NAME_TEST} ${PROJECT_NAME_TEST})
