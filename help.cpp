#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include "Header Files/tokenizer.h"

using namespace std;

bool help(string command){
	bool status;
	vector<string>tknzCommand = tokenizer(command, ' ');
	// cout<<tknzCommand[1]<<endl;

	string schemaData;
	ifstream SchemaFile("DATABASE FILES/Schema.txt");
	
    if (!SchemaFile.is_open()) {
        cerr << "Could not open the Schema file - '"<< endl;
        exit(EXIT_FAILURE);
		status=0;
    }
    
    schemaData = string((std::istreambuf_iterator<char>(SchemaFile)), std::istreambuf_iterator<char>());

	vector<string>tknzSchemaData = tokenizer(schemaData, '\n');
	
	// cout<<tknzSchemaData[0]<<endl;
	vector<string>allTablesMetaData;
	for(int i=0; i<tknzSchemaData.size(); i=i+4){
		allTablesMetaData.push_back(tknzSchemaData[i]);
	}
	// cout<<allTablesMetaData[1]<<endl;
	vector<vector<string>>tknzAllTablesMetaData;
	for(int i=0; i<allTablesMetaData.size(); i++){
		vector<string>temp = tokenizer(allTablesMetaData[i], '#');
		tknzAllTablesMetaData.push_back(temp);
	}
	// cout<<tknzAllTablesMetaData[1][0]<<endl;

	string toDo = tknzCommand[1];
	if(toDo == "TABLES"){
		cout<<"Available Tables in Database:-"<<endl;
		for(int i=0; i<tknzAllTablesMetaData.size(); i++){
			cout<<tknzAllTablesMetaData[i][0]<<endl;
		}
	}
	else if(toDo == "CMD"){
		cout<<"Commands Available:- "<<endl;
		cout<<"DDL : CREATE, DROP"<<endl;
		cout<<"DML : SELECT, INSERT, UPDATE, DELETE"<<endl;
	}
	else if(toDo == "CREATE"){
		cout<<"Create table in Database using the below expected command: "<<endl;
		cout<<"CREATE TABLE table_name ( attribute_1 attribute1_type CHECK (constraint1), attribute_2 attribute2_type, …, PRIMARY KEY ( attribute_1, attribute_2 ), FOREIGN KEY ( attribute_y ) REFERENCES table_x ( attribute_t ), FOREIGN KEY ( attribute_w ) REFERENCES table_y ( attribute_z )… ) "<<endl;
	}
	else if(toDo == "DROP"){
		cout<<"Drop table in Database using the below expected command: "<<endl;
		cout<<"DROP TABLE table_name "<<endl;
	}
	else if(toDo == "SELECT"){
		cout<<"Display records from table using the below expected command: "<<endl;
		cout<<"SELECT attribute_list FROM table_list WHERE condition_list "<<endl;
	}
	else if(toDo == "INSERT"){
		cout<<"Enter records from table using the below expected command: "<<endl;
		cout<<"INSERT INTO table_name VALUES ( val1, val2, … ) "<<endl;
	}
	else if(toDo == "DELETE"){
		cout<<"Delete records from table using the below expected command: "<<endl;
		cout<<"DELETE FROM table_name WHERE condition_list "<<endl;
	}
	else if(toDo == "UPDATE"){
		cout<<"Update records from table using the below expected command: "<<endl;
		cout<<"UPDATE table_name SET attr1 = val1, attr2 = val2… WHERE condition_list "<<endl;
	}
	return status;
}