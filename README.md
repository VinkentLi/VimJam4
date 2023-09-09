create a build directory called "build"
then type this command to create a makefile in the build directory

(you dont need to do -G "MinGW Makefiles" if you are not using mingw)

cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DSDL2_DIR="just put the directory to the .cmake files in your sdl2 directory"

then once you created the makefile, use the makefile using make

in your commands, go to the root directory and do "./build/Paths"

then it will run
