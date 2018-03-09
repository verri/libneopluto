cmake_minimum_required(VERSION 3.0)
project(LibNeoPluto VERSION 0.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(neopluto
  src/account.cpp
  src/database.cpp
  src/date.cpp
  src/entry.cpp
  src/exception.cpp
  src/naive_bayes.cpp
  src/tag.cpp
  src/tag_model.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

find_package(Boost REQUIRED COMPONENTS locale)

add_subdirectory(external/cool)
add_subdirectory(external/jules)

target_compile_features(neopluto PRIVATE cxx_std_17)

target_include_directories(neopluto PUBLIC include)
target_include_directories(neopluto PRIVATE ${SQLITE3_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

target_link_libraries(neopluto PRIVATE ${SQLITE3_LIBRARIES} ${Boost_LOCALE_LIBRARY} cool jules)

target_compile_options(neopluto PRIVATE "$<$<CONFIG:DEBUG>:-O0>")
target_compile_options(neopluto PRIVATE "$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>")
target_compile_options(neopluto PRIVATE "$<$<CONFIG:DEBUG>:-fsanitize=address,leak,undefined>")
set_target_properties(neopluto PROPERTIES LINK_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address,leak,undefined -fuse-ld=gold")
