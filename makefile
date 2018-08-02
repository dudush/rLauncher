all: rlauncher

rlauncher: run.o
	gcc -std=c++11 -o rlauncher rlauncher.out

rlauncher.out: run.cpp
	gcc -std=c++11 -c run.cpp

clean:
	rm rlauncher.out rlauncher
