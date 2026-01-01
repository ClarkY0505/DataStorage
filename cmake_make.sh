#!/bin/bash
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: The CMakeLists.txt file cannot be found in the current directory."
    echo "Make sure to run this script in the root directory of your CMake project."
    exit 1
fi

if [ ! -d "build" ]; then
    echo "The directory for building does not exist. About to be Creating the build directory"
    mkdir build
    if [ $? -ne 0 ]; then
        echo "Failed to create the "build" directory"
        exit 1
    fi
fi

echo "Enter the build directory"
cd build || { echo "Dont enter the build directory"; exit 1; }

echo "Runing CMake config ."
cmake .. || { echo "CMake configuration failed"; exit 1; }

echo "Compiling"
make || { echo "Compiling failed"; exit 1; }

cd ..

echo "Compiling successful"
echo "The executable file is located in the current directory."
