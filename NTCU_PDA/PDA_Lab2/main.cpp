#include "floorplan.h"

using namespace std;

int main(int argc, char** argv){

	Parser parser;
	data_C data;
	
	parser.parser(argv[1],argv[2],data);
	
	Floorplan floorplan;

	float alpha = atof(argv[3]);
	floorplan.S_M(data,alpha);

	int a;
    
	cin >> a;
}