#https://dominikberner.ch/cmake-interface-lib/
cmake_minimum_required(VERSION 3.17)

project(sdl2raii
  DESCRIPTION "A header-only library that wraps libSDL type for type, \
function for function, to use RAII and modern C++isms. \
It is also emscripten-compatible."
 LANGUAGES CXX)

add_library(sdl2raii INTERFACE)
target_include_directories(sdl2raii
  INTERFACE
  $<BULD_INTERFACE:${sdl2raii_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(sdl2raii INTERFACE cxx_std_17)

# https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/
find_package(SDL2 REQUIRED)
target_link_libraries(sdl2raii INTERFACE SDL2::SDL2)
find_package(Boost CONFIG REQUIRED COMPONENTS headers)
target_link_libraries(sdl2raii INTERFACE Boost::headers)

include(GNUInstallDirs)
install(
  TARGETS sdl2raii
  EXPORT sdl2raii_Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT sdl2raii_Targets
  FILE sdl2raii_Targets.cmake
  NAMESPACE sdl2raii::
  DESTINATION lib/cmake/sdl2raii)
