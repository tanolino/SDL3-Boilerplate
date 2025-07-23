#!/usr/bin/bash

if command -v ninja >/dev/null 2>&1
then
	echo "Using Ninja to build"
	BUILDER=Ninja
elif command -v make >/dev/null 2>&1
then
	echo "Using Make to build"
	BUILDER="Unix Makefiles"
else
	echo "Missing either 'ninja' or 'make' command"
	exit 1
fi

if ! command -v cmake >/dev/null 2>&1
then
	echo "Missing CMake"
	exit 1
fi

if [[ "$1" == "release" ]]
then
	BUILD_TYPE=Release
else
	BUILD_TYPE=Debug
fi		
	
cmake -G "$BUILDER" \
	-D CMAKE_BUILD_TYPE=$BUILD_TYPE \
	-S . -B build_Linux &&
cmake --build build_Linux &&
cmake --install build_Linux --prefix install_Linux

if [[ "$1" != "release" ]]
then
	echo "copy compile_commands.json"
	cp build_Linux/compile_commands.json ./compile_commands.json
fi
