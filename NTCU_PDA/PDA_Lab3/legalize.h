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

struct Cluster_S
{
	vector<int> nodes;
	unsigned int n_first;
	unsigned int n_last;
	unsigned int number;
	unsigned int row;
	int x;
	int e;
	int q;
	int w;

	//Cluster_S() : n_first(0), n_last(0), number(0), x(0), e(0), q(0), w(0) {}
};
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
struct Row_S
{
	vector<Cluster_S> clusters;
	int space;
	int rowMin;
	int rowMax;
	int rowCoord;
	int rowHeight;
};
//-----------------------------------------------------------------------------------------------//
class Legalize_C
{
	public:
		int place_row(unsigned int r, unsigned int nNodeIter);
		void place_row_final(unsigned int r, unsigned int nNodeIter);
		void legalize(rplData_S *pData);
		void initial(rplData_S *pData);
		void init_cluster(Cluster_S& cluster);
		void add_cell(Cluster_S& cluster, unsigned int i);
		void add_cluster(unsigned int r);
		void collapse(unsigned int r);
		void dump();
		void slice_row(unsigned int r, Node_S node);
		bool check_overlap(Row_S row, Node_S node);
		int find_row(Node_S node);
		bool space_cheak(Node_S node, unsigned int r);
		int safty_add(unsigned int r, unsigned int nNodeIter);
	
		vector<Row_S> Rows;
		vector<int> bug_here_delete_it_will_cause_segfail;
		vector<Node_S> plNodes;

		bool debug;
		
};
//-----------------------------------------------------------------------------------------------//
#endif