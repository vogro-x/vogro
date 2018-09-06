#include <assert.h>
#include <iostream>
#include <map>
#include <string>

// urlMatch determine if the requested path matches the path that set in the
// handler, if mached, the possible path parameters will be stored in the
// storeMap.
bool urlMatch(std::string requestUrl, std::string handlerUrl,
              std::map<std::string, std::string> &storeMap) {
  if (handlerUrl.back() != '/')
    handlerUrl += '/';
  if (requestUrl.back() != '/')
    requestUrl += '/';

  std::string type, name, dynamicParam;
  auto handlerUrlLength = handlerUrl.length();
  auto requestUrlLength = requestUrl.length();
  auto max_length = (handlerUrlLength > requestUrlLength) ? handlerUrlLength
                                                          : requestUrlLength;
  for (auto i = 0, j = 0; (i < max_length) && (j < max_length); i++, j++) {
    if (handlerUrl[i] == '{') {
      auto tempIndex = i + 1;
      bool flag = true; // true代表当前在type域中
      do {
        if (handlerUrl[tempIndex] == ':') {
          flag = false;
          ++tempIndex;
        }

        if (!flag)
          name += handlerUrl[tempIndex];
        else
          type += handlerUrl[tempIndex];

        ++tempIndex;
      } while (handlerUrl[tempIndex] != '}');

      i = tempIndex + 1;

      if (flag == true)
        type = "int"; // default type is int
      do {
        // if type is int, every char in dynamicParam should be in [48,57]
        if ((type == "int") && (requestUrl[j] < 48 || requestUrl[j] > 57))
          return false;
        dynamicParam += requestUrl[j];
        ++j;
      } while (requestUrl[j] != '/');

      // store the dynamic parameters to storeMap
      storeMap[name] = dynamicParam;

      // clear name, type, dynamicParam
      name.clear();
      type.clear();
      dynamicParam.clear();
    }

    if ((i >= handlerUrlLength) || (j >= requestUrlLength) ||
        (handlerUrl[i] != requestUrl[j])) {
      return false;
    }
  }
  return true;
}

int main(void) {
  std::map<std::string, std::string> pathParam;

  std::string reqUrl1 = "/";
  std::string handlerUrl1 = "/";
  assert(urlMatch(reqUrl1, handlerUrl1, pathParam) == true);
  pathParam.clear();

  std::string reqUrl2 = "/abc/dd/kk";
  std::string handlerUrl2 = "/abc/{int:id}/kk";
  assert(urlMatch(reqUrl2, handlerUrl2, pathParam) == false);
  pathParam.clear();

  std::string reqUrl3 = "/abc/25/kk";
  std::string handlerUrl3 = "/abc/{int:id}/kk";
  assert(urlMatch(reqUrl3, handlerUrl3, pathParam) == true);
  assert(pathParam.at("id") == "25");
  pathParam.clear();

  std::string reqUrl4 = "/abc/andrew/kk";
  std::string handlerUrl4 = "/abc/{str:name}/kk";
  assert(urlMatch(reqUrl4, handlerUrl4, pathParam) == true);
  assert(pathParam.at("name") == "andrew");
  pathParam.clear();

  std::string reqUrl5 = "/abc/andrew/kk";
  std::string handlerUrl5 = "/abc/{id}/kk"; // default is int
  assert(urlMatch(reqUrl5, handlerUrl5, pathParam) == false);
  pathParam.clear();

  std::string reqUrl6 = "/abc/100/kk/andrew/";
  std::string handlerUrl6 = "/abc/{int:id}/kk/{str:name}/";
  assert(urlMatch(reqUrl6, handlerUrl6, pathParam) == true);
  assert(pathParam.at("id") == "100");
  assert(pathParam.at("name") == "andrew");
  pathParam.clear();

  std::string reqUrl7 = "/fdg";
  std::string handlerUrl7 = "/";
  assert(urlMatch(reqUrl7, handlerUrl7, pathParam) == false);
  pathParam.clear();

  std::string reqUrl8 = "/username/hjsdfghssdjjjjjj/";
  std::string handlerUrl8 = "/username/{str:name}/";
  assert(urlMatch(reqUrl8, handlerUrl8, pathParam) == true);
  return 0;
}