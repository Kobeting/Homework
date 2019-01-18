#ifndef _PARSER_H
#define _PARSER_H

#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

//-------------------------------------------------------------------------------------------------------//
struct column_S
{
    public:
        int t;
        int b;
};
//-------------------------------------------------------------------------------------------------------//
class data_C
{
	public:
		vector<column_S> pins;
};
//-------------------------------------------------------------------------------------------------------//
class Parser
{
    public:
        void parser(const char* FileName, data_C& data);
};
//-------------------------------------------------------------------------------------------------------//
#endif
