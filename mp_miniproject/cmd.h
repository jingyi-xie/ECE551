#ifndef __CMD_H__
#define __CMD_H__
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;
class Command {
 private:
  string cmdName;
  //Private vector to store all the arguments
  vector<string> argVector;
  //Private helper method to creat the search vector to search the location of the command
  void searchVector(string & commandName,
                    string & cinRead,
                    char *& environ,
                    vector<string> & search);
  //Private helper method to return the location of the command in the search vector
  int checkFiles(vector<string> & range);
  //Private helper method to transfer arguments that contain cariable to its value
  void checkVar(map<string, string> & map);

 public:
  Command(){};
  //Return the name of current command
  char * getCmdName() { return (char *)cmdName.c_str(); }
  //Parse the argument name  according to input string and push to argument vector
  void parseCmdName(string & cinput, char *& environ, map<string, string> & map);
  //Parse all the arguments  according to input string and push to argument vector
  void parseArg(string & wholeStr);
  //Execute the command
  void execute(string & inputStr, char ** newenviron, map<string, string> & map);
  ~Command(){};
};

//This method parses the name of the argument and push it as the first element in the vector
void Command::parseCmdName(string & cinput, char *& environ, map<string, string> & map) {
  stringstream inStream(cinput);
  string tempName;
  //Get the name of the command
  getline(inStream, tempName, ' ');
  //If it has '/', just push it to the vector
  if (tempName.find('/') != string::npos) {
    cmdName = tempName;
    argVector.push_back(tempName);
    return;
  }
  //If it starts with '$', check if it's a variable. If so, replace it with its value.
  if (tempName[0] == '$') {
    string tempStr = tempName.substr(1, tempName.length() - 1);
    if (map.find(tempStr) != map.end()) {
      tempName = map.find(tempStr)->second;
    }
  }
  vector<string> search;
  //The command doesn't contain '/' or a known variable, create the search vector to fill its path
  searchVector(tempName, cinput, environ, search);
  //Get the first location of the command in the search vector
  int pathIndex = checkFiles(search);
  //If command doesn't exist in the search range, report error
  if (pathIndex == -1) {
    cout << "Command " << tempName << " not found\n";
    cmdName = "";
    return;
  }
  else {
    //If found, push the command into the vector as the first element
    cmdName = search[pathIndex];
    argVector.push_back(cmdName);
  }
}

//This method is to parse the arguments and push them to argvector
void Command::parseArg(string & wholeStr) {
  string tempStr;
  //Check if the input is valid
  if (wholeStr.find(' ') == string::npos) {
    return;
  }
  //Get the string of input that excludes the command name
  string str = wholeStr.substr(wholeStr.find(' ') + 1, wholeStr.length() - 1);

  for (size_t i = 0; i < str.length(); i++) {
    //Set a bool to indicate if in quotation mark
    bool inQuote = false;
    //If contains \\, handle it later
    if (i > 0 && str[i] == '\\' && str[i - 1] == '\\') {
      continue;
    }
    //Handle \"
    if (i > 0 && str[i] == '"' && str[i - 1] == '\\' && str[i - 2] != '\\') {
      tempStr[tempStr.length() - 1] = '"';
      continue;
    }

    if (inQuote) {
      //If reach the end of quotation, add to argVector
      if (str[i] == '"') {
        argVector.push_back(tempStr);
        tempStr.clear();
        //Unmark the inQuote flag to indicate that it is now out of quotation
        inQuote = false;
        continue;
      }
      //If reaches the end of input but still don't see a closed quotation mark, report error
      if (i == str.length() - 1) {
        cout << "Unclosed quotation!\n";
        return;
      }
    }
    else {
      //Detect if it is now in quotation
      if (str[i] == '"') {
        inQuote = true;
        continue;
      }
      //A space means a new argument, push the old one to argVector
      if (str[i] == ' ') {
        if (!tempStr.empty()) {
          argVector.push_back(tempStr);
          tempStr.clear();
        }
        continue;
      }
    }
    //Push the current valut to the back of string
    tempStr.push_back(str[i]);
    //If reach the end, push the last argument in argvector
    if (i == str.length() - 1 && !tempStr.empty()) {
      argVector.push_back(tempStr);
    }
  }
}

//This method is to execute the according command by the input
void Command::execute(string & inputStr, char ** newenviron, map<string, string> & map) {
  //Parse the arguments and replace variable's name with its value
  parseArg(inputStr);
  checkVar(map);
  char * tempArgv[256] = {NULL};
  vector<string>::iterator it = argVector.begin();
  size_t index = 0;
  //Create char * according to vector of strings
  while (it != argVector.end()) {
    tempArgv[index] = (char *)it->c_str();
    index++;
    it++;
  }
  //execute the command
  execve(tempArgv[0], tempArgv, newenviron);
  return;
}

//This method is to create the search range for the command name
void Command::searchVector(string & commandName,
                           string & cinRead,
                           char *& environ,
                           vector<string> & search) {
  string envString(environ);
  stringstream envStream(envString);
  stringstream cinStream(cinRead);
  string temp;
  //Separate different paths by ":"
  while (getline(envStream, temp, ':')) {
    search.push_back(temp);
  }
  vector<string>::iterator it = search.begin();
  //Create all the possible absolute paths
  while (it != search.end()) {
    it->push_back('/');
    it->append(commandName);
    it++;
  }
}

//This method search the input vector to see if the command exists and return the position of it
int Command::checkFiles(vector<string> & range) {
  vector<string>::iterator it = range.begin();
  int result = 0;
  while (it != range.end()) {
    const char * tempPath = it->c_str();
    if (!access(tempPath, F_OK)) {
      return result;
    }
    result++;
    it++;
  }
  //If fail to find the command, return -1
  return -1;
}

void Command::checkVar(map<string, string> & map) {
  vector<string>::iterator argIt = argVector.begin();
  argIt++;
  while (argIt != argVector.end()) {
    string str = *argIt;
    stringstream ss(str);
    string temp;
    vector<string> search;
    while (getline(ss, temp, '$')) {
      if (temp != "" && str[max((int)str.find(temp) - 1, 0)] == '$') {
        search.push_back(temp);
      }
    }
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
          str = before.append(value);
          str.append(after);
          *argIt = str;
          break;
        }
      }
      it++;
    }
    argIt++;
  }
}
#endif
