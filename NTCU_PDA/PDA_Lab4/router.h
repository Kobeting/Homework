#ifndef _ROUTER_H
#define _ROUTER_H

#include "parser.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

//-------------------------------------------------------------------------------------------------------//
struct Net
{
	int l;
	int r;
	bool valid;
};
//-------------------------------------------------------------------------------------------------------//
class Router
{
    public:
        void initialize(data_C data);
        int estimate_width(data_C data);
        void init_status_table(data_C data);

        int channel_route(data_C data);
        bool all_route_complete();
        bool make_connect(bool position, int pin);
        void collap_split(int column);
        void add_jog(int column);
        bool per_route_complete(int column, int pin);
        int check_status(int column, int pin);
        void widen_channel(bool position, int pin);
        void widen_previous_channel(int y);
        void output(const char* FileName);

        int MJL;
        int SNC;

        vector<int> col;
        vector<int> row;
        vector<int> row_next;
        vector<int> row_temp;
        vector<Net> nets;
        vector<vector<int> > status_table;
        vector<vector<int> > output_row;
        vector<vector<int> > output_col;
};
//-------------------------------------------------------------------------------------------------------//
#endif
