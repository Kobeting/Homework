#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <vector>
#include <queue>
#include <boost/foreach.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#define H 0
#define V 1
#define INTERSECT_2_POINTS	// net short only if intersect more than 1 point

using namespace std;

typedef int coor_t;
typedef boost::geometry::model::d2::point_xy<coor_t> point_t;
typedef boost::geometry::model::segment<point_t> segment_t;
typedef vector<segment_t> segment_list;
typedef vector< set<int> > graph_t;

//const_expr int H = 0;
//const_expr int V = 1;

template <int N> segment_t initSegment(const coor_t same, const coor_t low, const coor_t high);


template <> segment_t initSegment<H> (const coor_t same, const coor_t low, const coor_t high)
{ return segment_t( point_t(low, same), point_t(high, same) ); }


template <> segment_t initSegment<V> (const coor_t same, const coor_t low, const coor_t high)
{ return segment_t( point_t(same, low), point_t(same, high) ); }


bool isConnectGraph(const graph_t &graph)
{
	if (graph.size() == 0) { return false; }

	const int size = graph.size();
	vector<bool> visite(size, false);
	queue<int> que;
	que.push(0);

	while(!que.empty()){
		const int node = que.front(); que.pop();
		visite[node] = true;

		for(auto iter=graph[node].begin(); iter!=graph[node].end(); ++iter){
			if(!visite[*iter]){
				visite[*iter] = true;
				que.push(*iter);
			}
		}
	}

	bool connect = true;
	for(int i=0; i<size; ++i) connect &= visite[i];
	return connect;
}


struct Net
{
	int net_name, size;
	segment_list horizontal, vertical;
	graph_t graph;

	Net(const int name) : net_name(name), size(0) {};

	void pushSegment(const int type, const coor_t same, const coor_t low, const coor_t high)
	{
		if(type == H) horizontal.push_back( initSegment<H>(same, low, high) );
		else vertical.push_back( initSegment<V>(same, low, high) );
		++size;
	}

	void constructGraph()
	{
		const int hsize = horizontal.size();
		const int vsize = vertical.size();
		this->size = hsize + vsize;
		this->graph.resize( this->size );

		for(int i=0; i<hsize; ++i){
			for(int j=i+1; j<hsize; ++j){
				if( boost::geometry::intersects( horizontal[i], horizontal[j] ) ){
					graph[i].insert(j);
					graph[j].insert(i);
				}
			}
		}

		for(int i=0; i<vsize; ++i){
			for(int j=i+1; j<vsize; ++j){
				if( boost::geometry::intersects( vertical[i], vertical[j] ) ){
					graph[hsize + i].insert(hsize + j);
					graph[hsize + j].insert(hsize + i);
				}
			}
		}

		for(int i=0; i<hsize; ++i){
			for(int j=0; j<vsize; ++j){
				if( boost::geometry::intersects( horizontal[i], vertical[j] ) ){
					graph[i].insert(hsize + j);
					graph[hsize + j].insert(i);
				}
			}
		}
	}

	inline bool isConnect() const { return isConnectGraph(this->graph); }
};


bool netShort(const Net &net1, const Net &net2)
{
	namespace bg = boost::geometry;
	for(int i=0; i<net1.horizontal.size(); ++i){
		for(int j=0; j<net2.horizontal.size(); ++j){
			if( bg::intersects( net1.horizontal[i], net2.horizontal[j] ) ){
				#ifdef INTERSECT_2_POINTS
				coor_t p[4];
				p[0] = bg::get<0, 0>(net1.horizontal[i]);
				p[1] = bg::get<1, 0>(net1.horizontal[i]);
				p[2] = bg::get<0, 0>(net2.horizontal[j]);
				p[3] = bg::get<1, 0>(net2.horizontal[j]);
				sort(p, p+4);
				if(p[1] != p[2]) return true;
				#else
				return true;
				#endif
			}
		}
	}

	for(int i=0; i<net1.vertical.size(); ++i){
		for(int j=0; j<net2.vertical.size(); ++j){
			if( bg::intersects( net1.vertical[i], net2.vertical[j] ) ){
				#ifdef INTERSECT_2_POINTS
				coor_t p[4];
				p[0] = bg::get<0, 1>(net1.vertical[i]);
				p[1] = bg::get<1, 1>(net1.vertical[i]);
				p[2] = bg::get<0, 1>(net2.vertical[j]);
				p[3] = bg::get<1, 1>(net2.vertical[j]);
				sort(p, p+4);
				if(p[1] != p[2]) return true;
				#else
				return true;
				#endif
			}
		}
	}

	return false;
}


