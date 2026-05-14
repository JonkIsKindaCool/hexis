cmake -B build
cmake --build build
cd build 
valgrind ./HaxScript
cd ../