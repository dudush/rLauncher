all: rlauncher

rlauncher: run.o
	g++ -std=c++11 -o rlauncher rlauncher.out

rlauncher.out: run.cpp
	g++ -c run.cpp -std=c++11

clean:
	rm rlauncher.out rlauncher
