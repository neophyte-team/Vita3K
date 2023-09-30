include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# Make option() in all external projects do nothing when a normal variable of the
# same name exists. The normal variable is not removed. The cache entry is not
# created or updated and is ignored if it exists.
set(CMAKE_POLICY_DEFAULT_CMP0077 "NEW")

function(check_submodules_present)
	file(READ "${CMAKE_SOURCE_DIR}/.gitmodules" gitmodules)
	string(REGEX MATCHALL "path *= *[^ \t\r\n]*" gitmodules ${gitmodules})

	foreach(module ${gitmodules})
		# Get module name
		string(REGEX REPLACE "path *= *" "" module ${module})

		# Stat the folder and get amount of entries
		file(GLOB RESULT "${CMAKE_SOURCE_DIR}/${module}/*")
		list(LENGTH RESULT RES_LEN)

		# If the module has no files, bring fatal error
		if(RES_LEN EQUAL 0)
			# directory is empty
			message(FATAL_ERROR "Submodule ${module} is empty. "
				"Please run: git submodule update --init --recursive")
		endif()
	endforeach()
endfunction()

check_submodules_present()

# Define the Architecture variable, right now it should only contain "x86_64" or "arm64"
include("dynarmic/CMakeModules/DetectArchitecture.cmake")

option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(CAPSTONE_ARCHITECTURE_DEFAULT "Whether architectures are enabled by default" OFF)
option(CAPSTONE_ARM_SUPPORT "ARM support" ON)
add_subdirectory(capstone EXCLUDE_FROM_ALL)

