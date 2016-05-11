rd /S /Q CMakeFiles
del CMakeCache.txt
del Makefile
del cmake_install.cmake

cmake -G "Unix Makefiles"
make

@pause