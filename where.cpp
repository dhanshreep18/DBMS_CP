#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>

#include "Header Files/tokenizer.h"

using namespace std;


// checks for a single condition for eg id = 1661, and returns true or false
bool conditionChecker(string condition, vector<string>tokenizedRecord, unordered_map<string, int> conditionMap){
	vector<string>tknzCondition = tokenizer(condition, '"');
	for(int i=0; i<tknzCondition.size(); i++){
		boost::algorithm::trim(tknzCondition[i]);
	}
	string conditionColumnName = tknzCondition[1];
	string conditionOperator = tknzCondition[2];
	string conditionValue = tknzCondition[3];

	if(conditionOperator == "="){
		if(tokenizedRecord[conditionMap[conditionColumnName]] == conditionValue) return true;
		else return false;
	}else if(conditionOperator == "!="){
		if(tokenizedRecord[conditionMap[conditionColumnName]] != conditionValue) return true;
		else return false;
	}else if(conditionOperator == ">"){
		if(stoi(tokenizedRecord[conditionMap[conditionColumnName]]) > stoi(conditionValue)) return true;
		else return false;
	}else if(conditionOperator == "<"){
		if(stoi(tokenizedRecord[conditionMap[conditionColumnName]]) < stoi(conditionValue)) return true;
		else return false;
	}else if(conditionOperator == ">="){
		if(stoi(tokenizedRecord[conditionMap[conditionColumnName]]) >= stoi(conditionValue)) return true;
		else return false;
	}else if(conditionOperator == "<="){
		if(stoi(tokenizedRecord[conditionMap[conditionColumnName]]) <= stoi(conditionValue)) return true;
		else return false;
	}
	return false;
}


/* sample input:
whereChecker("Students", "id = 111 AND name = 'yo yo'")
*/
vector<bool> whereChecker(string tableName, string conditionList){
	vector<bool> tableCheck;

	//1. --> Getting records from table file
	string records;
	string recordsFileName = "RELATIONS/"+tableName+".txt";
	ifstream RecordsFile(recordsFileName);
	
	if (!RecordsFile.is_open()) {
		cerr << "Could not open the table file"<< endl;
		exit(EXIT_FAILURE);
	}
	records = string((std::istreambuf_iterator<char>(RecordsFile)), std::istreambuf_iterator<char>());

	vector<string>recordOfEach = tokenizer(records, '\n');
	vector<vector<string>>tknzRecordOfEach;

	for(int i = 0; i < recordOfEach.size(); i++){
		// cout<<recordOfEach[i]<<endl;
		vector<string>temp = tokenizer(recordOfEach[i], '#');
		tknzRecordOfEach.push_back(temp);
	}

	// -------------------------------------------
	//2. --> Mapping of columns in table & index
	string schemaData;
	ifstream SchemaFile("DATABASE FILES/Schema.txt");
	
    if (!SchemaFile.is_open()) {
        cerr << "Could not open the Schema file - '"<< endl;
        exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
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

	unordered_map<string, int> columnNames;

	for(int i = 0; i < metaDataTknzd.size()/3; i++){
		columnNames.insert(pair<string ,int>(metaDataTknzd[3 * i + 1], i));
	}

	// -------------------------------------------
	//3. --> setting bool values

	if(boost::algorithm::contains(conditionList, "AND")){
		vector<string> conditionListTokens = tokenizer(conditionList, "AND");
		for(int r = 0; r < tknzRecordOfEach.size(); r++)
		{
			bool temp = true;
			for(int i = 0; i < conditionListTokens.size(); i++){
				temp = temp && conditionChecker(conditionListTokens[i], tknzRecordOfEach[r], columnNames);
			}
			tableCheck.push_back(temp);
		}

	}else if(boost::algorithm::contains(conditionList, "OR")){
		vector<string> conditionListTokens = tokenizer(conditionList, "OR");
		for(int r = 0; r < tknzRecordOfEach.size(); r++)
		{
			bool temp = false;
			for(int i = 0; i < conditionListTokens.size(); i++){
				temp = temp || conditionChecker(conditionListTokens[i], tknzRecordOfEach[r], columnNames);
			}
			tableCheck.push_back(temp);
		}
	}else{
		for(int r = 0; r < tknzRecordOfEach.size(); r++)
		{
			tableCheck.push_back(conditionChecker(conditionList, tknzRecordOfEach[r], columnNames));
		}
	}
	 
	// for(int i = 0; i < tableCheck.size(); i ++){
	// 	cout << tableCheck[i] << endl;
	// }
	return tableCheck;
}