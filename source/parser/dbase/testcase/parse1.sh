g++ -c -O2 --std=c++1y -D__BYTE_ORDER=__LITTLE_ENDIAN -I/usr/include -I/usr/include/boost parser1.cc
gcc -o parser1 parser1.o -L/usr/local/lib64 -L/srv/Projekte/dbase/lib  -lstdc++
