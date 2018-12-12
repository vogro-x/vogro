/************************************************************************
 Copyright 2018 andrewpqc@mails.ccnu.edu.cn
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ************************************************************************/

#ifndef __VOGRO_LOGO_HPP__
#define __VOGRO_LOGO_HPP__

#include "utils.hpp"
#include <iostream>

void printLogo(unsigned short port, const std::string & version){
        std::cout<<INITCOLOR(YELLOW_COLOR)<<std::endl<<std::endl;
        std::cout<<"        __      __                                                                "<<std::endl;
        std::cout<<"        \\ \\    / /                                                              "<<std::endl;
        std::cout<<"         \\ \\  / /__   __ _ _ __ ___      Vogro Version "<<version               <<std::endl;
        std::cout<<"          \\ \\/ / _ \\ / _` | '__/ _ \\     Vogro Server Listened on: "<<port    <<std::endl;
        std::cout<<"           \\  / (_) | (_| | | | (_) |    © 2018 Andrewpqc. All rights reserved. "<<std::endl;                      
        std::cout<<"            \\/ \\___/ \\__, |_|  \\___/                                          "<<std::endl;
        std::cout<<"                      __/ |                                                       "<<std::endl;
        std::cout<<"                     |___/                                                        "<<std::endl;
        std::cout<<INITCOLOR(ZERO_COLOR)<<std::endl<<std::endl;   
}


#endif