#include <iostream>
#include <string>
#include <map>

std::string get_real_path(const std::string& originPath, const std::map<std::string,std::string>& pathParam){
    auto length = originPath.length();
    std::string realPath;
    for(int i=0;i<length;i++) {
        if(originPath[i]=='{') {
            std::string key="";
            for(int j=i+1;j<length;j++){
                if(originPath[j]=='}'){
                    auto got = pathParam.find(key);
                    if(got == pathParam.end()){
                        std::cout<<"Fatal: Lock of path Parameter "<<key<<std::endl;
                        exit(1);
                    }else{
                        realPath += got->second;
                        i= j;
                        break;
                    }
                }else{
                    key+= originPath[j];
                }

            }
        }else{
            realPath +=originPath[i];
        }
    }

    if (realPath[realPath.length()-1] != '/')
        realPath += '/';

    return realPath;
}


int main(void){
    std::map<std::string,std::string> pathParam;
    pathParam["name"]= "Andrewpqc";
    pathParam["id"] ="123";
    pathParam["n"] ="vbb";

    auto realPath = get_real_path("/hh/{name}/{id}/{n}/",pathParam);
    std::cout<<realPath<<std::endl;


    return 0;
}