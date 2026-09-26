cmake_minimum_required(VERSION 3.15)

project(Threading VERSION 1.0)

include("${CMAKE_CURRENT_SOURCE_DIR}/submodules/configuration/cmake/ProjectOptions.cmake")

option(THREAD_SANITIZER "Enable ThreadSanitizer" OFF)

setup_project()

if(COVERAGE AND THREAD_SANITIZER)
    message(FATAL_ERROR "COVERAGE and THREAD_SANITIZER cannot be enabled together.")
endif()

if(THREAD_SANITIZER)
    if(WIN32 OR NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        message(FATAL_ERROR "ThreadSanitizer requires GCC or Clang on Unix-like systems.")
    endif()
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread)
endif()

include(CheckTypeSize)
check_type_size("size_t" SIZEOF_SIZET)
message(STATUS "size_t size: ${SIZEOF_SIZET}")
check_type_size("long int" SIZEOF_LONG_INT)
message(STATUS "long int size: ${SIZEOF_LONG_INT}")
check_type_size("long long int" SIZEOF_LONG_LONG_INT)
message(STATUS "long long int size: ${SIZEOF_LONG_LONG_INT}")

find_package(Git)
if(GIT_FOUND)
    if (NOT EXISTS ${PROJECT_SOURCE_DIR}/.git) # Manages the case when an archive is used
        execute_process(COMMAND git init WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND rm -Rf ${PROJECT_SOURCE_DIR}/submodules/logging OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND git submodule add -f https://github.com/ariadne-cps/logging submodules/logging WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND rm -Rf ${PROJECT_SOURCE_DIR}/submodules/utility OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND git submodule add -f https://github.com/ariadne-cps/utility submodules/utility WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND rm -Rf ${PROJECT_SOURCE_DIR}/submodules/configuration OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND git submodule add -f https://github.com/ariadne-cps/configuration submodules/configuration WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
    endif()
    message(STATUS "Getting all submodules if not downloaded already...")
    execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/logging/CMakeLists.txt)
        message(ERROR "Logging download failure")
    else()
        message(STATUS "Logging submodule found")
    endif()
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/utility/CMakeLists.txt)
        message(ERROR "Utility download failure")
    else()
        message(STATUS "Utility submodule found")
    endif()
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/configuration/cmake)
        message(ERROR "Configuration download failure")
    else()
        message(STATUS "Configuration submodule found")
    endif()
else()
    message(FATAL_ERROR "Git not found, no submodules could be downloaded")
endif()


if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
    require_same_dependency_commit(Configuration
        REFERENCES
            "${PROJECT_SOURCE_DIR}|submodules/configuration"
            "${PROJECT_SOURCE_DIR}/submodules/logging|submodules/configuration"
            "${PROJECT_SOURCE_DIR}/submodules/utility|submodules/configuration"
    )
endif()

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/submodules/logging/include)
include_directories(${PROJECT_SOURCE_DIR}/submodules/utility/include)

if(NOT TARGET threading)
    add_subdirectory(src)

    add_library(threading ${LIBRARY_KIND} $<TARGET_OBJECTS:THREADING_SRC>)
    set_target_properties(threading PROPERTIES OUTPUT_NAME "ariadne-threading")

    add_subdirectory(submodules)
    target_link_libraries(threading logging utility Threads::Threads)

    setup_standalone_project_tests(
        TARGET threading
        TEST_DIRECTORY test
        TEST_TARGET tests
        SOURCES
            "${PROJECT_SOURCE_DIR}/include"
            "${PROJECT_SOURCE_DIR}/src"
    )

endif()
