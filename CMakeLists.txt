cmake_minimum_required(VERSION 3.0)

add_library(neopluto
  src/database.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

target_compile_features(neopluto PRIVATE cxx_std_11)

target_include_directories(neopluto PUBLIC include)
target_include_directories(neopluto PRIVATE ${SQLITE3_INCLUDE_DIR})

target_link_libraries(neopluto PUBLIC ${SQLITE3_LIBRARIES})
