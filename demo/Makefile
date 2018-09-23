vogro:
	g++ -std=c++11 main.cpp ../src/server/http.hpp ../src/server/base.hpp ../src/utils.hpp ../src/server/common.hpp ../src/server/request.hpp ../src/server/response.hpp ../src/server/status.hpp ../src/server/logger.hpp -lssl -lcrypto -lpthread -lboost_system -O3  -I/usr/include/boost -I/usr/include/openssl -L/usr/lib -o vogro

client: test/test.cpp
	g++ -std=c++11 test/test.cpp -lpthread -lboost_system -o client

run_test:
	make vogro && make test && ./vogro && ./test

clean:
	rm *.o vogro *.gch client