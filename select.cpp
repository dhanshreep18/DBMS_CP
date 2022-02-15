#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <map>
#include "Header Files/tokenizer.h"
#include "Header Files/where.h"

using namespace std;

bool isNumber(string s)
{
    for (int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == false)
            return false;
 
    return true;
}

bool selectFromTable(string command){
	bool status=1;
    // cout<<command;
	vector<string>forTableName = tokenizer(command, "FROM");
	// cout<<forTableName[1];
	vector<string>for1tableName = tokenizer(forTableName[1], '"');
	
	string tableName = for1tableName[1];
	// cout<<tableName;

	//1. --> Getting records from table file
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

	vector<string>recordOfEach = tokenizer(records, '\n');
	vector<vector<string>>tknzRecordOfEach;

	for(int i=0; i<recordOfEach.size(); i++){
		// cout<<recordOfEach[i]<<endl;
		vector<string>temp = tokenizer(recordOfEach[i], '#');
		tknzRecordOfEach.push_back(temp);
	}
	
	//4. --> Mapping of columns in table & index
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
		// mappingOfColAndIndex.insert({metaDataTknzd[i] , index});
	}


	//3. --> extracting column list from command
	vector<string>beforeTknzColList = tokenizer(forTableName[0], "SELECT");

    boost::algorithm::trim(beforeTknzColList[1]);
    // cout<<beforeTknzColList[1]<<endl;

	vector<string>colListToDisplay;
	
	if(beforeTknzColList[1] == "*"){
		// cout<<"* encountered"<<endl;
		for (auto it = mappingOfColAndIndex.cbegin(); it != mappingOfColAndIndex.cend(); it++){
        	// cout<< (*it).first <<"\n";
			colListToDisplay.push_back((*it).first);
        }
	}
	else{
		vector<string>bColList = tokenizer(beforeTknzColList[1], ',');
		
		for(int i=0; i<bColList.size(); i++){
			vector<string>temp = tokenizer(bColList[i], '"');
			colListToDisplay.push_back(temp[1]);
			// cout<<temp[1]<<endl;
		}
	}
	
	if(command.find("WHERE") == std::string::npos){
		// cout<<"where not found"<<endl;
		for(int i=0; i<tknzRecordOfEach.size(); i++){
			for(int j=0; j<colListToDisplay.size(); j++){
				int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
				cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
			}
			cout<<endl;	    
		}
		return status;
	}
	
	else{
		//4. --> extracting conditions
		vector<string>tknzWHERE = tokenizer(command, "WHERE");
		
		if(tknzWHERE[1].find("AND")!= std::string::npos){
			// cout<<"and ayaa"<<endl;
			vector<bool>boolList = whereChecker(tableName, tknzWHERE[1]);
			for(int i=0; i<boolList.size(); i++){
				// cout<<boolList[i]<<" ";
				if(boolList[i]==1){
					for(int j=0; j<colListToDisplay.size(); j++){
						cout<<tknzRecordOfEach[i][mappingOfColAndIndex[colListToDisplay[j]]]<<" ";
					}
					cout<<endl;
				}
			}	
		}

		else if(tknzWHERE[1].find("OR")!= std::string::npos){
			// cout<<"or ayaa"<<endl;
			// cout<<tknzWHERE[1]<<endl;
			vector<bool>boolList = whereChecker(tableName, tknzWHERE[1]);
			for(int i=0; i<boolList.size(); i++){
				// cout<<"bool"<<boolList[i]<<" ";
				if(boolList[i]==1){
					for(int j=0; j<colListToDisplay.size(); j++){
						cout<<tknzRecordOfEach[i][mappingOfColAndIndex[colListToDisplay[j]]]<<" ";
					}
					cout<<endl;
				}
			}
		}
		
		else{
			// cout<<"only one condition"<<endl;
			// cout<<tknzWHERE[1]<<endl;
			vector<string>tknzCondition = tokenizer(tknzWHERE[1], '"');
			for(int i=0; i<tknzCondition.size(); i++){
				boost::algorithm::trim(tknzCondition[i]);
				// cout<<tknzCondition[i]<<endl;
			}
			// cout<<tknzCondition[1]<<" "<<tknzCondition[2]<<" "<<tknzCondition[3]<<endl;
			string conditionColumnName = tknzCondition[1];
			string conditionOperator = tknzCondition[2];
			string conditionValue = tknzCondition[3];
			int conditionIndex = mappingOfColAndIndex[conditionColumnName];

			// cout<<conditionIndex<<endl;

			//5. --> checking for operators
			if(conditionOperator == "="){
				for(int i=0; i<tknzRecordOfEach.size(); i++){
					for(int j=0; j<colListToDisplay.size(); j++){
						if(tknzRecordOfEach[i][conditionIndex] == conditionValue){
							// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
							int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
							cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
						}
					}
					cout<<endl;	    
				}
			}

			else if(conditionOperator == ">"){
				for(int i=0; i<tknzRecordOfEach.size(); i++){
					for(int j=0; j<colListToDisplay.size(); j++){
						int value = stoi(tknzRecordOfEach[i][conditionIndex]);
						int cValue = stoi(conditionValue);
						if(value > cValue){
							// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
							int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
							cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
						}
							
					}
					cout<<endl;    
				}
			}

			else if(conditionOperator == ">="){
				for(int i=0; i<tknzRecordOfEach.size(); i++){
					for(int j=0; j<colListToDisplay.size(); j++){
						int value = stoi(tknzRecordOfEach[i][conditionIndex]);
						int cValue = stoi(conditionValue);
						if(value >= cValue){
							// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
							int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
							cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
						}
							
					}
					cout<<endl;    
				}
			}

			else if(conditionOperator == "<"){
				for(int i=0; i<tknzRecordOfEach.size(); i++){
					for(int j=0; j<colListToDisplay.size(); j++){
						int value = stoi(tknzRecordOfEach[i][conditionIndex]);
						int cValue = stoi(conditionValue);
						if(value < cValue){
							// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
							int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
							cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
						}
							
					}
					cout<<endl;    
				}
			}

			else if(conditionOperator == "<="){
				for(int i=0; i<tknzRecordOfEach.size(); i++){
					for(int j=0; j<colListToDisplay.size(); j++){
						int value = stoi(tknzRecordOfEach[i][conditionIndex]);
						int cValue = stoi(conditionValue);
						if(value <= cValue){
							// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
							int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
							cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
						}
							
					}
					cout<<endl;    
				}
			}

			else if(conditionOperator == "!="){
				for(int i=0; i<tknzRecordOfEach.size(); i++){
					for(int j=0; j<colListToDisplay.size(); j++){

						if(isNumber(tknzRecordOfEach[i][conditionIndex]) && isNumber(conditionValue)){
							int value = stoi(tknzRecordOfEach[i][conditionIndex]);
							int cValue = stoi(conditionValue);
							if(value != cValue){
								// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
								int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
								cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
							}
						}
						else{
							if(tknzRecordOfEach[i][conditionIndex] != conditionValue){
								// cout<<colListToDisplay[j]<<" "<<mappingOfColAndIndex[colListToDisplay[j]]<<endl;
								int idxOfRecordToBeDisplayed = mappingOfColAndIndex[colListToDisplay[j]];
								cout<<tknzRecordOfEach[i][idxOfRecordToBeDisplayed]<<" ";
							}
						}	
					}
					cout<<endl;    
				}
			}

		}
	}
	return status;
}	