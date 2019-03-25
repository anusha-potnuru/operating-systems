all: proc_2.cpp mmu.cpp master.cpp sched2.cpp
	g++ master.cpp -o master -lm
	g++ sched2.cpp -o scheduler -lm
	g++ mmu.cpp -o mmu -lm
	g++ proc_2.cpp -o process -lm

clean:
	rm process master scheduler mmu