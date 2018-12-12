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

#ifndef __MIME_TYPE_HPP
#define __MIME_TYPE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// the following mime types is referenced from Ngnix's mime types

class MimeTypeMap {
private:
    MimeTypeMap() {
        extMime["html"] = "text/html";
        extMime["htm"] = "text/html";
        extMime["shtml"] = "text/html";
        extMime["css"] = "text/css";
        extMime["xml"] = "text/xml";
        extMime["gif"] = "image/gif";
        extMime["jpg"] = "image/jpeg";
        extMime["jpeg"] = "image/jpeg";
        extMime["js"] = "application/javascript";
        extMime["atom"] = "application/atom+xml";
        extMime["rss"] = "application/rss+xml";
        extMime["mml"] = "text/mathml";
        extMime["txt"] = "text/plain";
        extMime["jad"] = "text/vnd.sun.j2me.app-descriptor";
        extMime["wml"] = "text/vnd.wap.wml";
        extMime["htc"] = "text/x-component";
        extMime["png"] = "image/png";
        extMime["tif"] = "image/tiff";
        extMime["tiff"] = "image/tiff";
        extMime["wbmp"] = "image/vnd.wap.wbmp";
        extMime["ico"] = "image/x-icon";
        extMime["jng"] = "image/x-jng";
        extMime["bmp"] = "image/x-ms-bmp";
        extMime["svg"] = "image/svg+xml";
        extMime["svgz"] = "image/svg+xml";
        extMime["webp"] = "image/webp";
        extMime["woff"] = "application/font-woff";
        extMime["jar"] = "application/java-archive";
        extMime["war"] = "application/java-archive";
        extMime["ear"] = "application/java-archive";
        extMime["json"] = "application/json";
        extMime["hqx"] = "application/mac-binhex40";
        extMime["doc"] = "application/msword";
        extMime["pdf"] = "application/pdf";
        extMime["ps"] = "application/postscript";
        extMime["eps"] = "application/postscript";
        extMime["ai"] = "application/postscript";
        extMime["rtf"] = "application/rtf";
        extMime["m3u8"] = "application/vnd.apple.mpegurl";
        extMime["xls"] = "application/vnd.ms-excel";
        extMime["eot"] = "application/vnd.ms-fontobject";
        extMime["ppt"] = "application/vnd.ms-powerpoint";
        extMime["wmlc"] = "application/vnd.wap.wmlc";
        extMime["kml"] = "application/vnd.google-earth.kml+xml";
        extMime["kmz"] = "application/vnd.google-earth.kmz";
        extMime["7z"] = "application/x-7z-compressed";
        extMime["cco"] = "application/x-cocoa";
        extMime["jardiff"] = "application/x-java-archive-diff";
        extMime["jnlp"] = "application/x-java-jnlp-file";
        extMime["run"] = "application/x-makeself";
        extMime["pl"] = "application/x-perl";
        extMime["pm"] = "application/x-perl";
        extMime["prc"] = "application/x-pilot";
        extMime["pdb"] = "application/x-pilot";
        extMime["rar"] = "application/x-rar-compressed";
        extMime["rpm"] = "application/x-redhat-package-manager";
        extMime["sea"] = "application/x-sea";
        extMime["swf"] = "application/x-shockwave-flash";
        extMime["sit"] = "application/x-stuffit";
        extMime["tcl"] = "application/x-tcl";
        extMime["tk"] = "application/x-tcl";
        extMime["der"] = "application/x-x509-ca-cert";
        extMime["pem"] = "application/x-x509-ca-cert";
        extMime["crt"] = "application/x-x509-ca-cert";
        extMime["xpi"] = "application/x-xpinstall";
        extMime["xhtml"] = "application/xhtml+xml";
        extMime["xspf"] = "application/xspf+xml";
        extMime["zip"] = "application/zip";
        extMime["bin"] = "application/octet-stream";
        extMime["exe"] = "application/octet-stream";
        extMime["dll"] = "application/octet-stream";
        extMime["deb"] = "application/octet-stream";
        extMime["dmg"] = "application/octet-stream";
        extMime["iso"] = "application/octet-stream";
        extMime["img"] = "application/octet-stream";
        extMime["msi"] = "application/octet-stream";
        extMime["msp"] = "application/octet-stream";
        extMime["msm"] = "application/octet-stream";
        extMime["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        extMime["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        extMime["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        extMime["mid"] = "audio/midi";
        extMime["midi"] = "audio/midi";
        extMime["kar"] = "audio/midi";
        extMime["mp3"] = "audio/mpeg";
        extMime["ogg"] = "audio/ogg";
        extMime["m4a"] = "audio/x-m4a";
        extMime["ra"] = "audio/x-realaudio";
        extMime["3gpp"] = "video/3gpp";
        extMime["3gp"] = "video/3gpp";
        extMime["ts"] = "video/mp2t";
        extMime["mp4"] = "video/mp4";
        extMime["mpeg"] = "video/mpeg";
        extMime["mpg"] = "video/mpeg";
        extMime["mov"] = "video/quicktime";
        extMime["webm"] = "video/webm";
        extMime["flv"] = "video/x-flv";
        extMime["m4v"] = "video/x-m4v";
        extMime["mng"] = "video/x-mng";
        extMime["asx"] = "video/x-ms-asf";
        extMime["asf"] = "video/x-ms-asf";
        extMime["wmv"] = "video/x-ms-wmv";
        extMime["avi"] = "video/x-msvideo";
    }

    std::unordered_map <std::string, std::string> extMime;

public:
    MimeTypeMap(const MimeTypeMap &) = delete;

    MimeTypeMap &operator=(const MimeTypeMap &) = delete;

    std::string getMimeTypeByExt(const std::string & ext) {
        auto got = extMime.find(ext);
        return (got == extMime.end()) ? "" : got->second;
    }

    static MimeTypeMap &GetInstance() {
        static MimeTypeMap instance;  //局部静态变量
        return instance;
    }
};

#endif
