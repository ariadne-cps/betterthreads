cmake_minimum_required(VERSION 3.10)

project(Threading VERSION 1.0)

option(COVERAGE "Enable coverage reporting" OFF)
option(THREAD_SANITIZER "Enable ThreadSanitizer" OFF)

if(COVERAGE AND THREAD_SANITIZER)
    message(FATAL_ERROR "COVERAGE and THREAD_SANITIZER cannot be enabled together.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/configuration/cmake")
include(ProjectOptions)
include(CoverageSupport)
include(DependencyChecks)

enable_project_options()
enable_project_coverage()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.20)
        message(FATAL_ERROR "MSVC version must be at least 19.20!")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.2)
        message(FATAL_ERROR "GCC version must be at least 10.2!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(FATAL_ERROR "Clang version must be at least 11.0!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! MSVC, GCC and Clang are supported.")
endif()


if(THREAD_SANITIZER)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        message(FATAL_ERROR "ThreadSanitizer requires GCC or Clang.")
    endif()
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread)
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_KIND STATIC)
elseif(COVERAGE)
    set(LIBRARY_KIND STATIC)
else()
    set(LIBRARY_KIND SHARED)
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
        execute_process(COMMAND rm -Rf ${PROJECT_SOURCE_DIR}/submodules/helper OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND git submodule add -f https://github.com/ariadne-cps/helper submodules/helper WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
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
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/helper/CMakeLists.txt)
        message(ERROR "Helper download failure")
    else()
        message(STATUS "Helper submodule found")
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
            "${PROJECT_SOURCE_DIR}/submodules/helper|submodules/configuration"
    )
endif()

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/submodules/logging/include)
include_directories(${PROJECT_SOURCE_DIR}/submodules/helper/include)

if(NOT TARGET threading)
    add_subdirectory(src)

    add_library(threading ${LIBRARY_KIND} $<TARGET_OBJECTS:THREADING_SRC>)

    add_subdirectory(submodules)
    target_link_libraries(threading logging helper Threads::Threads)

    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        if(NOT TARGET tests)
            enable_testing()
            add_subdirectory(test)
        endif()

        if(COVERAGE)
            if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang")
                get_property(
                    THREADING_TEST_TARGETS
                    TARGET tests
                    PROPERTY MANUALLY_ADDED_DEPENDENCIES
                )

                setup_target_for_coverage_llvm(
                    NAME coverage
                    TARGET threading
                    OBJECTS
                        ${THREADING_TEST_TARGETS}
                    DEPENDENCIES tests
                    SOURCES
                        "${PROJECT_SOURCE_DIR}/include"
                        "${PROJECT_SOURCE_DIR}/src"
                )
            else()
                setup_target_for_coverage_gcc(
                    NAME coverage
                    DEPENDENCIES tests
                )
            endif()
        endif()
    endif()
endif()
