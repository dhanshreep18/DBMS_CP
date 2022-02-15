#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <cstring>
#include <map>
#include <fstream>
#include "Header Files/tokenizer.h"

using namespace std;

bool checkValueWithConstraint (string tableMetaExceptPkFk, vector<string> dataToBeUpdated){
	bool flag=1;
	// cout<<tableMetaExceptPkFk<<endl;
	map<string, int>mapOfConstraints;
	mapOfConstraints["DEFAULT"]=1;
	mapOfConstraints["NONE"]=2;
	mapOfConstraints["NOT"]=3;
	mapOfConstraints["UNIQUE"]=4;
	mapOfConstraints["CHECK"]=5;

	vector<pair<string, string>> newRecordToBeChecked;

	for(int i=0; i<dataToBeUpdated.size(); i++){
		// cout<<dataToBeUpdated[i]<<endl;
		
		vector<string>newTemp=tokenizer(dataToBeUpdated[i], '=');
		vector<string>updatingColTknz=tokenizer(newTemp[0], '"');
		vector<string>updatingColValTknz=tokenizer(newTemp[1], '"');
		// cout<<updatingColTknz[1]<<" "<<updatingColValTknz[1]<<endl;

		newRecordToBeChecked.push_back({updatingColTknz[1], updatingColValTknz[1]});
	}
	
	vector<string>metaDataTknzd=tokenizer(tableMetaExceptPkFk, '#');
	map<string, int>mappingOfColAndIndex;
	int index;
	for(int i=1; i<metaDataTknzd.size(); i=i+3){
		index=(i-1)/3;
		// cout<<metaDataTknzd[i]<<" "<<index<<endl;
		mappingOfColAndIndex[metaDataTknzd[i]] = index;
	}

	string records;
	string tableName = metaDataTknzd[0];
	string recordsFileName = "RELATIONS/"+tableName+".txt";
	ifstream RecordsFile(recordsFileName);
	
	if (!RecordsFile.is_open()) {
		cerr << "Could not open the Schema file - '"<< endl;
		exit(EXIT_FAILURE);
		flag=0;
	}
	records = string((std::istreambuf_iterator<char>(RecordsFile)), std::istreambuf_iterator<char>());

	// cout<<records<<endl;

	vector<string>recordOfEach = tokenizer(records, '\n');
	vector<vector<string>>tknzRecordOfEach;

	for(int i=0; i<recordOfEach.size(); i++){
		// cout<<recordOfEach[i]<<endl;
		vector<string>temp = tokenizer(recordOfEach[i], '#');
		tknzRecordOfEach.push_back(temp);
	}

	//find the index and col in map
	vector<pair<string, string>> colAndConstraint;

	for(int i=0; i<newRecordToBeChecked.size(); i++){
		if(mappingOfColAndIndex.find(newRecordToBeChecked[i].first) != mappingOfColAndIndex.end()){
			// cout<<mappingOfColAndIndex[newRecordToBeChecked[i].first];
			//extract constraint from meta data
			string constraint = metaDataTknzd[mappingOfColAndIndex[newRecordToBeChecked[i].first]*3+1+2];

			colAndConstraint.push_back({newRecordToBeChecked[i].first, constraint});

			// cout<<colAndConstraint[i].first<<" "<<colAndConstraint[i].second<<endl;
		}
		else{
			cout<<"Col not found"<<endl;
		}
	}
	
	for(int i=0; i<newRecordToBeChecked.size(); i++){
		vector<string>tknzConstraint;
		tknzConstraint=tokenizer(colAndConstraint[i].second, ' ');
		// cout<<mapOfConstraints[tknzConstraint[0]]<<endl;
		int toBeCheckedWith = mapOfConstraints[tknzConstraint[0]];
		switch(toBeCheckedWith){
			case 1: { 
				//DEFAULT
				// cout<<colAndConstraint[i].second<<endl;
				string constraint = colAndConstraint[i].second;
				vector<string>tknzConstraint = tokenizer(constraint, ' ');
				vector<string>constraintValue = tokenizer(tknzConstraint[2], '"');
				// cout<<constraintValue[1]<<endl;
				// cout<<newRecordToBeChecked[i].second<<endl;
				if(newRecordToBeChecked[i].second == " " || newRecordToBeChecked[i].second != " "){
					if(flag==0){

					}
					else{
						flag=1;
					}
				}

				break;
			}
			case 2:{
				//NONE
				// cout<<colAndConstraint[i].second<<endl;
				// cout<<newRecordToBeChecked[i].second<<endl;
				if(flag==0){

				}
				else{
					flag=1;
				}
				break;
			}
			case 3:{
				//NOT NULL
				// cout<<colAndConstraint[i].second<<endl;
				cout<<newRecordToBeChecked[i].second<<endl;
				if(newRecordToBeChecked[i].second == " "){
					cout<<"Value cannot be NULL"<<endl;
				}
				break;
			}
			case 4:{
				//UNIQUE
				// cout<<colAndConstraint[i].second<<endl;
				// cout<<newRecordToBeChecked[i].second<<endl;
				vector<string>recordOfEach = tokenizer(records, '\n');
				vector<vector<string>>tknzRecordOfEach;
				for(int i=0; i<recordOfEach.size(); i++){
					// cout<<recordOfEach[i]<<endl;
					vector<string>temp = tokenizer(recordOfEach[i], '#');
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
					if(tknzRecordOfEach[k][indexToBeChecked]==newRecordToBeChecked[i].second){
						cout<<"Same Column Value found in records"<<endl;
						flag=0;
						break;
					}
					else{
						// cout<<"Same Column Value NOT found in records"<<endl;
					}
				}
				break;
			}
			case 5:{
				//CHECK
				// cout<<colAndConstraint[i].second<<endl;
				// cout<<newRecordToBeChecked[i].second<<endl;
				
				int indexToBeChecked = mappingOfColAndIndex[colAndConstraint[i].first];

				// cout<<indexToBeChecked<<endl;
				string operatorToBeApplied = tknzConstraint[3];

				if(operatorToBeApplied == "!="){
					string newValue = newRecordToBeChecked[i].second;
					vector<string> constraint = tokenizer(tknzConstraint[4], '"');
					string constraintValue = constraint[1];
					// cout<<constraintValue<<endl;
					if(newValue==constraintValue){
						cout<<"Constraint value and new value is matching !"<<endl;
						flag=0;
					}
				}

				else if(operatorToBeApplied == "=="){

				}

				else if(operatorToBeApplied == ">="){
					string newValue = newRecordToBeChecked[i].second;
					vector<string> constraint = tokenizer(tknzConstraint[4], '"');
					string constraintValue = constraint[1];
					// cout<<constraintValue<<endl;

					int value = stoi(newValue);
					int cValue = stoi(constraintValue);
					if(value < cValue){
						cout<<"Value to be updated is not >=  given constraint !"<<endl;
						flag=0;
					}
				}

				else if(operatorToBeApplied == ">"){
					string newValue = newRecordToBeChecked[i].second;
					vector<string> constraint = tokenizer(tknzConstraint[4], '"');
					string constraintValue = constraint[1];
					// cout<<constraintValue<<endl;

					int value = stoi(newValue);
					int cValue = stoi(constraintValue);
					if(value <= cValue){
						cout<<"Value to be updated is not > given constraint !"<<endl;
						flag=0;
					}
				}

				else if(operatorToBeApplied == "<="){
					string newValue = newRecordToBeChecked[i].second;
					vector<string> constraint = tokenizer(tknzConstraint[4], '"');
					string constraintValue = constraint[1];
					// cout<<constraintValue<<endl;

					int value = stoi(newValue);
					int cValue = stoi(constraintValue);
					if(value > cValue){
						cout<<"Value to be updated is not <= given constraint !"<<endl;
						flag=0;
					}
				}

				else if(operatorToBeApplied == "<"){
					string newValue = newRecordToBeChecked[i].second;
					vector<string> constraint = tokenizer(tknzConstraint[4], '"');
					string constraintValue = constraint[1];
					// cout<<constraintValue<<endl;

					int value = stoi(newValue);
					int cValue = stoi(constraintValue);
					if(value >= cValue){
						cout<<"Value to be updated is not < given constraint !"<<endl;
						flag=0;
					}
				}
				
				break;
			}
			default:{
				cout<<"Wrong constraint in command\n";
			}
		}
	}

	return flag;
}