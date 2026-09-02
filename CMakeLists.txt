cmake_minimum_required(VERSION 3.20)
project(
    Evitania
    VERSION 0.317.6
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(
    PROJECT_VERSION=\"${PROJECT_VERSION}\"
)
add_compile_options(-fpermissive)

include(FetchContent)

# Fetch MinHook
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        master
)
FetchContent_Populate(minhook)

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_Populate(imgui)

file(GLOB MINHOOK_SRC "${minhook_SOURCE_DIR}/src/*.c" "${minhook_SOURCE_DIR}/src/hde/*.c")
file(GLOB SRC_FILES "src/Cores/dll.cpp" "src/Modules/Hooks/*.cpp" "src/Modules/Menu/*.cpp" "src/Features/*/*.cpp")

add_library(Evitania SHARED
    ${SRC_FILES}
    ${MINHOOK_SRC}
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

# Build the injector separately
add_executable(injector src/Cores/injector.cpp)
set_target_properties(injector PROPERTIES SUFFIX ".exe")
target_link_options(injector PRIVATE "-static" "-static-libgcc" "-static-libstdc++")

target_include_directories(Evitania PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${minhook_SOURCE_DIR}/include
    src
    src/Modules
    src/Features
)

set_target_properties(Evitania PROPERTIES PREFIX "" SUFFIX ".dll")
target_link_options(Evitania PRIVATE "-static" "-static-libgcc" "-static-libstdc++")
target_link_libraries(Evitania PRIVATE d3d11 d3dcompiler dxgi gdi32 dwmapi)
