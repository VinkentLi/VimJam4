### Paths
## I couldn't think of a good name lol
# the code sucks as well lol

## Anyways heres how to build it
# make sure you have cmake

create a build directory called "build"
then type this command to create a makefile in the build directory

(you dont need to do -G "MinGW Makefiles" if you are not using mingw)

cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DSDL2_DIR="path/to/sdl2/cmake/files" -DSDL2_image_DIR="path/to/sdl2_image/cmake/files"

then once you created the makefile, use the makefile using make

in your commands, go to the project directory and do "./build/Paths"

then it will run
