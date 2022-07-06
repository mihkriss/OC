files: m1 m2

m1: libd1.so prog1.cpp
	g++ prog1.cpp -L. -ld1 -o m1 -Wl,-rpath -Wl,.

m2: libd1.so libd2.so prog2.cpp
	g++ prog2.cpp -ldl -o m2 -Wl,-rpath -Wl,.

libd1.so: l1.o 
	g++ -shared l1.o -o libd1.so 

libd2.so: l2.o 
	g++ -shared l2.o -o libd2.so 

l1.o: libary1.cpp
	g++ -fPIC -c libary1.cpp -o l1.o

l2.o: libary2.cpp
	g++ -fPIC -c libary2.cpp -o l2.o
