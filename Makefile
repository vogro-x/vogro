vogro:
	g++ -std=c++11 main.cpp http_server.hpp base_server.hpp utils.hpp common.hpp request.hpp response.hpp status_code.hpp vogro_logger.hpp -lpthread -lboost_system -O3  -I/usr/include/boost -L/usr/lib -o vogro
clean:
	rm *.o vogro *.gch