bool pair_cmp(const pair<int, int>& lhs, const pair<int, int>& rhs)
{ return (lhs.first < rhs.first); }


bool isMatchRoutingCase(const vector<Net>& net_list, const char* casename)
{
	namespace bg = boost::geometry;

	FILE* file = fopen(casename, "r");
	if (file == nullptr) {
		fprintf(stdout, "Error: cannot open file '%s'\n", casename);
		return false;
	}

	vector<int> case_top, case_btm;
	char buf[2048];
	const char *del = " \t\r\n";
	// extract the top column
	fgets(buf, 2048, file);
	for (char* ptr = strtok(buf, del); ptr != nullptr; ptr = strtok(nullptr, del)) {
		const int signal = atoi(ptr);
		if (signal) { case_top.push_back(signal); }
	}
	// extract the bottom column
	fgets(buf, 2048, file);
	for (char* ptr = strtok(buf, del); ptr != nullptr; ptr = strtok(nullptr, del)) {
		const int signal = atoi(ptr);
		if (signal) { case_btm.push_back(signal); }
	}
	fclose(file);

	vector< pair<int, int> > top, btm;
	// First: find the y-coordinates of the topmost and the bottommost
	const int net_size = net_list.size();
	int ytop = 0, ybtm = 100;
	for (int i = 0; i < net_size; ++i) {
		const Net& net = net_list[i];
		const int seg_size = net.vertical.size();
		for (int j = 0; j < seg_size; ++j) {
			ybtm = min(ybtm, bg::get<0, 1>(net.vertical[j]));
			ytop = max(ytop, bg::get<1, 1>(net.vertical[j]));
		}
	}
	fprintf(stdout, "The top/bottom signals are at tracks %d/%d\n", ytop, ybtm);
	// Then, extract signals
	for (int i = 0; i < net_size; ++i) {
		const Net& net = net_list[i];
		const int seg_size = net.vertical.size();
		for (int j = 0; j < seg_size; ++j) {
			if (bg::get<0, 1>(net.vertical[j]) == ybtm) {
				//fprintf(stdout, "push to bottom: %d / %d\n", bg::get<0, 0>(net.vertical[j]), net.net_name);
				btm.push_back( make_pair(bg::get<0, 0>(net.vertical[j]), net.net_name) );
			}
			if (bg::get<1, 1>(net.vertical[j]) == ytop) {
				top.push_back( make_pair(bg::get<1, 0>(net.vertical[j]), net.net_name) );
			}
		}
	}
	sort(top.begin(), top.end(), pair_cmp);
	sort(btm.begin(), btm.end(), pair_cmp);
	// Finally, compare the signals with the testcase
	fprintf(stdout, "Number of signals of the case and the result at top are %d and %d respectively.\n", (int)case_top.size(), (int)top.size());
	fprintf(stdout, "Number of signals of the case and the result at bottom are %d and %d respectively.\n", (int)case_btm.size(), (int)btm.size());
	const bool top_signal_check = (case_top.size() != top.size());
	const bool btm_signal_check = (case_btm.size() != btm.size());
	if (top_signal_check) {
		fprintf(stdout, "The number of signals at the top of the routing result is different from that of the given case.\n");
	}
	if (btm_signal_check) {
		fprintf(stdout, "The number of signals at the bottom of the routing result is different from that of the given case.\n");
	}
	if (top_signal_check || btm_signal_check) { return false; }

	for (size_t i = 0; i < case_top.size(); ++i) {
		if (top[i].second != case_top[i]) {
			fprintf(stdout, "-----------------------------------------------------------------\n");
			fprintf(stdout, "Find unmatched signals at the top.\n");
			if (i == 0) { fprintf(stdout, "The 1st signal at top is expected to be %d, but the corresponding signal is %d.\n", case_top[i], top[i].second); }
			else if (i == 1) { fprintf(stdout, "The 2nd signal at top is expected to be %d, but the corresponding signal is %d.\n", case_top[i], top[i].second); }
			else { fprintf(stdout, "The %dth signal at top is expected to be %d, but the corresponding signal is %d.\n", (int)i+1, case_top[i], top[i].second); }
			fprintf(stdout, "-----------------------------------------------------------------\n");
			return false;
		}
	}
	for (size_t i = 0; i < case_btm.size(); ++i) {
		if (btm[i].second != case_btm[i]) {
			fprintf(stdout, "-----------------------------------------------------------------\n");
			fprintf(stdout, "Find unmatched signals at the bottom.\n");
			if (i == 0) { fprintf(stdout, "The 1st signal at bottom is expected to be %d, but the corresponding signal is %d.\n", case_btm[i], btm[i].second); }
			else if (i == 1) { fprintf(stdout, "The 2nd signal at bottom is expected to be %d, but the corresponding signal is %d.\n", case_btm[i], btm[i].second); }
			else { fprintf(stdout, "The %dth signal at bottom is expected to be %d, but the corresponding signal is %d.\n", (int)i+1, case_btm[i], btm[i].second); }
			fprintf(stdout, "-----------------------------------------------------------------\n");
			return false;
		}
	}

	return true;
}


