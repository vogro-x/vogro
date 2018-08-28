#include <iostream>
#include <string>
using namespace std;

void splitQueryString(string str) {
    string key,val;
    auto flag = true;
    for (auto i = 0; i <= str.length(); i++) {
        if (i == str.length() || str[i] == '&') {
            flag = true;
            cout << key << ":" << val << endl;
            key.clear();
            val.clear();
            continue;
        }
        else if (str[i] == '=') {
            flag = false;
            continue;
        }

        if (flag) {
            key += str[i];
        }
        else {
            val += str[i];
        }
    }
}

int main(void) {

    splitQueryString("");
    cout << endl;

    splitQueryString("a=1");
    cout << endl;

    splitQueryString("a=1&b=2");
    cout << endl;

    splitQueryString("id=25&name=andrew");
    cout << endl;

    return 0;
}

/***************************************

OUTPUT


 :

a:1

a:1
b:2

id:25
name:andrew
   
*****************************************/