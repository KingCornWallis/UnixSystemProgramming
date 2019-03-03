all: master reverse cleanup upper

master: master.cpp message.h
	g++ -o master master.cpp -lrt -lpthread

reverse: reverse.cpp message.h
	g++ -o reverse reverse.cpp -lrt -lpthread

cleanup: cleanup.cpp
	g++ -o cleanup cleanup.cpp -lrt -lpthread

upper: upper.cpp message.h
	g++ -o upper upper.cpp -lrt -lpthread