cmake_minimum_required (VERSION 2.8.4)  # 2.8.4 because cmake changed its cygwin behavior in 2.8.4 and we rely on the new behavior

project (Frames CXX)

set(LIBRARY_OUTPUT_PATH lib)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

include_directories("test")
include_directories("include")

add_definitions(-D_WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Someday we'll want to disable this, look for real problems, and then turn this right back on again.

# TODO: make these optional
include_directories("deps/boost_1_54_0")

if(MINGW)
  set(PLATFORM mingw)
elseif(WIN32)
  set(PLATFORM win32)
endif()

include_directories("deps/freetype-2.3.12/${PLATFORM}/include")
include_directories("deps/glew-1.10.0/${PLATFORM}/include")
include_directories("deps/jpeg-9/${PLATFORM}/include")
include_directories("deps/libpng-1.4.3/${PLATFORM}/include")
include_directories("deps/lua-5.1.5/${PLATFORM}/include")
include_directories("deps/SDL2-2.0.1/${PLATFORM}/include")
include_directories("deps/zlib-1.2.8/${PLATFORM}/include")
include_directories("deps/gtest-1.7.0/${PLATFORM}/include")

link_directories("deps/freetype-2.3.12/${PLATFORM}/lib")
link_directories("deps/glew-1.10.0/${PLATFORM}/lib")
link_directories("deps/jpeg-9/${PLATFORM}/lib")
link_directories("deps/libpng-1.4.3/${PLATFORM}/lib")
link_directories("deps/lua-5.1.5/${PLATFORM}/lib")
link_directories("deps/SDL2-2.0.1/${PLATFORM}/lib")
link_directories("deps/zlib-1.2.8/${PLATFORM}/lib")
link_directories("deps/gtest-1.7.0/${PLATFORM}/lib")


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
target_link_libraries(test debug gtestd)
target_link_libraries(test debug gtest_maind)
target_link_libraries(test optimized gtest)
target_link_libraries(test optimized gtest_main)
