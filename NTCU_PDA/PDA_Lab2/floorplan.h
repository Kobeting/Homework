#include "parser.h"
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

//-------------------------------------------------------------------------------------------------------//
struct Polish_S
{
	public:
		bool op;
		bool cut_dir;
		bool rotation;
		int min_space;
		int height;
		int width;
};
//-------------------------------------------------------------------------------------------------------//
class Floorplan
{
	public:
		vector<Polish_S> polish_exp;

		void S_M(data_C data, float alpha);
		void swap(vector<Polish_S>& polish_exp, int index);
		int cost(vector<Polish_S> &polish_exp, vector<net_S> nets, float alpha);
};
//-------------------------------------------------------------------------------------------------------//