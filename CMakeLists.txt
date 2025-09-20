cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
        SlaveTatsGUI
        VERSION 0.1.1
        DESCRIPTION "SlaveTats GUI."
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

file(
	GLOB_RECURSE
	headers_extern
	CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/catmenu/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/catmenu/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/slavetatsng/include/*.h"
)

file(
	GLOB_RECURSE
	sources_extern
	CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/slavetatsng/src/*.cpp"
)


set(sources ${sources_extern}
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc)

set(headers ${headers} ${headers_extern}
    include/JContainers/jc_interface.h
    include/JContainers/jcontainers_constants.h
    include/slavetatsng_wrapper.h
    include/tattoo_field_db.h
	include/UI.h
    include/slavetats_util.h
    include/widget_actor_selector.h
    include/widget_area_selector.h
    include/widget_slot_selector.h
    include/widget_field_selector.h
    include/window_field_editor.h
    include/window_inspector.h
    include/widget_int_editor.h
    include/widget_float_editor.h
    include/widget_string_editor.h
    include/widget_new_field_selector.h
    include/nioverride_utils.h
    include/widget_override_list.h
    include/widget_cached_tattoo_selector.h
    include/window_apply_remove.h
    include/imgui_utils.h
	src/PCH.h
)

set(sources ${sources}
    src/tattoo_field_db.cpp
	src/main.cpp
    src/slavetats_util.cpp
    src/widget_actor_selector.cpp
    src/widget_area_selector.cpp
    src/widget_slot_selector.cpp
    src/widget_field_selector.cpp
    src/window_field_editor.cpp
    src/window_inspector.cpp
    src/widget_int_editor.cpp
    src/widget_float_editor.cpp
    src/widget_string_editor.cpp
    src/widget_new_field_selector.cpp
    src/nioverride_utils.cpp
    src/widget_override_list.cpp
    src/widget_cached_tattoo_selector.cpp
    src/window_apply_remove.cpp
    src/imgui_utils.cpp
	src/UI.cpp
)

########################################################################################################################
## Configure target DLL
########################################################################################################################
find_package(CommonLibSSE CONFIG REQUIRED)
find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/utils.hpp")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-hash-library)
find_package(magic_enum CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${headers} ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/cmake>
        $<INSTALL_INTERFACE:src>)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/catmenu>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/slavetatsng/include>
		${CLIB_UTIL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        nlohmann_json::nlohmann_json
		unofficial::hash-library
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/catmenu/lib/imgui.lib
        )

target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        src/PCH.h)