add_library(crypto-algorithms STATIC crypto-algorithms/sha256.c crypto-algorithms/sha256.h)
target_include_directories(crypto-algorithms PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/crypto-algorithms")
set_property(TARGET crypto-algorithms PROPERTY FOLDER externals)

add_library(dirent INTERFACE)

if(WIN32)
	target_include_directories(dirent INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/dirent/include")
endif()

add_library(printf INTERFACE)
target_include_directories(printf INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/printf")

set(FMT_INSTALL ON)
add_subdirectory(fmt)
set(fmt_DIR fmt)
set_property(TARGET fmt PROPERTY FOLDER externals)

set(SPDLOG_WCHAR_FILENAMES ON CACHE BOOL "")
set(SPDLOG_NO_THREAD_ID ON CACHE BOOL "")
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "")
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
set_property(TARGET spdlog PROPERTY FOLDER externals)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")

add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
set_property(TARGET glad PROPERTY FOLDER externals)

add_library(better-enums INTERFACE)
target_include_directories(better-enums INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/better-enums")
target_compile_definitions(better-enums INTERFACE BETTER_ENUMS_STRICT_CONVERSION=1)

add_library(googletest STATIC googletest/googletest/src/gtest_main.cc googletest/googletest/src/gtest-all.cc)
target_include_directories(googletest PUBLIC googletest/googletest/include)
target_include_directories(googletest PRIVATE googletest/googletest)
target_compile_definitions(googletest PUBLIC GTEST_HAS_PTHREAD=0)
set_property(TARGET googletest PROPERTY FOLDER externals)

add_subdirectory(libfat16)
set_property(TARGET FAT16 PROPERTY FOLDER externals)

# The imgui target is including both imgui and imgui_club.
add_library(imgui STATIC imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/misc/cpp/imgui_stdlib.cpp)
target_compile_definitions(imgui PRIVATE IMGUI_DISABLE_DEMO_WINDOWS)
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui_club/imgui_memory_editor/")
set_property(TARGET imgui PROPERTY FOLDER externals)

add_library(miniz STATIC miniz/miniz.c miniz/miniz.h)
target_include_directories(miniz PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/miniz")
set_property(TARGET miniz PROPERTY FOLDER externals)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/sdl2-cmake-scripts")

if(APPLE)
	set(SDL2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sdl/macos" CACHE PATH "Where SDL2 is located" FORCE)
elseif(WIN32)
	set(SDL2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sdl/windows" CACHE PATH "Where SDL2 is located" FORCE)
	set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include" CACHE PATH "Where the SDL2 include files can be found" FORCE)
endif()

find_package(SDL2 REQUIRED)
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE "${SDL2_INCLUDE_DIR}")
target_link_libraries(sdl2 INTERFACE "${SDL2_LIBRARY}")

if(USE_QT_FRONTEND)
	message(STATUS "Qt frontend enabled")
endif()

#Qt setup, from https://github.com/yuzu-emu/yuzu/blob/master/CMakeLists.txt

if(USE_QT_FRONTEND AND MSVC)
	set(USE_BUNDLED_QT ON)
endif()

# SOLELY for the fact that I want everything to link and build, i'm just going to define all these here for now.
# todo: come back to this
option(USE_QT_MULTIMEDIA "Use QtMultimedia for Camera" ON)
option(USE_QT_WEB_ENGINE "Use QtWebEngine for web applet implementation" ON)
option(ENABLE_QT_TRANSLATION "Enable translations for the Qt frontend" OFF)

function(download_bundled_external remote_path lib_name prefix_var)

	set(package_base_url "https://github.com/yuzu-emu/")	# take from Yuzu for now since idk if we'll need any other libs?
	set(package_repo "no_platform")
	set(package_extension "no_platform")
	if (WIN32)
		set(package_repo "ext-windows-bin/raw/master/")
		set(package_extension ".7z")
	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set(package_repo "ext-linux-bin/raw/main/")
		set(package_extension ".tar.xz")
	elseif (ANDROID)
		set(package_repo "ext-android-bin/raw/main/")
		set(package_extension ".tar.xz")
	else()
		message(FATAL_ERROR "No package available for this platform")
	endif()
	set(package_url "${package_base_url}${package_repo}")

	set(prefix "${CMAKE_BINARY_DIR}/external/${lib_name}")
	if (NOT EXISTS "${prefix}")
		message(STATUS "Downloading binaries for ${lib_name}...")

		message(STATUS "Downloading " ${package_url}${remote_path}${lib_name}${package_extension} " to " ${CMAKE_BINARY_DIR}/external/${lib_name}${package_extension})

		file(DOWNLOAD
				${package_url}${remote_path}${lib_name}${package_extension}
				"${CMAKE_BINARY_DIR}/external/${lib_name}${package_extension}" SHOW_PROGRESS)

		#message(FATAL_ERROR ${PROJECT_SOURCE_DIR})
		#execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_BINARY_DIR}/external/${lib_name}${package_extension}"
		#directory "${PROJECT_SOURCE_DIR}/external")

		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_BINARY_DIR}/external/${lib_name}${package_extension}"
				WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/external")
	endif()
	message(STATUS "Using bundled binaries at ${prefix}")
	set(${prefix_var} "${prefix}" PARENT_SCOPE)
endfunction()

function(set_qt_components)
	# Best practice is to ask for all components at once, so they are from the same version
	set(QT_COMPONENTS Core Widgets Concurrent Gui PARENT_SCOPE)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		list(APPEND QT_COMPONENTS DBus)
	endif()
	if (USE_QT_MULTIMEDIA)
		list(APPEND QT_COMPONENTS Multimedia)
	endif()
	if (USE_QT_WEB_ENGINE)
		list(APPEND QT_COMPONENTS WebEngineCore WebEngineWidgets)
	endif()
	if (ENABLE_QT_TRANSLATION)
		list(APPEND QT_COMPONENTS LinguistTools)
	endif()
endfunction(set_qt_components)

# Qt5 requires that we find components, so it doesn't fit our pretty little find package function
if(USE_QT_FRONTEND)
	set(QT_VERSION 5.15)
	# These are used to specify minimum versions
	set(QT5_VERSION 5.15)
	set(QT_DEFAULT_MAJOR_VERSION 5)
	set(QT_MAJOR_VERSION 5)

	set(USE_QT_MULTIMEDIA ON)
	# Check for system Qt on Linux, fallback to bundled Qt
	if (UNIX AND NOT APPLE)
		if (NOT USE_BUNDLED_QT)
			find_package(Qt5 ${QT5_VERSION} COMPONENTS Widgets DBus Multimedia)
		endif()
		if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND (NOT Qt5_FOUND OR USE_BUNDLED_QT))
			# Check for dependencies, then enable bundled Qt download

			# Check that the system GLIBCXX version is compatible
			find_program(OBJDUMP objdump)
			if (NOT OBJDUMP)
				message(FATAL_ERROR "Required program `objdump` not found.")
			endif()
			find_library(LIBSTDCXX libstdc++.so.6)
			execute_process(
					COMMAND
					${OBJDUMP} -T ${LIBSTDCXX}
					COMMAND
					grep GLIBCXX_3.4.28
					COMMAND
					sed "s/[0-9a-f]*.*  //"
					COMMAND
					sed "s/ .*//"
					COMMAND
					sort -u
					OUTPUT_VARIABLE
					GLIBCXX_MET
			)
			if (NOT GLIBCXX_MET)
				message(FATAL_ERROR "Qt too old or not found, and bundled Qt package is not \
					compatible with this system. Either install Qt ${QT_VERSION}, or provide the path \
					to Qt by setting the variable Qt5_ROOT.")
			endif()

			# Check for headers
			find_package(PkgConfig REQUIRED)
			pkg_check_modules(QT_DEP_GLU QUIET glu>=9.0.0)
			if (NOT QT_DEP_GLU_FOUND)
				message(FATAL_ERROR "Qt bundled package dependency `glu` not found. \
					Perhaps `libglu1-mesa-dev` needs to be installed?")
			endif()
			pkg_check_modules(QT_DEP_MESA QUIET dri>=20.0.8)
			if (NOT QT_DEP_MESA_FOUND)
				message(FATAL_ERROR "Qt bundled package dependency `dri` not found. \
					Perhaps `mesa-common-dev` needs to be installed?")
			endif()

			# Check for X libraries
			set(BUNDLED_QT_REQUIREMENTS
					libxcb-icccm.so.4
					libxcb-image.so.0
					libxcb-keysyms.so.1
					libxcb-randr.so.0
					libxcb-render-util.so.0
					libxcb-render.so.0
					libxcb-shape.so.0
					libxcb-shm.so.0
					libxcb-sync.so.1
					libxcb-xfixes.so.0
					libxcb-xinerama.so.0
					libxcb-xkb.so.1
					libxcb.so.1
					libxkbcommon-x11.so.0
					libxkbcommon.so.0
			)
			set(UNRESOLVED_QT_DEPS "")
			foreach (REQUIREMENT ${BUNDLED_QT_REQUIREMENTS})
				find_library(BUNDLED_QT_${REQUIREMENT} ${REQUIREMENT})
				if (NOT BUNDLED_QT_${REQUIREMENT})
					set(UNRESOLVED_QT_DEPS ${UNRESOLVED_QT_DEPS} ${REQUIREMENT})
				endif()
				unset(BUNDLED_QT_${REQUIREMENT})
			endforeach()
			unset(BUNDLED_QT_REQUIREMENTS)

			if (NOT "${UNRESOLVED_QT_DEPS}" STREQUAL "")
				message(FATAL_ERROR "Bundled Qt package missing required dependencies: ${UNRESOLVED_QT_DEPS}")
			endif()

			set(USE_BUNDLED_QT ON CACHE BOOL "Download bundled Qt" FORCE)
		endif()
		if (USE_BUNDLED_QT)
			# Binary package currently does not support Qt webengine, so make sure it's disabled
			set(USE_QT_WEB_ENGINE OFF CACHE BOOL "Use Qt Webengine" FORCE)
		endif()
	endif()

	set(QT_NO_CMAKE_SYSTEM_PATH)

	if(USE_BUNDLED_QT)
		if ((MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1940) AND (ARCHITECTURE STREQUAL "x86_64"))
			set(QT_BUILD qt-5.15.2-msvc2019_64)
		elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux") AND NOT MINGW AND (ARCHITECTURE STREQUAL "x86_64"))
			set(QT_BUILD qt5_5_15_2)
		elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "MacOS")) # I think this is the only check we need for MacOS?
			set(QT_BUILD qt_error)
		else()
			message(FATAL_ERROR "No bundled Qt binaries for your toolchain. Disable USE_BUNDLED_QT and provide your own.")
		endif()

		if (DEFINED QT_BUILD)
			# i think i need to add a check here for if we already have the libs dl'd
			download_bundled_external("qt/" ${QT_BUILD} QT_PREFIX)
		endif()

		set(QT_PREFIX_HINT "${QT_PREFIX}" PARENT_SCOPE)

		set(QT_NO_CMAKE_SYSTEM_PATH "NO_CMAKE_SYSTEM_PATH")
		list(APPEND CMAKE_PREFIX_PATH "${QT_PREFIX}")
		# Binary package for Qt5 has Qt Multimedia
		set(USE_QT_MULTIMEDIA ON CACHE BOOL "Use Qt Multimedia" FORCE)
	endif()

	set_qt_components()
	find_package(Qt5 ${QT5_VERSION} COMPONENTS ${QT_COMPONENTS} ${QT_PREFIX_HINT} ${QT_NO_CMAKE_SYSTEM_PATH})
	include_directories(${Qt5_INCLUDE_DIR})
	message(${Qt5_FOUND})
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endif()

# Cubeb setup, from https://github.com/RPCS3/rpcs3/blob/master/3rdparty/cubeb/CMakeLists.txt
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Don't build shared libs")
set(BUILD_TESTS FALSE CACHE BOOL "Don't build tests")
set(BUILD_RUST_LIBS FALSE CACHE BOOL "Don't build rust libs")
set(BUILD_TOOLS FALSE CACHE BOOL "Don't build tools")
set(BUNDLE_SPEEX TRUE CACHE BOOL "Bundle the speex library")
set(LAZY_LOAD_LIBS TRUE CACHE BOOL "Lazily load shared libraries")
set(USE_SANITIZERS FALSE CACHE BOOL "Dont't use sanitizers")

add_subdirectory(cubeb EXCLUDE_FROM_ALL)
set_property(TARGET cubeb PROPERTY FOLDER externals)
set_property(TARGET speex PROPERTY FOLDER externals)

if (ARCHITECTURE STREQUAL "arm64")
	target_compile_definitions(speex PUBLIC
		#_USE_NEON
	)
elseif (ARCHITECTURE STREQUAL "x86_64")
	target_compile_definitions(speex PUBLIC
		_USE_SSE
		_USE_SSE2
	)
endif ()

if(WIN32)
	add_library(winsock INTERFACE)
	find_library(WSOCK32 wsock32)
	find_library(WS2_32 ws2_32)
	find_library(IPHLPAPI iphlpapi)
	target_link_libraries(winsock INTERFACE WSOCK32 WS2_32 IPHLPAPI)
endif()

set(DYNARMIC_TESTS OFF CACHE BOOL "")
set(DYNARMIC_NO_BUNDLED_FMT ON CACHE BOOL "")
set(DYNARMIC_FRONTENDS "A32" CACHE STRING "")
add_subdirectory(dynarmic)
set_property(TARGET dynarmic PROPERTY FOLDER externals)
set_property(TARGET mcl PROPERTY FOLDER externals)
if(ARCHITECTURE STREQUAL "x86_64")
	set_property(TARGET Zycore PROPERTY FOLDER externals)
	set_property(TARGET Zydis PROPERTY FOLDER externals)
endif()

if(MSVC)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		get_target_property(MCL_COMPILE_OPTIONS mcl COMPILE_OPTIONS)
		list(REMOVE_ITEM MCL_COMPILE_OPTIONS /std:c++latest)
		set_property(TARGET mcl PROPERTY COMPILE_OPTIONS ${MCL_COMPILE_OPTIONS})
	endif()
endif()

set(UNICORN_ARCH "arm")
add_subdirectory(unicorn EXCLUDE_FROM_ALL)
set_property(TARGET unicorn PROPERTY FOLDER externals)

add_library(vita-toolchain INTERFACE)
target_include_directories(vita-toolchain INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vita-toolchain/src")

option(YAML_CPP_BUILD_TOOLS "Enable parse tools" OFF)
option(YAML_CPP_BUILD_CONTRIB "Enable contrib stuff in library" OFF)
add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)
set_property(TARGET yaml-cpp PROPERTY FOLDER externals)

if(USE_DISCORD_RICH_PRESENCE)
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/external/discord_game_sdk.zip")
		message(STATUS "Downloading discord gamesdk...")
		file(DOWNLOAD https://dl-game-sdk.discordapp.net/latest/discord_game_sdk.zip
			"${CMAKE_BINARY_DIR}/external/discord_game_sdk.zip" SHOW_PROGRESS)
	endif()
endif()

if(USE_DISCORD_RICH_PRESENCE)
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/external/discord_game_sdk")
		file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/external/discord_game_sdk")
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_BINARY_DIR}/external/discord_game_sdk.zip"
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/external/discord_game_sdk")
		file(RENAME "${CMAKE_BINARY_DIR}/external/discord_game_sdk/lib/x86_64/discord_game_sdk.so"
			"${CMAKE_BINARY_DIR}/external/discord_game_sdk/lib/x86_64/libdiscord_game_sdk.so")
	endif()

	add_library(discord-rpc STATIC
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/achievement_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/activity_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/application_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/core.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/image_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/lobby_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/network_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/overlay_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/relationship_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/storage_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/store_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/types.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/user_manager.cpp
		${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp/voice_manager.cpp)

	if(APPLE)
		if(ARCHITECTURE STREQUAL "x86_64")
			target_link_libraries(discord-rpc PUBLIC "${CMAKE_BINARY_DIR}/external/discord_game_sdk/lib/x86_64/discord_game_sdk.dylib")
		else()
			target_link_libraries(discord-rpc PUBLIC "${CMAKE_BINARY_DIR}/external/discord_game_sdk/lib/aarch64/discord_game_sdk.dylib")
		endif()
	elseif(WIN32)
		target_link_libraries(discord-rpc PUBLIC "${CMAKE_BINARY_DIR}/external/discord_game_sdk/lib/x86_64/discord_game_sdk.dll.lib")
	elseif(UNIX)
		target_link_libraries(discord-rpc PUBLIC "${CMAKE_BINARY_DIR}/external/discord_game_sdk/lib/x86_64/libdiscord_game_sdk.so")
	endif()

	target_include_directories(discord-rpc PUBLIC "${CMAKE_BINARY_DIR}/external/discord_game_sdk/cpp")
	set_property(TARGET discord-rpc PROPERTY FOLDER externals)
endif()

option(BUILD_EXTERNAL "Build external dependencies in /External" OFF)
option(SKIP_GLSLANG_INSTALL "Skip installation" ON)
option(ENABLE_SPVREMAPPER "Enables building of SPVRemapper" OFF)
option(ENABLE_GLSLANG_BINARIES "Builds glslang and spirv-remap" OFF)
option(ENABLE_HLSL "Enables HLSL input support" OFF)
option(ENABLE_CTEST "Enables testing" OFF)
add_subdirectory(glslang)

option(SPIRV_CROSS_CLI "Build the CLI binary. Requires SPIRV_CROSS_STATIC." OFF)
option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)
option(SPIRV_CROSS_ENABLE_HLSL "Enable HLSL target support." OFF)
option(SPIRV_CROSS_ENABLE_MSL "Enable MSL target support." OFF)
option(SPIRV_CROSS_ENABLE_CPP "Enable C++ target support." OFF)
option(SPIRV_CROSS_ENABLE_REFLECT "Enable JSON reflection target support." OFF)
option(SPIRV_CROSS_ENABLE_C_API "Enable C API wrapper support in static library." OFF)
option(SPIRV_CROSS_ENABLE_UTIL "Enable util module support." OFF)
option(SPIRV_CROSS_SKIP_INSTALL "Skips installation targets." ON)
add_subdirectory(SPIRV-Cross)
set_property(TARGET spirv-cross-core PROPERTY FOLDER externals)
set_property(TARGET spirv-cross-glsl PROPERTY FOLDER externals)

