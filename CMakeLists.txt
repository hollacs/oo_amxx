cmake_minimum_required (VERSION 3.8)

project ("oo_amxx")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library (oo_amxx SHARED 
	"public/sdk/amxxmodule.cpp" 
	"public/sdk/amxxmodule.h" 
	"inc/moduleconfig.h"
	"src/oo_amxx.cpp" 
	"src/oo_natives.cpp" 
	"inc/oo_natives.h" 
	"inc/oo_manager.h" 
	"inc/oo_class.h" 
	"inc/oo_object.h" 
	"inc/oo_defs.h" 
	"src/oo_manager.cpp" 
	"inc/oo_utils.h" 
	"src/oo_utils.cpp"
)

set_target_properties(oo_amxx PROPERTIES OUTPUT_NAME "oo")

target_include_directories(oo_amxx PRIVATE
	${PROJECT_SOURCE_DIR}/public
	${PROJECT_SOURCE_DIR}/public/sdk
	${PROJECT_SOURCE_DIR}/inc
	${PROJECT_SOURCE_DIR}/src
)

target_compile_definitions(oo_amxx PRIVATE
	HAVE_STDINT_H
	PAWN_CELL_SIZE=32
	_CRT_SECURE_NO_WARNINGS
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	#  -pipe -fno-strict-aliasing -fvisibility=hidden -Wall -Werror -Wno-parentheses -Wno-uninitialized -Wno-unused -Wno-switch
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	target_link_options(oo_amxx PRIVATE "-m32")

	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		target_compile_definitions(oo_amxx PRIVATE DEBUG _DEBUG)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -O0")
	else ()
		target_compile_definitions(oo_amxx PRIVATE NDEBUG)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
	endif ()

	if (UNIX)
		target_compile_definitions(oo_amxx PRIVATE _LINUX POSIX LINUX)
		target_link_libraries(oo_amxx)
	endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	if (CMAKE_BUILD_TYPE MATCHES ".*Debug")
		target_compile_definitions(oo_amxx PRIVATE DEBUG _DEBUG)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MTd /Od /RTC1")
	else ()
		target_compile_definitions(oo_amxx PRIVATE NDEBUG)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT /Ox")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:ICF /OPT:REF")
	endif ()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Oy-")
	target_compile_definitions(oo_amxx PRIVATE WIN32 _WINDOWS)
endif ()

if (UNIX)
	set_target_properties(oo_amxx PROPERTIES PREFIX "")
	set_target_properties(oo_amxx PROPERTIES SUFFIX "_amxx_i386.so")
elseif (WIN32)
	set_target_properties(oo_amxx PROPERTIES PREFIX "")
	set_target_properties(oo_amxx PROPERTIES SUFFIX "_amxx.dll")
endif ()