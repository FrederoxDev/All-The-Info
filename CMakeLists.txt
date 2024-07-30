cmake_minimum_required(VERSION 3.12)
project(All-The-Info CXX ASM_NASM)
set(MOD_VERSION "1.0.0")

# Amethyst Minecraft Folder
set(AmethystFolder "$ENV{localappdata}/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/com.mojang/amethyst/") 

# Configure to build within the Amethyst Folder
configure_file(mod.json.in "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}/mod.json" @ONLY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${AmethystFolder}/mods/${PROJECT_NAME}@${MOD_VERSION}") 

# Compiler Options
set(CMAKE_CXX_STANDARD 23)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "Build configurations" FORCE)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: RelWithDebInfo" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await")
  
# Fetch ImGUI
include(FetchContent)
FetchContent_Declare( 
  imgui 
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90.9 
)
FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_SOURCES
    "${imgui_SOURCE_DIR}/*.cpp"
)

include(FetchContent)
FetchContent_Declare(
  implot
  GIT_REPOSITORY https://github.com/epezent/implot
  GIT_TAG        v0.16
)

FetchContent_MakeAvailable(implot)
 
file(GLOB IMPLOT_SOURCES 
    "${implot_SOURCE_DIR}/*.cpp"
)

# Make the Mod
file(GLOB_RECURSE ${PROJECT_NAME}_All
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)   

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_All})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    AmethystAPI 
    libhat
    $ENV{amethyst_src}/AmethystAPI/lib/fmt.lib
    imgui_impl
    implot_impl
) 

# Add a library for ImGui implementation
add_library(imgui_impl ${IMGUI_SOURCES})
add_library(implot_impl ${IMPLOT_SOURCES})

target_include_directories(imgui_impl PUBLIC
    ${imgui_SOURCE_DIR}
)

target_include_directories(implot_impl PUBLIC
    ${imgui_SOURCE_DIR}
    ${implot_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui_impl) 

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:/MP>
)

# EnTT Config Options
target_compile_definitions(${PROJECT_NAME} PUBLIC ENTT_PACKED_PAGE=128)

# Get the AmethystAPI Lib.
add_subdirectory("$ENV{amethyst_src}/AmethystAPI" "build")     
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${${PROJECT_NAME}_All})  