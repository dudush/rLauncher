all: rlauncher

rlauncher: run.o
	gcc -std=c++11 -o rlauncher rlauncher.out

rlauncher.out: run.cpp
	gcc -c run.cpp -std=c++11

clean:
	rm rlauncher.out rlauncher
