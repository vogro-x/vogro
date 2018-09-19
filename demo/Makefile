vogro:
	g++ -std=c++11 main.cpp ../src/server/http.hpp ../src/server/base.hpp ../src/utils.hpp ../src/common.hpp ../src/request.hpp ../src/response.hpp ../src/status.hpp ../src/logger.hpp -lssl -lcrypto -lpthread -lboost_system -O3  -I/usr/include/boost -I/usr/include/openssl -L/usr/lib -o vogro

clean:
	rm *.o vogro *.gch