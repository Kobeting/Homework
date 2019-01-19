#include "router.h"
#include "parser.h"

#include <climits>

int main(int argc, char* argv[]){

	Parser parser;
	data_C data;
	
	parser.parser(argv[1],data);

	cout << "[Router] - Routing ... " << endl;
	
	Router router;
	router.MJL = 1;
	int best_snc;
	int min_track = INT_MAX;
	for(int snc=2; snc<data.pins.size(); snc++){
		router.SNC = snc;
		router.initialize(data);
		int track = router.channel_route(data);
		if(track < min_track){
			min_track = track;
			best_snc = snc;
		}
	}
	
	router.SNC = best_snc;
	router.initialize(data);
	router.channel_route(data);

	cout << "[Router] - Routing Complete!" << endl;
	router.output(argv[2]);
	router.dump(data);

	
}
