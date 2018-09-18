$(CXX) = g++ -std=c++11
LAPTH1 = -I/usr/include/boost
LPATH2 = -I/usr/include/openssl
LFLAG1 = -O3 -pthread -lboost_system 
LFLAG2 = -lssl -lcrypto

status.o: status.hpp
	$(CXX) -c status.hpp -o status.o

mime.o: mime.hpp
	$(CXX) -c mime.hpp -o mime.o

utils.o: utils.hpp
	$(CXX) -c utils.hpp -o utils.o

logger.o: logger.hpp
	$(CXX) -c logger.hpp -o logger.o

request.o: request.hpp utils.o
	$(CXX) -c request.hpp utils.o  $(LFLAG1) $(LAPTH1) -o request.o

response.o: response.hpp status.o
	$(CXX) -c response.hpp status.o -o response.o

error.o: default_error_handler.hpp request.o response.o status.o
	$(CXX) -c default_error_handler.hpp request.o response.o status.o -o error.o

context.o: context.hpp request.o response.o
	$(CXX) -c context.hpp request.o response.o -o context.o

base.o: base.hpp context.o logger.o request.o response.o utils.o error.o
	$(CXX) -c base.hpp context.o logger.o request.o response.o error.o utils.o $(LFLAG1) $(LAPTH1) -o base.o

http.o: http.hpp base.o status.o 
	$(CXX) -c http.hpp base.o status.o $(LFLAG1) $(LAPTH1) -o http.o

https.o: https.hpp base.o
	$(CXX) -c https.hpp base.o -o https.o $(LFLAG2) $(LPATH2) -o https.o

vogro_http: main.cpp http.o 
	$(CXX) main.cpp http.o $(LAPTH1) $(LFLAG1) -o vogro_http

vogro_http: main.cpp https.o
	$(CXX) main.cpp https.o $(LAPTH2) $(LFLAG2) -o vogro_http

vogro:
	g++ -std=c++11 main.cpp http.hpp base.hpp utils.hpp common.hpp request.hpp response.hpp status.hpp logger.hpp -lssl -lcrypto -lpthread -lboost_system -O3  -I/usr/include/boost -I/usr/include/openssl -L/usr/lib -o vogro

clean:
	rm *.o vogro *.gch