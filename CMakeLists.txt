cmake_minimum_required(VERSION 3.26)
project(ESFS)

set(CMAKE_CXX_STANDARD 14)

include_directories(dependencies/vendors/imgui)
include_directories(dependencies/vendors/imgui/backends)
include_directories(dependencies/vendors/imgui/misc/cpp)
include_directories(dependencies/vendors/imgui/misc/freetype)
include_directories(dependencies/vendors/imgui/misc/single_file)
include_directories(dependencies/vendors/raylib)
include_directories(dependencies/vendors/raylib/include)
include_directories(dependencies/vendors/raylib_imgui)
include_directories(dependencies/vendors/raylib/lib)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CONSOLE "")
ENDIF (CMAKE_BUILD_TYPE MATCHES "Debug")

IF(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CONSOLE "WIN32")
ENDIF (CMAKE_BUILD_TYPE MATCHES "Release")

add_executable(ESFS "${CONSOLE}"
        dependencies/vendors/imgui/backends/imgui_impl_opengl3.cpp
        dependencies/vendors/imgui/backends/imgui_impl_opengl3.h
        dependencies/vendors/imgui/backends/imgui_impl_opengl3_loader.h
        dependencies/vendors/imgui/misc/cpp/imgui_stdlib.cpp
        dependencies/vendors/imgui/misc/cpp/imgui_stdlib.h
        dependencies/vendors/imgui/imconfig.h
        dependencies/vendors/imgui/imgui.cpp
        dependencies/vendors/imgui/imgui.h
        dependencies/vendors/imgui/imgui_demo.cpp
        dependencies/vendors/imgui/imgui_draw.cpp
        dependencies/vendors/imgui/imgui_internal.h
        dependencies/vendors/imgui/imgui_tables.cpp
        dependencies/vendors/imgui/imgui_widgets.cpp
        dependencies/vendors/imgui/imstb_rectpack.h
        dependencies/vendors/imgui/imstb_textedit.h
        dependencies/vendors/imgui/imstb_truetype.h
        dependencies/vendors/raylib/include/raylib.h
        dependencies/vendors/raylib/include/raymath.h
        dependencies/vendors/raylib/include/rlgl.h
        dependencies/vendors/raylib_imgui/imgui_impl_raylib.h
        dependencies/vendors/raylib_imgui/rlImGui.cpp
        dependencies/vendors/raylib_imgui/rlImGui.h
        dependencies/vendors/raylib_imgui/rlImGuiColors.h
        src/main.cpp
        src/InputSystem.cpp
        src/InputSystem.h
        src/RenderSystem.cpp
        src/RenderSystem.h
        src/ChargeSystem.cpp
        src/ChargeSystem.h
        src/GridSystem.cpp
        src/GridSystem.h)


#set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++")
target_link_libraries(ESFS -static -lraylib -lopengl32 -lgdi32 -lwinmm)

target_include_directories(ESFS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/vendors/raylib/include")
target_link_directories(ESFS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/vendors/raylib/lib")
target_link_libraries(ESFS raylib)
