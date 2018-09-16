vogro:
	g++ -std=c++11 main.cpp http.hpp base.hpp utils.hpp common.hpp request.hpp response.hpp status.hpp logger.hpp  -lpthread -lboost_system -O3  -I/usr/include/boost -L/usr/lib -o vogro
clean:
	rm *.o vogro *.gch