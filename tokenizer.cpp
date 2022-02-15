#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <cstring>

using namespace std;

// Tokenises a string using the given character delimiter and returns a vector of tokens
vector <string> tokenizer (string command, char delimiter){
  vector <string> tokens;
  stringstream check1(command);   
  string intermediate;

  while(getline(check1, intermediate, delimiter)){
        tokens.push_back(intermediate);
  }
  return tokens;
}

// Tokenises a string using the given string delimiter and returns a vector of tokens
vector <string> tokenizer (string command, string delimiter){
  vector <string> tokens;
  
	string given_str = command;  
	string delim = delimiter;

	size_t pos = 0;  
	string token1; 

	while (( pos = given_str.find (delim)) != std::string::npos)  
	{  
		token1 = given_str.substr(0, pos);  
		tokens.push_back(token1);
		given_str.erase(0, pos + delim.length()); 
	}  
	tokens.push_back(given_str);

  return tokens;
}