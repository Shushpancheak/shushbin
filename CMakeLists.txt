cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

project (
  "shushbin"
  VERSION 1.0.0
  DESCRIPTION ""
)
set (
  SOURCES
  src/pe_translator.cpp
)
set (
  HEADERS
  include/error/program_error_codes.hpp
  include/pe_translator/pe_translator.hpp
  include/pe_translator/pe_support.hpp
  include/pe_translator/instructions.hpp
  include/utils/arguments_parser.hpp
  include/utils/arithmetic.hpp
  include/utils/string.hpp
  include/utils/strings.hpp
  include/utils/vector.hpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} src/main.cpp)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# - - - - - - - - - - - - - GTEST INITIALIZATION- - - - - - - - - - - - - - - -
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

set(BUILD_TESTS OFF CACHE BOOL "Build tests")
if (BUILD_TESTS)
add_subdirectory(submodules/googletest)
include_directories(submodules/googletest/googletest/include)

enable_testing()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include_directories(${gtest_SOURCE_DIR})

set(UNIT_TESTS_NAME "run-tests-${PROJECT_NAME}")
set(UNIT_TESTS_FILE
  "test/test.cpp"
)

add_executable(${UNIT_TESTS_NAME} ${UNIT_TESTS_FILE} ${HEADERS} ${SOURCES})
add_test(${UNIT_TESTS_NAME} ${UNIT_TESTS_NAME})


target_link_libraries(${UNIT_TESTS_NAME} gtest_main)

# For next_ libraries.
set(BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)

endif() # BUILD_TESTS
