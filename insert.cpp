#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <map>
#include "Header Files/tokenizer.h"
#include "Header Files/constraintChecker.h"

using namespace std;

bool insertInTable(string command){
	bool status=1;
    
	map<string, int>mapOfConstraints;
	mapOfConstraints["DEFAULT"]=1;
	mapOfConstraints["NONE"]=2;
	mapOfConstraints["NOT"]=3;
	mapOfConstraints["UNIQUE"]=4;
	mapOfConstraints["CHECK"]=5;

	//1. --> tokenize using space for getting table name

	vector<string> tokenizingTN = tokenizer(command, '"');
	// cout<<tokenizingTN[1];
	string tableName = tokenizingTN[1];

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
	// cout<<tableMetaExceptPkFk<<endl;

	vector<string>metaDataTknzd=tokenizer(tableMetaExceptPkFk, '#');
	map<string, int>mappingOfColAndIndex;
	int index;
	for(int i=1; i<metaDataTknzd.size(); i=i+3){
		index=(i-1)/3;
		// cout<<metaDataTknzd[i]<<" "<<index<<endl;
		mappingOfColAndIndex[metaDataTknzd[i]] = index;
	}

	string records;
	string tableName1 = metaDataTknzd[0];
	string recordsFileName = "RELATIONS/"+tableName1+".txt";
	ifstream RecordsFile(recordsFileName);
	
	if (!RecordsFile.is_open()) {
		cerr << "Could not open the Schema file - '"<< endl;
		exit(EXIT_FAILURE);
		status=0;
	}
	records = string((std::istreambuf_iterator<char>(RecordsFile)), std::istreambuf_iterator<char>());

	// cout<<records<<endl;

	vector<string>recordOfEach = tokenizer(records, '\n');
	vector<vector<string>>tknzRecordOfEach;

	//storing records into vector of strings
	for(int i=0; i<recordOfEach.size(); i++){
		// cout<<recordOfEach[i]<<endl;
		vector<string>temp = tokenizer(recordOfEach[i], '#');
		tknzRecordOfEach.push_back(temp);
	}

	



	// -----------------
	
    //2. --> tokenize whole command for '('
	vector<string> tokensExceptTableName = tokenizer(command, '(');
	//cout<<tokensExceptTableName[1];	

	//3. --> tokenize  tokensExceptTableName for ')'
	vector<string> tokensForValues = tokenizer(tokensExceptTableName[1], ')');
	//cout<<tokensForValues[0];
    
	//4. tokenize for ',' and get all the values list
	vector<string> valuesList = tokenizer(tokensForValues[0], ',');
	vector<string> VALUES_LIST;
	
	for(int i=0; i<valuesList.size(); i++){
		vector<string>temp = tokenizer(valuesList[i], '"');
		// cout<<temp[1]<<endl;
		boost::algorithm::trim(temp[1]);
		VALUES_LIST.push_back(temp[1]);
	}

	vector<string>tknzTableMetaExceptPkFk = tokenizer(tableMetaExceptPkFk, '#');
	vector<string>columns;
	for(int i=1; i<tknzTableMetaExceptPkFk.size(); i=i+3){
		columns.push_back(tknzTableMetaExceptPkFk[i]);
	}

	vector<string>newVALUES_LIST;

	if(VALUES_LIST.size() == columns.size()){
		vector<pair<string, string>> newValuesWithColumns;

		for(int i=0; i<VALUES_LIST.size(); i++){
			newValuesWithColumns.push_back({columns[i], VALUES_LIST[i]});
		}

		//find the index and col in map
		vector<pair<string, string>> colAndConstraint;

		for(int i=0; i<newValuesWithColumns.size(); i++){
			if(mappingOfColAndIndex.find(newValuesWithColumns[i].first) != mappingOfColAndIndex.end()){
				// cout<<mappingOfColAndIndex[newValuesWithColumns[i].first];
				//extract constraint from meta data
				string constraint = metaDataTknzd[mappingOfColAndIndex[newValuesWithColumns[i].first]*3+1+2];

				colAndConstraint.push_back({newValuesWithColumns[i].first, constraint});

				// cout<<colAndConstraint[i].first<<" "<<colAndConstraint[i].second<<endl;
			}
			else{
				cout<<"Col not found"<<endl;
			}
		}

		// cout<<newValuesWithColumns.size()<<" "<<endl;
		// cout<<colAndConstraint.size()<<" "<<endl;

		for(int i=0; i<newValuesWithColumns.size(); i++){
			// cout<<colAndConstraint[i].second<<" ";
			vector<string>tknzConstraint;
			tknzConstraint=tokenizer(colAndConstraint[i].second, ' ');
			// cout<<mapOfConstraints[tknzConstraint[0]]<<endl;
			int toBeCheckedWith = mapOfConstraints[tknzConstraint[0]];
			// for(int j=0; j<tknzConstraint.size()+1; j++){
			// 	cout<<tknzConstraint[j]<<" "<<toBeCheckedWith;
			// }
			// cout<<endl;
			switch(toBeCheckedWith){
				case 1: { 
					//DEFAULT
					// cout<<"case 1: "<<toBeCheckedWith<<endl;
					// cout<<colAndConstraint[i].second<<endl;
					string constraint = colAndConstraint[i].second;
					vector<string>tknzConstraint = tokenizer(constraint, ' ');
					vector<string>constraintValue = tokenizer(tknzConstraint[2], '"');
					// cout<<constraintValue[1]<<endl;
					// cout<<newRecordToBeChecked[i].second<<endl;
					
					if(newValuesWithColumns[i].second == ""){
						newVALUES_LIST.push_back(constraintValue[1]);
					}
					else{
						newVALUES_LIST.push_back(newValuesWithColumns[i].second);
					}

					break;
				}
				case 2:{
					//NONE
					// cout<<colAndConstraint[i].second<<endl;
					// cout<<newRecordToBeChecked[i].second<<endl;
					// cout<<"case 2: "<<toBeCheckedWith<<endl;
					if(status==0){

					}
					else{
						// status=1;
						newVALUES_LIST.push_back(newValuesWithColumns[i].second);
					}
					break;
				}
				case 3:{
					//NOT NULL
					// cout<<"case 3: "<<toBeCheckedWith<<endl;
					// cout<<colAndConstraint[i].second<<endl;
					// cout<<newValuesWithColumns[i].second<<endl;
					if(newValuesWithColumns[i].second == " "){
						cout<<"Value cannot be NULL"<<endl;
					}
					else{
						newVALUES_LIST.push_back(newValuesWithColumns[i].second);
					}
					break;
				}
				case 4:{
					//UNIQUE
					// cout<<"case 4: "<<toBeCheckedWith<<endl;
					// cout<<colAndConstraint[i].second<<endl;
					// cout<<newRecordToBeChecked[i].second<<endl;
					vector<string>recordOfEach = tokenizer(records, '\n');
					vector<vector<string>>tknzRecordOfEach;
					for(int k=0; k<recordOfEach.size(); k++){
						// cout<<recordOfEach[i]<<endl;
						vector<string>temp = tokenizer(recordOfEach[k], '#');
						tknzRecordOfEach.push_back(temp);
					}

					// for(int i=0; i<tknzRecordOfEach.size(); i++){
					// 	for(int j=0; j<tknzRecordOfEach[i].size(); j++){
					// 		cout<<tknzRecordOfEach[i][j]<<" ";
					// 	}
					// 	cout<<endl;
					// }
					
					int indexToBeChecked = mappingOfColAndIndex[colAndConstraint[i].first];
					// cout<<indexToBeChecked<<endl;
					for(int k=0; k<tknzRecordOfEach.size(); k++){
						if(tknzRecordOfEach[k][indexToBeChecked]==newValuesWithColumns[i].second){
							cout<<"Same Column Value found in records"<<endl;
							status=0;
							break;
						}
						else{
							// cout<<"Same Column Value NOT found in records"<<endl;
							
						}
					}
					if(status!=0){
						newVALUES_LIST.push_back(newValuesWithColumns[i].second);
					}
					break;
				}
				case 5:{
					//CHECK
					// cout<<"case 5: "<<toBeCheckedWith<<endl;
					// cout<<colAndConstraint[i].second<<endl;
					// cout<<newRecordToBeChecked[i].second<<endl;
					
					int indexToBeChecked = mappingOfColAndIndex[colAndConstraint[i].first];

					// cout<<indexToBeChecked<<endl;
					string operatorToBeApplied = tknzConstraint[3];

					if(operatorToBeApplied == "!="){
						string newValue = newValuesWithColumns[i].second;
						vector<string> constraint = tokenizer(tknzConstraint[4], '"');
						string constraintValue = constraint[1];
						// cout<<constraintValue<<endl;
						if(newValue==constraintValue){
							cout<<"Constraint value and new value is matching !"<<endl;
							status=0;
						}
						else{
							newVALUES_LIST.push_back(newValuesWithColumns[i].second);
						}
					}

					else if(operatorToBeApplied == "=="){

					}

					else if(operatorToBeApplied == ">="){
						string newValue = newValuesWithColumns[i].second;
						vector<string> constraint = tokenizer(tknzConstraint[4], '"');
						string constraintValue = constraint[1];
						// cout<<constraintValue<<endl;

						int value = stoi(newValue);
						int cValue = stoi(constraintValue);
						if(value < cValue){
							cout<<"Value to be inserted is not >=  given constraint !"<<endl;
							status=0;
						}
						else{
							newVALUES_LIST.push_back(newValuesWithColumns[i].second);
						}
					}

					else if(operatorToBeApplied == ">"){
						string newValue = newValuesWithColumns[i].second;
						vector<string> constraint = tokenizer(tknzConstraint[4], '"');
						string constraintValue = constraint[1];
						// cout<<constraintValue<<endl;

						int value = stoi(newValue);
						int cValue = stoi(constraintValue);
						if(value <= cValue){
							cout<<"Value to be inserted is not > given constraint !"<<endl;
							status=0;
						}
						else{
							newVALUES_LIST.push_back(newValuesWithColumns[i].second);
						}
					}

					else if(operatorToBeApplied == "<="){
						string newValue = newValuesWithColumns[i].second;
						vector<string> constraint = tokenizer(tknzConstraint[4], '"');
						string constraintValue = constraint[1];
						// cout<<constraintValue<<endl;

						int value = stoi(newValue);
						int cValue = stoi(constraintValue);
						if(value > cValue){
							cout<<"Value to be inserted is not <= given constraint !"<<endl;
							status=0;
						}
						else{
							newVALUES_LIST.push_back(newValuesWithColumns[i].second);
						}
					}

					else if(operatorToBeApplied == "<"){
						string newValue = newValuesWithColumns[i].second;
						vector<string> constraint = tokenizer(tknzConstraint[4], '"');
						string constraintValue = constraint[1];
						// cout<<constraintValue<<endl;

						int value = stoi(newValue);
						int cValue = stoi(constraintValue);
						if(value >= cValue){
							cout<<"Value to be inserted is not < given constraint !"<<endl;
							status=0;
						}
						else{
							newVALUES_LIST.push_back(newValuesWithColumns[i].second);
						}
					}
					
					break;
				}
				default:{
					cout<<"Wrong constraint in command\n";
					break;
				}
			}
		}
		// if(status==1){
		// 	for(int i=0; i<newVALUES_LIST.size(); i++){
		// 		cout<<newVALUES_LIST[i]<<" ";
		// 	}
		// }

		// -----------------
		if(status==1){
			string TUPLE_VALUE = newVALUES_LIST[0];
			for(int i = 1; i < newVALUES_LIST.size(); i++){
				TUPLE_VALUE = TUPLE_VALUE + "#" + newVALUES_LIST[i];
			}
			//cout<<TUPLE_VALUE;
		
			ofstream myfile;
			//5. --> create relation file for table
			string modifiedTableName = "RELATIONS/"+tableName+".txt";
			myfile.open (modifiedTableName.c_str(), std::ios_base::app);
			myfile << TUPLE_VALUE <<"\n";
			myfile.close();
		}
		
	}
	else{
		cout<<"Less values than expected";
		status=0;
		return status;
	}
	
	return status;
}