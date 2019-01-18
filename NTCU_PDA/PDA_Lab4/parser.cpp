#include "parser.h"

using namespace std;

//-------------------------------------------------------------------------------------------------------//
void Parser::parser(const char* FileName, data_C& data){
	ifstream input;
    input.open(FileName, ios::in);

	if (!input){
        cout << "File " << FileName << " Can Not Be Opened! Exit" << endl;
        return;
    }
    else{
    	cout << "[Parser] - File " << FileName << " Open Success." << endl;
        cout << "[Parser] - Parsing ... " << endl;
    }

    stringstream ss;
    string line, word;
    getline(input,line);

    ss.str("");
    ss.clear();
    ss << line;
    while(1){
    	ss >> word;
    	if(ss.fail()) break;
    	column_S column;
        istringstream is(word);
    	is >> column.t;
    	data.pins.push_back(column);
    }

    getline(input,line);

    ss.str("");
    ss.clear();
    ss << line;
    int ColumnIter = 0;
    while(1){
    	ss >> word;
    	if(ss.fail()) break;
        istringstream is(word);
    	is >> data.pins[ColumnIter++].b;
    }

    // for(int i=0; i<data.pins.size(); i++)
    // 	cout << data.pins[i].t << ";";
    // cout << endl;
    // for(int i=0; i<data.pins.size(); i++)
    // 	cout << data.pins[i].b << ";";
    cout << "[Parser] - Parsing Complete!" << endl;
}
//-------------------------------------------------------------------------------------------------------//
