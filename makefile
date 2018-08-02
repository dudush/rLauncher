all: rlauncher

rlauncher: run.o
	gcc -o rlauncher rlauncher.out -std=c++11

rlauncher.out: run.cpp
	gcc -c run.cpp -std=c++11

clean:
	rm rlauncher.out rlauncher
