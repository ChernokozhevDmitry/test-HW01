cmake_minimum_required(VERSION 3.15)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(map_vec_allocator VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED TRUE)

option(BUILD_WITH_TESTS "Build with tests" OFF)

if(BUILD_WITH_TESTS)
    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )

    # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_executable(test_version test_version.cpp)
    target_include_directories(test_version PUBLIC ${CMAKE_SOURCE_DIR})
    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )    
    target_link_libraries(test_version PUBLIC gtest gtest_main)    
endif()


configure_file(version.h.in version.h)

add_executable(map_vec_allocator map_vec_allocator.cpp)

# set_target_properties(helloworld_cli helloworld test_version PROPERTIES
set_target_properties(map_vec_allocator PROPERTIES
CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
#target_include_directories(ip_filter
#    PRIVATE "${CMAKE_BINARY_DIR}"
#)

#target_link_libraries(helloworld_cli PRIVATE helloworld)

if (MSVC)
    target_compile_options(map_vec_allocator PRIVATE
        /W4
    )
    # target_compile_options(helloworld PRIVATE
    #     /W4
    # )
    if(BUILD_WITH_TESTS)
        target_compile_options(test_version PRIVATE
            /W4
        )
    endif()

else ()
    target_compile_options(map_vec_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    # target_compile_options(helloworld PRIVATE
    #     -Wall -Wextra -pedantic -Werror
    # )
    if(BUILD_WITH_TESTS)
        target_compile_options(test_version PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()



install(TARGETS map_vec_allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

if(BUILD_WITH_TESTS)
    enable_testing()
    add_test(test_version test_version)
endif()