int main(const int argc, const char *argv[])
{
	if (argc < 3) {
		fprintf(stdout, "Usage: %s [routing result] [routing case]\n", argv[0]);
		return 0;
	}

	set<coor_t> y_coor;
	vector<Net> net_list;
	FILE *file = fopen(argv[1], "r");

	if (file == nullptr) {
		fprintf(stdout, "Error: cannot open file '%s'\n", argv[1]);
		fprintf(stdout, "Program exit...\n");
		return 0;
	}

	fprintf(stdout, "---------- start to parse input ----------\n");

	int net_name, line_no = 0;
	while(fscanf(file, ".begin %d\n", &net_name) == 1) {
		++line_no;
		net_list.push_back(Net(net_name));
		Net &net = net_list.back();
		fprintf(stdout, "Start to parse net %d...\n", net_name);

		char buf[100];
		while( (fgets(buf, 100, file)!=NULL) && strncmp(buf, ".end", 4) ) {
			++line_no;
			char type;
			coor_t a, b, c;
			if (sscanf(buf, ".%c %d %d %d", &type, &a, &b, &c) != 4) {
				fprintf(stdout, "Format error at line %d\n", line_no);
				return 0;
			}

			if(type == 'H'){
				if (c <= a) {
					fprintf(stdout, "Format error: invalid wire .%c %d %d %d\n", type, a, b, c);
					return 0;
				}
				net.pushSegment(H, b, a, c);
				y_coor.insert(b);
			}
			else{
				if (c <= b) {
					fprintf(stdout, "Format error: invalid wire .%c %d %d %d\n", type, a, b, c);
					return 0;
				}
				net.pushSegment(V, a, b, c);
				y_coor.insert(b);
				y_coor.insert(c);
			}
		}

		if (net.size == 0) {
			fprintf(stdout, "net %d is empty.\n", net_name);
		}
	}

	fclose(file);
	fprintf(stdout, "---------- parse input complete ----------\n");

	fprintf(stdout, "\n");

	fprintf(stdout, "---------- Open net checking ----------\n");
	const int net_size = net_list.size();
	bool routing_pass = true;
	for(int i=0; i<net_size; ++i){
		Net &net = net_list[i];
		net.constructGraph();
		if(!net.isConnect()) {
			printf("Net %d is open!\n", net.net_name);
			routing_pass = false;
		}
	}
	fprintf(stdout, "---------- Open net checking complete ----------\n");

	fprintf(stdout, "\n");

	fprintf(stdout, "---------- Short net checking ----------\n");
	for(int i=0; i<net_size; ++i){
		for(int j=i+1; j<net_size; ++j){
			if( netShort( net_list[i], net_list[j] ) ) {
				printf("Net %d and %d are short!\n", net_list[i].net_name, net_list[j].net_name);
				routing_pass = false;
			}
		}
	}
	fprintf(stdout, "---------- Short net checking complete ----------\n");

	fprintf(stdout, "\n");

	const bool complete_routing = isMatchRoutingCase(net_list, argv[2]);

	fprintf(stdout, "\n");
	// does not account top and bottom layer
	fprintf(stdout, "---------- Status Report ----------\n");
	fprintf(stdout, "track count: %lu\n", y_coor.size()-2u);
	if (!complete_routing) {
		fprintf(stdout, "Exist signal that is not connected\n");
	}

	if (routing_pass && complete_routing) {
		fprintf(stdout, "All signals are connected successfully.\n");
	}
	else {
		fprintf(stdout, "Routing failed. Please check the error messages.\n");
	}
	fprintf(stdout, "-----------------------------------\n");

	return 0;
}
