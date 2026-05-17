cmake -B build
cmake --build build
cd build 
valgrind ./HexiScript
cd ../