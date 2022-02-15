#include <iostream>
#include <string>
#include <cstdio>
#include <fstream>
#include "Header Files/tokenizer.h"

using namespace std;

void dropTable(string command){
	string recordsFileName = "RELATIONS/"+ tokenizer(command, '"')[1]+".txt";
	
	string schemaFile = "DATABASE FILES/Schema.txt";

	// ifstream is(recordsFileName);
	ofstream ofs;
    ofs.open("temp.txt", ofstream::out);

	ifstream file(schemaFile);
  	string str;
  	while (getline(file, str)) {	
		if (str!="\n" && tokenizer(str, '#')[0] == tokenizer(command, '"')[1]){
			getline(file, str);
			getline(file, str);
			getline(file, str);
		}else
		{
			ofs << str << '\n';
			getline(file, str);
			ofs << str << '\n';
			getline(file, str);
			ofs << str << '\n';
			getline(file, str);
			ofs << str << '\n';
		}
	}
    // closing output file
    ofs.close();
    // closing input file
	file.close();
    // remove the original file
	remove(schemaFile.c_str());
   	// rename the file
   	rename("temp.txt", schemaFile.c_str());

	bool status = remove(recordsFileName.c_str());
    if(status==0)
        cout<<"\nTable dropped successfully!";
    else
        cout<<"\nTable does not exist.";
    cout<<endl;
}