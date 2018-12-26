#include "parser.h"
#include <algorithm>
#include <math.h>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

//-------------------------------------------------------------------------------------------------------//
struct Node_S
{
	public:
		int visited;
		int index;
		int height;
		int width;
		int x;
		int y;
		int r_child;
		int l_child;
		int parent;
};
//-------------------------------------------------------------------------------------------------------//
struct Btree_S
{
	public:
		vector<Node_S> node;
		int root;

        int chip_width;
        int chip_height;
};
//-------------------------------------------------------------------------------------------------------//
struct Coor_S
{
	public:
		int x;
		int y;
};
//-------------------------------------------------------------------------------------------------------//
class Floorplan
{
	public:
		Btree_S b_tree;

		void SM(data_C data, double alpha, const char* NetFileName);
		int area(Btree_S& b_tree);
		int HPWL(Btree_S b_tree, data_C data);
		int cost(Btree_S& b_tree, data_C data, double alpha);
		bool check_visited(Btree_S b_tree, int i);
		Btree_S rotate(Btree_S b_tree, int i);
		Btree_S move(Btree_S b_tree, int i, int j);
		Btree_S swap(Btree_S b_tree, int i, int j);
		void dump(Btree_S& b_tree, data_C data, const char* NetFileName, double alpha, clock_t t);
};
//-------------------------------------------------------------------------------------------------------//