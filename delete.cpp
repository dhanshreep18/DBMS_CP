#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <boost/algorithm/string.hpp>

#include "Header Files/tokenizer.h"
#include "Header Files/where.h"

using namespace std;

void deleteRecord(string command){
	string tableName = tokenizer(command, '"')[1];

	string recordsFileName = "RELATIONS/"+tableName+".txt";
	
	if(boost::algorithm::contains(command, "WHERE")){
		string conditionList = tokenizer(command, "WHERE")[1];
		conditionList.pop_back();
		
		vector<bool> boolRecords = whereChecker(tableName, conditionList);

		ifstream is(recordsFileName);
		ofstream ofs;
    	ofs.open("temp.txt", ofstream::out);

		int line_num = 0;
		std::ifstream file(recordsFileName);
  		std::string str;
  		while (std::getline(file, str)) {
			if (boolRecords[line_num] == false)
			{
				ofs << str << '\n';
			}
    		line_num++;
  		}
    	// closing output file
    	ofs.close();
    	// closing input file
    	is.close();
    	// remove the original file
    	remove(recordsFileName.c_str());
    	// rename the file
    	rename("temp.txt", recordsFileName.c_str());

		int count = 0;
		for(int i = 0; i < boolRecords.size(); i++){
			if (boolRecords[i] == true) count++;
		}
		cout << count << " rows affected" << endl;

	}else{
		string line;
		int count=0;
	
		ifstream mFile (recordsFileName);   
	
		if(mFile.is_open()) 
		{
			while(mFile.peek()!=EOF)
			{
				getline(mFile, line);
				count++;
			}
			mFile.close();
			std::ofstream ofs;
			ofs.open(recordsFileName, std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			cout << count << " rows affected" << endl;
		}
		else
			cout<<"Table does not exist\n";
	}
}