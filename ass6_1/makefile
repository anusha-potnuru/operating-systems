all:
	g++ -c api.cpp
	ar -rcs libapi.a api.o
	g++ user.cpp -L. -lapi -o user
clear:
	rm user libapi.a api.o