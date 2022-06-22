# CMakeList.txt : CMake project for amxmodx-oo, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("oo_amxx")

# Add source to this project's executable.
add_library (oo_amxx SHARED "public/sdk/amxxmodule.cpp" "public/sdk/amxxmodule.h" "inc/memory_.h" "inc/assembly_create.h" "src/assembly_create.cpp" "src/oo_amxx.cpp" "src/oo_natives.cpp" "inc/oo_natives.h" "inc/oo_manager.h" "inc/oo_class.h" "inc/oo_object.h" "inc/oo_defs.h" "src/oo_manager.cpp" "inc/oo_utils.h" "src/oo_utils.cpp")

# TODO: Add tests and install targets if needed.
set_target_properties (oo_amxx
	PROPERTIES
		CXX_STANDARD 20
)

target_include_directories (oo_amxx
	PRIVATE
		${PROJECT_SOURCE_DIR}/public
		${PROJECT_SOURCE_DIR}/public/sdk
		${PROJECT_SOURCE_DIR}/public/memtools
		${PROJECT_SOURCE_DIR}/public/amtl
		${PROJECT_SOURCE_DIR}/inc
		${PROJECT_SOURCE_DIR}/src
)

target_compile_definitions (oo_amxx
	PRIVATE
		HAVE_STDINT_H
		NO_MSVC8_AUTO_COMPAT
		_CRT_SECURE_NO_WARNINGS
)

#target_compile_options(oo_amxx 
#    PRIVATE
#        "/wd4996;"
#)