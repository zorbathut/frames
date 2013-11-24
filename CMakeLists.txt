cmake_minimum_required (VERSION 2.8.4)  # 2.8.4 because cmake changed its cygwin behavior in 2.8.4 and we rely on the new behavior

project (Frames CXX)

set(LIBRARY_OUTPUT_PATH lib)

include_directories("test")
include_directories("include")

if(MINGW)
  set(PLATFORM mingw)
  set(PLATFORMCXX mingw)
elseif(WIN32)
  set(PLATFORM win32)
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS) # Suppress all those annoying "insecure function" warnings on MSVC
  
  if(MSVC90)
    set(PLATFORMCXX win32_9)
  elseif(MSVC10)
    set(PLATFORMCXX win32_10)
  elseif(MSVC11)
    set(PLATFORMCXX win32_11)
  elseif(MSVC12)
    set(PLATFORMCXX win32_12)
  endif()
endif()

# TODO: make these optional
include_directories("deps/boost_1_54_0")
include_directories("deps/freetype-2.3.12/${PLATFORM}/include")
include_directories("deps/glew-1.10.0/${PLATFORM}/include")
include_directories("deps/jpeg-9/${PLATFORM}/include")
include_directories("deps/libpng-1.4.3/${PLATFORM}/include")
include_directories("deps/lua-5.1.5/${PLATFORM}/include")
include_directories("deps/SDL2-2.0.1/${PLATFORM}/include")
include_directories("deps/zlib-1.2.8/${PLATFORM}/include")

link_directories("deps/freetype-2.3.12/${PLATFORM}/lib")
link_directories("deps/glew-1.10.0/${PLATFORM}/lib")
link_directories("deps/jpeg-9/${PLATFORM}/lib")
link_directories("deps/libpng-1.4.3/${PLATFORM}/lib")
link_directories("deps/lua-5.1.5/${PLATFORM}/lib")
link_directories("deps/SDL2-2.0.1/${PLATFORM}/lib")
link_directories("deps/zlib-1.2.8/${PLATFORM}/lib")

# Used only for testing, thankfully
include_directories("deps/gtest-1.7.0/${PLATFORMCXX}/include")
link_directories("deps/gtest-1.7.0/${PLATFORMCXX}/lib")

file(GLOB_RECURSE HEADERS "include/*.h")

add_library(frames "src/color.cpp" "src/configuration.cpp" "src/environment.cpp" "src/event.cpp" "src/frame.cpp" "src/input.cpp" "src/layout.cpp" "src/loader.cpp" "src/lua.cpp" "src/mask.cpp" "src/os_win32.cpp" "src/raw.cpp" "src/renderer.cpp" "src/stream.cpp" "src/text.cpp" "src/text_manager.cpp" "src/texture.cpp" "src/texture_config.cpp" "src/texture_manager.cpp" "src/utility.cpp" ${HEADERS})
target_link_libraries(frames glew32s)
target_link_libraries(frames opengl32)
target_link_libraries(frames jpeg)

if(MINGW)
  target_link_libraries(frames png14)
  target_link_libraries(frames lua)
  target_link_libraries(frames freetype)
  target_link_libraries(frames z)
  target_link_libraries(frames pthread)
elseif(WIN32)
  target_link_libraries(frames libpng14)
  target_link_libraries(frames lua51)
  target_link_libraries(frames freetype2312)
  target_link_libraries(frames zlib)
endif()

add_executable(test "test/lib.cpp" "test/lib.h" "test/frame.cpp" "test/mask.cpp" "test/text.cpp")
target_link_libraries(test frames)
target_link_libraries(test SDL2)
target_link_libraries(test winmm)
target_link_libraries(test version)
target_link_libraries(test imm32)
target_link_libraries(test gtest)
target_link_libraries(test gtest_main)

# Compiler-specific stuff
if (MSVC11) # VS11 has spotty support for tr1::tuple and this breaks google test.
  set_property( TARGET test APPEND PROPERTY COMPILE_DEFINITIONS _VARIADIC_MAX=10 )
endif()
