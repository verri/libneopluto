cmake_minimum_required(VERSION 3.0)
project(NeoPluto VERSION 0.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(neopluto
  src/account.cpp
  src/database.cpp
  src/exception.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_subdirectory(external/cool)

target_compile_features(neopluto PRIVATE cxx_std_11)

target_include_directories(neopluto PUBLIC include)
target_include_directories(neopluto PRIVATE ${SQLITE3_INCLUDE_DIR})

target_link_libraries(neopluto PRIVATE ${SQLITE3_LIBRARIES} cool)
