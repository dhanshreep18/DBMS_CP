#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>

#include "Header Files/tokenizer.h"

using namespace std;

template <typename T>
bool contains(vector<T> vec, const T & elem)
{
    bool result = false;
    if( find(vec.begin(), vec.end(), elem) != vec.end() )
    {
        result = true;
    }
    return result;
}

// Parser for CREATE TABLE command
bool createTable(string command){
	bool status = 1;

	// to check if table already exists
	string check_name = tokenizer(command, '"')[1];

	std::ifstream ifs("DATABASE FILES/Schema.txt");
	std::string s;
	if(ifs.peek() != std::ifstream::traits_type::eof()){
		while(std::getline(ifs, s)) {
			// std::cout << s << std::endl;
			if(!s.empty())
			{
				if(tokenizer(s,'#')[0] == check_name){
				cout << "\nTable with name \"" << check_name << "\" already exists!" << endl;
				return 0;
				}
			}
		}
	}

	// to check if primary key and foreign key exists
    vector <string> check = tokenizer(command, ' ');
	string p_check = "PRIMARY", f_check = "FOREIGN";
	bool checkPrimary = contains(check, p_check);
	bool checkForeign = contains(check, f_check);
	
	
	// FOREIGN KEYS
	string FOREIGN = "FOREIGN";
	vector <string> FK_Division;
	if(checkForeign == 1){
		FK_Division = tokenizer(command, "FOREIGN KEY");
		for(int i = 1; i < FK_Division.size(); i++){
			vector <string> temp = tokenizer(FK_Division[i],'"');
			FOREIGN = FOREIGN + "#" + temp[1] + "#" + temp[3]+ "#" + temp[5];
		}

		// cout << endl << FOREIGN;
	}else{
		FK_Division.push_back(command);
	}

	// PRIMARY KEYS
	string PRIMARY = "PRIMARY";
	vector <string> PK_Division;
	if(checkPrimary == 1){
		PK_Division = tokenizer(FK_Division[0], "PRIMARY KEY");
		vector <string> temp = tokenizer(PK_Division[1],'"');
		for(int i = 1; i < temp.size(); i = i+2){
			PRIMARY = PRIMARY + "#" + temp[i];
		}

		// cout << endl << PRIMARY;
	}else{
		PK_Division.push_back(FK_Division[0]);
	}

	// ATTRIBUTES and TABLE NAME
	vector <string> TABLE_DEFINITION = tokenizer(PK_Division[0], '(');

	string TABLE_NAME = tokenizer(TABLE_DEFINITION[0], '"')[1];

	vector attributesList = tokenizer(TABLE_DEFINITION[1], ',');
	vector<pair<string, string>> ATTRIBUTES_LIST;
	vector<string> CONSTRAINT;

	for(int i = 0; i < attributesList.size() - 1; i++){
		vector<string> currentAttributePair = tokenizer(attributesList[i], '"');
		pair<string, string> temp;
		boost::algorithm::trim(currentAttributePair[1]);
		boost::algorithm::trim(currentAttributePair[2]);

		temp.first = currentAttributePair[1];
		if(boost::algorithm::contains(currentAttributePair[2], " ")){
			temp.second = tokenizer(currentAttributePair[2], ' ')[0];
		}else
		{
			temp.second = currentAttributePair[2];
		}

		ATTRIBUTES_LIST.push_back(temp);

		if(boost::algorithm::contains(attributesList[i], "NOT NULL")){
			CONSTRAINT.push_back("NOT NULL");
		}

		else if(boost::algorithm::contains(attributesList[i], "UNIQUE")){
			CONSTRAINT.push_back("UNIQUE");
		}

		else if(boost::algorithm::contains(attributesList[i], "CHECK")){
			string temp = tokenizer(attributesList[i],"CHECK")[1];
			boost::algorithm::trim(temp);
			vector<string> components = tokenizer(temp, '"' + currentAttributePair[1] + '"');
			CONSTRAINT.push_back("CHECK : " + currentAttributePair[1]+ components[1]);
		}

		else if(boost::algorithm::contains(attributesList[i], "DEFAULT")){
			string temp = tokenizer(attributesList[i],"DEFAULT")[1];
			boost::algorithm::trim(temp);
			CONSTRAINT.push_back("DEFAULT : " + temp);
		}

		else{
			CONSTRAINT.push_back("NONE");
		}
	}

	string SCHEMA = TABLE_NAME;
	for(int i = 0; i < ATTRIBUTES_LIST.size(); i++){
		SCHEMA = SCHEMA + "#" + ATTRIBUTES_LIST[i].first + "#" + ATTRIBUTES_LIST[i].second + "#" + CONSTRAINT[i];
	}
	SCHEMA = SCHEMA + " ";
	// cout << endl << SCHEMA << endl;

	ofstream myfile;
  	myfile.open ("DATABASE FILES/Schema.txt", std::ios_base::app);
  	myfile << SCHEMA << "\n" << PRIMARY << "\n" << FOREIGN << endl << endl;
  	myfile.close();

	ofstream outfile ;
	string relationFile = "RELATIONS/"+TABLE_NAME+".txt";
	outfile.open(relationFile);
	
	cout << "\nTable created successfully!" << endl;

	return status;
}