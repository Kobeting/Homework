#ifndef _LEGALIZE_H_
#define _LEGALIZE_H_

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <climits>
#include "rplParser.h"

//-----------------------------------------------------------------------------------------------//
struct Node_S
{
	unsigned int index;
	char nodeName[32];
    int width;  
    int height;  
    int nodeType;
    int plNodeX;
    int plNodeY;
    int plNodeFixed;
};
//-----------------------------------------------------------------------------------------------//
struct Seg_S
{
	int l;
	int h;
};
//-----------------------------------------------------------------------------------------------//
struct Row_S
{
	vector<Seg_S> free_space;
	int rowCoord;
	int rowHeight;
};
//-----------------------------------------------------------------------------------------------//
class Legalize_C
{
	public:
		int distNode(Row_S row, Node_S node, int segment);
		void legalize(rplData_S *pData);
		void initial(rplData_S *pData);
		void dump();
		void update_freespace(Row_S& row, int plNodeX, int width);
		bool check_bound(Row_S row, Node_S node);
		bool is_fit(Seg_S segment, Node_S node);
		
		vector<Row_S> Rows;
		vector<int> bug_here_delete_it_will_cause_segfail;
		vector<Node_S> plNodes;
		
};
//-----------------------------------------------------------------------------------------------//
#endif