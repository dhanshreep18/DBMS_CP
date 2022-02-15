/*
  DBMS Course Project ==> Mini Database Management System
  Group TY1 CS_A:
  Anubhav Pabby -> 14
  Anjali Asolkar -> 19
  Avinash Vijayvargiya -> 20
  Dhanshree Pajankar -> 62
*/

#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "Header Files/tokenizer.h"
#include "Header Files/createTable.h"
#include "Header Files/insert.h"
#include "Header Files/update.h"
#include "Header Files/select.h"
#include "Header Files/describe.h"
#include "Header Files/help.h"
#include "Header Files/delete.h" 
#include "Header Files/drop.h"

using namespace std;

int main() {

  ios_base::sync_with_stdio(false);	

  unordered_map<string, int>mapping;
  mapping["CREATE"]=1;
  mapping["SELECT"]=2;
  mapping["DROP"]=3;
  mapping["DESCRIBE"]=4;
  mapping["INSERT"]=5;
  mapping["DELETE"]=6;
  mapping["UPDATE"]=7;
  mapping["HELP"]=8;
  mapping["QUIT"]=9;

  string command;

  while(1){
    cout << "DBMS_CP > ";
    getline(cin, command);
    //tokenizing command
    vector <string> tokens = tokenizer(command, ' ');

	  //cout << tokens[0] << " " << mapping[tokens[0]] << endl;

    //tokenized string is passed to mapping
    int command_identifier = mapping[tokens[0]];
    
    switch(command_identifier){
      case 0: //call create function
	  		cout<<"COMMAND NOT FOUND!";
            break;
      case 1: //call create function
			{
				// cout << "create token has been found";
				createTable(command);
				break;
			}
      case 2: //call select function
	  		{
				selectFromTable(command);
				break;
			}
              
      case 3: //call drop function
	  		dropTable(command);
            break;
      case 4: //call describe function
	  		{
				describeTable(command);
				break;
			}
    
      case 5: //call insert function
              {   insertInTable(command);
				  break;
			  }
      case 6: //call delete function
            deleteRecord(command);
            break;
      case 7: //call update function
            {   if(updateInTable(command)){
		           cout<<"Record updated successfully!"<<endl;
	            }
		        else{
			       cout<<"Record not updated !"<<endl;
		        }
		    break;
	  }
      case 8: //call help function
	  		{
				help(command);
				break;
			}
            
      case 9: //call exit function
              exit(0);
              break;                                                    
      default: cout<<"COMMAND NOT FOUND!";
               break;             
    }

    cout << endl;
  }
}