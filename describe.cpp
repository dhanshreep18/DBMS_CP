#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include "Header Files/tokenizer.h"

using namespace std;

bool describeTable(string command){
	bool status;
	vector<string>tknzCommand = tokenizer(command, '"');
	string tableName = tknzCommand[1];
	cout<<tableName<<endl;

	//2. -->Reading schema file for checking if table exists?
	string schemaData;
	ifstream SchemaFile("DATABASE FILES/Schema.txt");
	
    if (!SchemaFile.is_open()) {
        cerr << "Could not open the Schema file - '"<< endl;
        exit(EXIT_FAILURE);
		status=0;
    }
    
    schemaData = string((std::istreambuf_iterator<char>(SchemaFile)), std::istreambuf_iterator<char>());

	// cout<<schemaData<<endl;

	size_t found = schemaData.find(tableName);
	// cout<<string::npos<<" "<<found<<endl;
    if (found != string::npos){
		// cout << "Table found at: " << found << '\n';
	}
	else{
		cout << "Table not found "<< '\n';
		status=0;
		return status;
	}

	string tableMetaData="";
	int c=0, i=found;
	while(c<3){
		if(schemaData[i]=='\n'){
			c++;
		}
		tableMetaData +=  schemaData[i];
		i++;
	}
	// cout<<tableMetaData<<endl;

	string tableMetaExceptPkFk="";
	int j=0;
	int ctr=0;
	while(ctr < 1){
		if(tableMetaData[j]=='\n'){
			ctr++;
		}
		tableMetaExceptPkFk += tableMetaData[j];
		j++;
	}

	string tableMetaPk="";
	int j_=0;
	int ctr_=0;
	while(ctr_ < 2){
		if(tableMetaData[j_]=='\n' && ctr_ == 0){
			ctr_++;
		}
		tableMetaPk += tableMetaData[j_];
		j++;
	}
	// cout<<tableMetaExceptPkFk<<endl;

	vector<string>tknzTableMetaExceptPkFk = tokenizer(tableMetaExceptPkFk,'#');
	for(int i=1; i<tknzTableMetaExceptPkFk.size(); i=i+3){
		cout<<"ColumnName: "<<tknzTableMetaExceptPkFk[i]<<" Type: "<<tknzTableMetaExceptPkFk[i+1]<<" Constraint: "<<tknzTableMetaExceptPkFk[i+2]<<endl;
	}
	
	return status;
}