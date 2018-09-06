common.o:
	g++ -c common.hpp -std=c++11 -o common.o

code.o:
	g++ -c status_code.hpp -std=c++11 -o code.o

request.o:
	g++ -c request.hpp -std=c++11 -o request.o

response.o:
	g++ -c response.hpp -std=c++11 -o response.o

baseServer.o: 
	g++ -c base_server.hpp -std=c++11 -o baseServer.o -lpthread -lboost_system -O3  -I/usr/include/boost -L/usr/lib

all: common.o code.o request.o response.o baseServer.o
	g++ -std=c++11 main.cpp http_server.hpp common.o code.o response.o request.o baseServer.o  -lpthread -lboost_system -O3  -I/usr/include/boost -L/usr/lib -o vogro

vogro:
	g++ -std=c++11 main.cpp http_server.hpp base_server.hpp utils.hpp common.hpp request.hpp response.hpp status_code.hpp vogro_logger.hpp -lpthread -lboost_system -O3  -I/usr/include/boost -L/usr/lib -o vogro
clean:
	rm *.o vogro *.gch