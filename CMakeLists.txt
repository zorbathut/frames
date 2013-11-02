cmake_minimum_required (VERSION 2.8.4)  # 2.8.4 because cmake changed its cygwin behavior in 2.8.4 and we rely on the new behavior

project (Frame CXX)
set(LIBRARY_OUTPUT_PATH lib)

include_directories("test")
include_directories("include")

add_definitions(-D_WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Someday we'll want to disable this, look for real problems, and then turn this right back on again.

# TODO: make these optional
include_directories("extern/boost_1_54_0")
include_directories("extern/freetype-2.3.12/win32/include")
include_directories("extern/glew-1.10.0/include")
include_directories("extern/jpeg-9/win32/include")
include_directories("extern/libpng-1.4.3/win32/include")
include_directories("extern/lua-5.1.5/win32/include")
include_directories("extern/lua-5.1.5/etc") # needed for lua51.hpp
include_directories("extern/SDL2-2.0.1/win32/include")
include_directories("extern/zlib-1.2.8/win32/include")

link_directories("extern/freetype-2.3.12/win32/lib")
link_directories("extern/glew-1.10.0/lib/Release/Win32")
link_directories("extern/jpeg-9/win32/lib")
link_directories("extern/libpng-1.4.3/win32/lib")
link_directories("extern/lua-5.1.5/win32/lib")
link_directories("extern/SDL2-2.0.1/win32/lib")
link_directories("extern/zlib-1.2.8/win32/lib")


file(GLOB_RECURSE HEADERS "include/*.h")

add_library(frame "src/color.cpp" "src/configuration.cpp" "src/environment.cpp" "src/event.cpp" "src/frame.cpp" "src/input.cpp" "src/layout.cpp" "src/loader.cpp" "src/lua.cpp" "src/mask.cpp" "src/os_win32.cpp" "src/raw.cpp" "src/renderer.cpp" "src/stream.cpp" "src/text.cpp" "src/text_manager.cpp" "src/texture.cpp" "src/texture_config.cpp" "src/texture_manager.cpp" "src/utility.cpp" ${HEADERS})
target_link_libraries(frame glew32s)
target_link_libraries(frame opengl32)

add_executable(sandbox WIN32 "test/sandbox.cpp")
target_link_libraries(sandbox frame)
target_link_libraries(sandbox SDL2)
target_link_libraries(sandbox SDL2main)
target_link_libraries(sandbox winmm)
target_link_libraries(sandbox version)
target_link_libraries(sandbox imm32)
target_link_libraries(sandbox libpng14)
target_link_libraries(sandbox jpeg)
target_link_libraries(sandbox lua51)
target_link_libraries(sandbox glew32s)
target_link_libraries(sandbox freetype2312)
target_link_libraries(sandbox zlib)

