all:
	make split
	make request
	make response
	make regex
	make urlMatch
	make code
	make parse_request_line
	make parse_header

split:
	g++ split_query_string_test.cpp -std=c++11 -o split
	./split
	
	

request:
	g++ request_test.cpp ../request.hpp -std=c++11 -o request
	./request
	

response:
	g++ response_test.cpp ../response.hpp -std=c++11 -o response
	./response
	

regex:
	g++ regex_test.cpp -std=c++11 -lboost-system -o regex
	./regex
	

urlMatch:
	g++ url_match_test.cpp -std=c++11 -o urlMatch
	./urlMatch
	

code:
	g++ code_test.cpp ../status_code.hpp -std=c++11 -o code
	./code
	

parse_request_line:
	g++ parse_request_line_test.cpp -std=c++11 -o parse_request_line
	./parse_request_line
	

parse_header:
	g++ parse_header_test.cpp -std=c++11 -o parse_header
	./parse_header

color:
	g++ color_test.cpp ../common.hpp -std=c++11 -o color
	./color

trim:
	g++ trim_test.cpp ../common.hpp -std=c++11 -o trim
	./trim

log:
	g++ log_test.cpp ../log.hpp -std=c++11 -o log
	./log

mime:
	g++ mime_type_test.cpp ../mime_type.hpp -std=c++11 -o mime
	./mime 
	
clean:
	rm split request regex urlMatch response code parse_request_line  parse_header a.out *.o *.gch color trim mime
	