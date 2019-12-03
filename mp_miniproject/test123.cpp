#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
using namespace std;
int main() {
  string str("1111$var1$var2$var223");
  //cout << str.find("var1") << endl;                     //5
  //cout << str.substr(0, 4) << endl;                     //5-1
  //cout << str.substr(5 + 4, str.length() - 1) << endl;  //find+length

  map<string, string> map;
  map["var1"] = "aa";
  map["var2"] = "bb";
  map["var22"] = "cc";

  stringstream ss(str);
  string temp;
  vector<string> search;
  while (getline(ss, temp, '$')) {
    if (temp != "" && str[max((int)str.find(temp) - 1, 0)] == '$') {
      search.push_back(temp);
    }
  }
  //cout << search[0] << endl;
  //cout << search[1] << endl;
  //cout << search[2] << endl;
  vector<string>::iterator it = search.begin();

  while (it != search.end()) {
    for (size_t i = 0; i < it->length(); i++) {
      size_t len = it->length();
      string temp = it->substr(0, len - i);
      if (map.find(temp) != map.end()) {
        string value = map.find(temp)->second;
        size_t pos = str.find(*it);
        string before = str.substr(0, pos - 1);
        string after = str.substr(pos + temp.length(), str.length() - 1);
        //cout << before << " " << after << endl;
        str = before.append(value);
        str.append(after);
        break;
      }
    }
    it++;
  }
  cout << str;

  /*
  vector<string>::iterator it = search.begin();
  while (it != search.end()) {
    it->push_back('/');
    it->append(commandName);
    it++;
  }
  std::cout << "Hello World!\n";
  */
}