add_library(dlmalloc STATIC "${CMAKE_CURRENT_SOURCE_DIR}/dlmalloc/dlmalloc.cc")
target_include_directories(dlmalloc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dlmalloc")
target_compile_definitions(dlmalloc PUBLIC ONLY_MSPACES=1)
target_compile_definitions(dlmalloc PUBLIC USE_LOCK=0)
set_property(TARGET dlmalloc PROPERTY FOLDER externals)

add_subdirectory(pugixml)
set_property(TARGET pugixml-static PROPERTY FOLDER externals)

add_library(CLI11 INTERFACE)
target_include_directories(CLI11 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/cli11")

add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers/include")
if(APPLE)
	target_include_directories(vulkan INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/MoltenVK")
endif()

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator-Hpp/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator-Hpp/VulkanMemoryAllocator/include")

add_subdirectory(ffmpeg)

add_subdirectory(psvpfstools)
set_property(TARGET psvpfsparser PROPERTY FOLDER externals)
set_property(TARGET libzRIF PROPERTY FOLDER externals)
set_property(TARGET libb64 PROPERTY FOLDER externals)

if(APPLE)
 	execute_process(
 		COMMAND brew --prefix openssl
 		RESULT_VARIABLE BREW_RESULT
 		OUTPUT_VARIABLE BREW_OPENSSL
 		OUTPUT_STRIP_TRAILING_WHITESPACE
 	)
 	if(BREW_RESULT EQUAL 0)
 		set(OPENSSL_ROOT_DIR "${BREW_OPENSSL}")
		set(OPENSSL_USE_STATIC_LIBS TRUE)
 	else()
		message(FATAL_ERROR "Cannot locate OpenSSL directory\nPlease run: brew install openssl")
	endif()
endif()
find_package(OpenSSL QUIET)

if(NOT OPENSSL_FOUND)
	if(MSVC)
		message("OpenSSL not found, using prebuilt version")

		if(NOT EXISTS "${CMAKE_BINARY_DIR}/external/openssl.zip")
			message(STATUS "Downloading openssl...")
			file(DOWNLOAD https://firedaemon.com/download-firedaemon-openssl-3.1-zip
				"${CMAKE_BINARY_DIR}/external/openssl.zip" SHOW_PROGRESS)
		endif()

		if(NOT EXISTS "${CMAKE_BINARY_DIR}/external/openssl")
			file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/external/openssl")
			execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_BINARY_DIR}/external/openssl.zip"
				WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/external/openssl")
		endif()

		set(OPENSSL_ROOT_DIR "${CMAKE_BINARY_DIR}/external/openssl/openssl-3/x64")
	endif()

	find_package(OpenSSL REQUIRED)
endif()

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

add_library(ssl INTERFACE)
target_link_libraries(ssl INTERFACE OpenSSL::SSL)

file(GLOB LIBATRAC9_SOURCES
	LibAtrac9/C/src/*.c
	LibAtrac9/C/src/*.h
)
add_library(libatrac9 STATIC ${LIBATRAC9_SOURCES})

target_include_directories(libatrac9 PUBLIC LibAtrac9/C/src)
set_property(TARGET libatrac9 PROPERTY FOLDER externals)

option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(XXHASH_BUILD_XXHSUM "Build the xxhsum binary" OFF)
add_subdirectory(xxHash/cmake_unofficial EXCLUDE_FROM_ALL)
set_property(TARGET xxhash PROPERTY FOLDER externals)

# Tracy
option(TRACY_ENABLE_ON_CORE_COMPONENTS
	"Enable and require Tracy to compile core components such as the renderer, shader recompiler and
		HLE modules"
	ON)
add_library(tracy STATIC tracy/public/TracyClient.cpp)
target_include_directories(tracy SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tracy/public>)
target_link_libraries(tracy PUBLIC ${CMAKE_DL_LIBS})
set_property(TARGET tracy PROPERTY FOLDER externals)

if(UNIX)
	target_link_libraries(tracy PUBLIC pthread)
endif()

# Enable Tracy on-demand profiling mode to avoid unnecessary memory usage when the developer
# isn't profiling by only enabling profiling after a connection has been established with
# a Tracy server
target_compile_definitions(tracy PUBLIC $<$<CONFIG:Debug,RelWithDebInfo>:TRACY_ON_DEMAND>)

# Disable Tracy automated data collection in order to prevent Tracy-related code from being profiled
target_compile_definitions(tracy PUBLIC TRACY_NO_SYSTEM_TRACING)

# Defining TRACY_ENABLE for both compiling Tracy and compiling the project that links
# against it is needed for Tracy to work
#
# -------------------------- Options (choose one at a time) ---------------------------
#
# A (DEFAULT). Enable Tracy on any target that links against if the build configuration
# is either Debug and RelWithDebInfo
#
# Using generator expresions instead of a conditional with CMAKE_BUILD_TYPE is needed
# in order for the condition to work properly on both single-config and multi-config
# CMake project generators. More info here:
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations
target_compile_definitions(tracy PUBLIC $<$<CONFIG:Debug,RelWithDebInfo>:TRACY_ENABLE>)

#
# ----------------------------------------------------------------------------------------
#
# B. Enable Tracy on every target that links to it regardless of the
# build configuration:
#
# target_compile_definitions(tracy PUBLIC TRACY_ENABLE)

# Use XDG desktop portals on Linux
set(NFD_PORTAL ON)

# nativefiledialog-extended
add_subdirectory(nativefiledialog-extended)

# Create alias to prevent the need of multiple changes in case the target name changes
add_library(NFDe::NFDe ALIAS nfd)
set_property(TARGET nfd PROPERTY FOLDER externals)
