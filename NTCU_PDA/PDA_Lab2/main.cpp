#include "floorplan.h"

using namespace std;

int main(int argc, char** argv){

	Parser parser;
	data_C data;
	
	parser.parser(argv[2],argv[3],data);
	
	Floorplan floorplan;

	float alpha = atof(argv[1]);
	floorplan.SM(data,alpha,argv[4]);
}