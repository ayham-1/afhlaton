cmake_minimum_required(VERSION 3.25.3 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# build type variable
if (CMAKE_BUILD_TYPE)
	string(TOLOWER ${CMAKE_BUILD_TYPE} BUILDTYPE_LOWER)
endif()

# afhlaton

message(STATUS "Processing afhlaton...")

project(
    afhlaton
    VERSION 0.0.0
    LANGUAGES C
)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()

file(GLOB_RECURSE SOURCES CONFIGUE_DEPENDS "src/*.c")

add_executable(
    afhlaton
    ${SOURCES}
)
target_compile_definitions(afhlaton PUBLIC WLR_USE_UNSTABLE)
target_compile_options(afhlaton PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(afhlaton PUBLIC ".")
target_link_options(afhlaton PRIVATE -rdynamic)

# Dependencies

message(STATUS "Processing dependencies...")

# wlroots fresh from the oven
message (STATUS "Baking wlroots...")

include(ExternalProject)

set(WLROOTS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/wlroots)
set(WLROOTS_BUILD_DIR ${CMAKE_BINARY_DIR}/wlroots)
set(WLROOTS_LIBRARY ${WLROOTS_BUILD_DIR}/libwlroots.so.13)
ExternalProject_Add(
	wlroots
	PREFIX ${WLROOTS_BUILD_DIR}
	SOURCE_DIR ${WLROOTS_SOURCE_DIR}
	CONFIGURE_COMMAND meson setup ${WLROOTS_BUILD_DIR} ${WLROOTS_SOURCE_DIR} --buildtype=${BUILDTYPE_LOWER} -Dwerror=false -Dexamples=false -Drenderers=gles2
	BUILD_COMMAND ninja -j8 -C ${WLROOTS_BUILD_DIR}
	BUILD_ALWAYS false
	BUILD_IN_SOURCE false
	BUILD_BYPRODUCTS ${WLROOTS_LIBRARY}
	INSTALL_COMMAND echo "wlroots: install not needed"
)
target_include_directories(afhlaton PRIVATE extern/wlroots/include/ ${WLROOTS_BUILD_DIR}/include/)
target_link_libraries(afhlaton PRIVATE ${WLROOTS_LIBRARY})
add_dependencies(afhlaton wlroots)

message (STATUS "Linking other dependencies...")

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET 
	wayland-server wayland-client wayland-cursor wayland-protocols 
	libdrm xkbcommon libinput pixman-1)

target_link_libraries(afhlaton PUBLIC PkgConfig::deps)

# protocols

message(STATUS "Processing Protocols...")

find_program(WaylandScanner NAMES wayland-scanner)
message(STATUS "Found WaylandScanner at ${WaylandScanner}")

execute_process(
	COMMAND pkg-config --variable=pkgdatadir wayland-protocols
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Found wayland protocols at ${WAYLAND_PROTOCOLS_DIR}")

set(PROTOCOL_OUTPUT gen-protocols/)
file(MAKE_DIRECTORY ${PROTOCOL_OUTPUT})
target_include_directories(afhlaton PRIVATE ${PROTOCOL_OUTPUT})

function(protocol protoPath protoName local)
	if (local)
		execute_process(
			COMMAND ${WaylandScanner} server-header ${protoPath} ${PROTOCOL_OUTPUT}/${protoName}-protocol.h
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
		execute_process(
			COMMAND ${WaylandScanner} private-code ${protoPath} ${PROTOCOL_OUTPUT}/${protoName}-protocol.c
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	else()
		execute_process(
			COMMAND ${WaylandScanner} server-header ${WAYLAND_PROTOCOLS_DIR}/${protoPath} ${PROTOCOL_OUTPUT}/${protoName}-protocol.h
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
		execute_process(
			COMMAND ${WaylandScanner} private-code ${WAYLAND_PROTOCOLS_DIR}/${protoPath} ${PROTOCOL_OUTPUT}/${protoName}-protocol.c
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	endif()

	target_sources(afhlaton PRIVATE ${PROTOCOL_OUTPUT}/${protoName}-protocol.c)
endfunction()

protocol("stable/xdg-shell/xdg-shell.xml" "xdg-shell" false)

install(TARGETS afhlaton)

