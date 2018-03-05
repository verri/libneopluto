cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(neopluto
  src/database.cpp)

find_package(SQLite3 REQUIRED)

target_compile_features(neopluto PRIVATE cxx_std_11)

target_include_directories(neopluto PUBLIC include)
target_include_directories(neopluto PRIVATE ${SQLITE3_INCLUDE_DIR})

target_link_libraries(neopluto PRIVATE ${SQLITE3_LIBRARIES})
