#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include "Header Files/tokenizer.h"
#include "Header Files/constraintChecker.h"
#include "Header Files/where.h"

using namespace std;

bool updateInTable(string command){
	bool status=1;

	//1. --> tokenize using space for getting table name
	vector<string> tokens = tokenizer(command, '"');
    string tableName = tokens[1];
	// vector<string> tableN = tokenizer(tableName, '"');
	// tableName = tableN[1];
	// boost::algorithm::trim(tableName);
	// cout<<tableName<<endl;
    
	//2. -->Reading schema file for checking if table exists?
	string schemaData;
	// bool isFound=0;
	ifstream SchemaFile("DATABASE FILES/Schema.txt");
	
    if (!SchemaFile.is_open()) {
        cerr << "Could not open the Schema file - '"<< endl;
        exit(EXIT_FAILURE);
		status=0;
    }
    
	//3. --> Storing schema file data into string
    schemaData = string((std::istreambuf_iterator<char>(SchemaFile)), std::istreambuf_iterator<char>());

	// cout<<schemaData<<endl;

    //4. -->Searching for tableName in schema
	size_t found = schemaData.find(tableName);
	// cout<<string::npos<<" "<<found<<endl;
    if (found != string::npos){
		// cout << "Table found at: " << found << '\n';
	}
	else{
		cout << "Table not found "<< '\n';
		status=0;
	}
	
    
	//5. -->extract required table's meta data
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
	
	//6. --> extract first line of table meta data
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
	// cout<<tableMetaExceptPkFk<<endl;

    //7. --> tokenize first line of TableMetaData using '#'
	vector<string>tknzdTableMetaExceptPkFk = tokenizer(tableMetaExceptPkFk, '#');
	for(int i=0; i<tknzdTableMetaExceptPkFk.size(); i++){
		boost::algorithm::trim(tknzdTableMetaExceptPkFk[i]);
		// cout<<tknzdTableMetaExceptPkFk[i]<<endl;
	}

	//8. --> tokenize command using keyword 'SET'
	vector<string>cmdTknzdSET = tokenizer(command, "SET");
	for(int i=0; i<cmdTknzdSET.size(); i++){
		boost::algorithm::trim(cmdTknzdSET[i]);
		// cout<<cmdTknzdSET[i]<<endl;
	}
	// cout<<cmdTknzdSET[1]<<endl;

	//9. --> tokenize command using 'WHERE' for extracting condition list and data to be updated list
	vector<string>cmdTknzdWHERE = tokenizer(cmdTknzdSET[1], "WHERE");
	for(int i=0; i<cmdTknzdWHERE.size(); i++){
		boost::algorithm::trim(cmdTknzdWHERE[i]);
		// cout<<cmdTknzdWHERE[i]<<endl;
	}
	// cout<<cmdTknzdWHERE[0]<<endl;

    //10. -->tokenize individual attribute values to be updated by ','
	vector<string> dataToBeUpdated = tokenizer(cmdTknzdWHERE[0], ',');
	for(int i=0; i<dataToBeUpdated.size(); i++){
		boost::algorithm::trim(dataToBeUpdated[i]);
	}

	if(checkValueWithConstraint(tableMetaExceptPkFk, dataToBeUpdated)==1){
		// cout<<"Condition true"<<endl;

		vector<pair<int, string>> idxAttrValuePair;
		for(int i=0; i<dataToBeUpdated.size(); i++){
			vector<string> temp = tokenizer(dataToBeUpdated[i], '=');
			vector<string>temp0 = tokenizer(temp[0], '"');
			temp[0]=temp0[1];
			vector<string>temp1 = tokenizer(temp[1], '"');
			temp[1]=temp1[1];

			auto it = find(tknzdTableMetaExceptPkFk.begin(), tknzdTableMetaExceptPkFk.end(), temp[0]);
			if (it == tknzdTableMetaExceptPkFk.end()){
				cout<<"Check Attribute to be updated !"<<endl;
				status=0;
			} 
			else{
				int index = std::distance(tknzdTableMetaExceptPkFk.begin(), it);
				index = (index-1)/3;
				idxAttrValuePair.push_back({index, temp[1]});
				// cout<<idxAttrValuePair[i].first<<" "<<idxAttrValuePair[i].second<<endl;
			}
			// cout<<temp[0]<<" "<<temp[1]<<endl;
		}

		//14. --> Reading records from 
		string records;
		string recordsFileName = "RELATIONS/"+tableName+".txt";
		ifstream RecordsFile(recordsFileName);
		
		if (!RecordsFile.is_open()) {
			cerr << "Could not open the Schema file - '"<< endl;
			exit(EXIT_FAILURE);
			status=0;
		}
		records = string((std::istreambuf_iterator<char>(RecordsFile)), std::istreambuf_iterator<char>());

		// cout<<records<<endl;

		vector<string>tknzRecordDiffLine = tokenizer(records, '\n');
		vector<vector<string>>tknzRecords;
		for(int i=0; i<tknzRecordDiffLine.size(); i++){
			vector<string> temp = tokenizer(tknzRecordDiffLine[i], '#');
			tknzRecords.push_back(temp);
		}

        // cout<<"old record: ";
		// for(int i=0; i<tknzRecords.size(); i++){
		// 	for(int j=0; j<tknzRecords[i].size(); j++){
		// 		cout<<tknzRecords[i][j]<<" ";
		// 	}
		// 	cout<<endl;
		// }
		
		vector<string>conditionList = tokenizer(command, "WHERE");
		vector<bool> boolList = whereChecker(tableName, conditionList[1]);
		int c=0;
		for(int i=0; i<boolList.size(); i++){
			// cout<<boolList[i]<<" ";
			if(boolList[i]==1){
				c++;
				for(int j=0; j<idxAttrValuePair.size(); j++){
					tknzRecords[i][idxAttrValuePair[j].first] = idxAttrValuePair[j].second;
				}
			}
		}
		cout<<c<<" Rows affected"<<endl;
		// cout<<"new record: ";
		// for(int i=0; i<tknzRecords.size(); i++){
		// 	for(int j=0; j<tknzRecords[i].size(); j++){
		// 		cout<<tknzRecords[i][j]<<" ";
		// 	}
		// 	cout<<endl;
		// }

		string newRecords = "";
		for(int i=0; i<tknzRecords.size(); i++){
			for(int j=0; j<tknzRecords[i].size(); j++){
				if(j==0){
					newRecords = newRecords + tknzRecords[i][j];
				}
				else{
					newRecords = newRecords + "#" + tknzRecords[i][j];
				}
				
			}
			newRecords = newRecords + '\n';
		}
		

		// cout<<newRecords;

		//19. --> writing into file new updated record
		ofstream myfile;
		string modifiedTableName = "RELATIONS/"+tableName+".txt";
		myfile.open (modifiedTableName.c_str());

		myfile<<newRecords;

		myfile.close();

		SchemaFile.close();
		RecordsFile.close();
	}
	else{
		// cout<<"Condition false"<<endl;
		status=0;
	}

	return status;
}