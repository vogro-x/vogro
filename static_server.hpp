#ifndef __STATIC_SERVER_HPP__
#define __STATIC_SERVER_HPP__
#include "request.hpp"
#include "response.hpp"
#include "vogro_utils.hpp"
#include "mime_type.hpp"

#include <boost/asio.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <memory>

template <typename socket_type>
void ServeStatic(vogro::Response &response, vogro::Request &request, std::ostream& responseStream,
    std::shared_ptr<boost::asio::streambuf>& write_buffer,socket_type socket){
    char *buffer;
	if((buffer = getcwd(NULL, 0)) == NULL)
	{
		perror("getcwd error");
	}
	
    auto cwd = std::string(buffer);
    auto path = request.getPath();
    std::cout<<"cwd:"<<cwd<<std::endl;
    std::cout<<"path"<<path<<std::endl;

	auto filepath = cwd+ path;

    response.addHeader("Connection","Keep-Alive");

    auto ext = getFileExtension(filepath);
     MimeTypeMap &mime = MimeTypeMap::GetInstance();
     auto type = mime.getMimeTypeByExt(ext);
  if (type == ""){
      response.setCode(415);
        // not support the requested media type
  }else{

      response.addHeader("Content-Type",type);
        std::ifstream ifs;
        ifs.open(filepath, std::ifstream::in|std::ifstream::binary);
        if(ifs){
            ifs.seekg(0, std::ios::end);
              size_t length=ifs.tellg();
   
              ifs.seekg(0, std::ios::beg);
            std::stringstream ss;
            ss<<ifs.rdbuf();
            std::string s;
            ss>>s;
              response.addBody(s);
              ifs.close();
    
        }


  }

  boost::asio::async_write( *socket, *write_buffer,
        [](const boost::system::error_code &ec, size_t bytes_transferred) {
            // do nothing here.
        });
    
}





#endif