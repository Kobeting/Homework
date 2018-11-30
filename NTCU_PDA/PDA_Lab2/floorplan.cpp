#include "floorplan.h"

using namespace std;

//-------------------------------------------------------------------------------------------------------//
void Floorplan::SM(data_C data, double alpha, const char* NetFileName){

	clock_t t;
  	t = clock();

	b_tree.node.resize(data.block_vectors.size());
	for(int i=0; i<data.block_vectors.size(); i++){
		b_tree.node[i].index = i;
		b_tree.node[i].width = data.blocks[i].x;
		b_tree.node[i].height = data.blocks[i].y;
		b_tree.node[i].x = 0;
		b_tree.node[i].y = 0;
	}

	for(int i=0; i<b_tree.node.size(); i++){
		if((i+1)*2-1 < b_tree.node.size())
			b_tree.node[i].l_child = (i+1)*2-1;
		else b_tree.node[i].l_child = -1;
		if((i+1)*2 < b_tree.node.size())
			b_tree.node[i].r_child = (i+1)*2;
		else b_tree.node[i].r_child = -1;
		if((i+1)/2 > 0)
			b_tree.node[i].parent = (i+1)/2-1;
		else b_tree.node[i].parent = -1;
	}

	b_tree.root = 0;
	//initial solution of b*tree

	Btree_S new_tree, best_tree;
	best_tree = b_tree;
	int num = b_tree.node.size();
	int Move;
	double ran;
	int MT = 0;
	int N = data.block_vectors.size() * k;
	int T = 111111111;
	int uphill = 0;
	int reject = 0;
	int cost_delta;
	int b_i, b_j;
	srand(2);

	while(1){
		MT = 0;
		uphill = 0;
		reject = 0;
		while(1){
			Move = rand()%3 + 1;
			b_i = rand() % num;
			b_j = rand() % num;
			while(b_i == b_j) b_j = rand() % num;
			switch(Move){
				case 1:{
					// cout << "case1" << endl;
					// cout << b_i << endl;
					new_tree = rotate(b_tree,b_i);
					break;
				}
				case 2:{
					// cout << "case2" << endl;
					// cout << b_i << ":" << b_j << endl;
					new_tree = move(b_tree,b_i,b_j);
					break;
				}
				default:{
					// cout << "case3" << endl;
					// cout << b_i << ":" << b_j << endl;
					new_tree = swap(b_tree,b_i,b_j);
					break;
				}
			}
			MT += 1;
			cost_delta = cost(new_tree,data,alpha) - cost(b_tree,data,alpha);
			ran = (double)rand() / (RAND_MAX + 1.0);
			if(!(cost_delta > 0) || ran < (exp(-(double)cost_delta/T))){
				if(cost_delta > 0)
					uphill += 1;
				b_tree = new_tree;		
				if(cost(b_tree,data,alpha) < cost(best_tree,data,alpha))
					best_tree = b_tree;
			}
			else{
				reject += 1;
			}
			if(uphill > N || MT > 2*N) break;
		}
		T *= reduce;
		if((double)(reject/MT) > 0.95 || T < epsilon) break;
	}
	//Simulated Annealing process

	t = clock() - t;
	dump(best_tree,data,NetFileName,alpha,t);

	return;
}
//-------------------------------------------------------------------------------------------------------//
bool Floorplan::check_visited(Btree_S b_tree, int i){
	if(i == -1) return 0;
	if(b_tree.node[i].visited >= 1) return 0;
	else return 1;
}
//-------------------------------------------------------------------------------------------------------//
int Floorplan::area(Btree_S& b_tree){
	
	for(int i=0; i<b_tree.node.size(); i++)
		b_tree.node[i].visited = 0;
	int x = 0;
	int r = b_tree.root;
	while(r != -1){
		// if(!b_tree.node[r].visited){
		// 	cout << b_tree.node[r].index << "  parent:" << b_tree.node[r].parent << "  left:" << b_tree.node[r].l_child << "  right" << b_tree.node[r].r_child << endl;
		// } //
		 
		b_tree.node[r].visited += 1;
		if(b_tree.node[r].l_child == -1 && b_tree.node[r].r_child == -1){
			b_tree.node[r].x = x;
			// cout << b_tree.node[r].index << "   width:" << b_tree.node[r].width << "    x:" << b_tree.node[r].x << "    :" << x << endl;
			if(b_tree.node[b_tree.node[r].parent].l_child == r) x -= b_tree.node[b_tree.node[r].parent].width;
			r = b_tree.node[r].parent;
		}
		else if(check_visited(b_tree,b_tree.node[r].l_child)){
			x += b_tree.node[r].width;
			r = b_tree.node[r].l_child;
		}
		else if(check_visited(b_tree,b_tree.node[r].r_child)){
			// if(b_tree.node[r].visited == 2) x -= b_tree.node[r].width;
			r = b_tree.node[r].r_child;
		}
		else{
			b_tree.node[r].x = x;
			// cout << b_tree.node[r].index << "   width:" << b_tree.node[r].width << "    x:" << b_tree.node[r].x << "    :" << x << endl;
			if(b_tree.node[b_tree.node[r].parent].l_child == r) x -= b_tree.node[b_tree.node[r].parent].width;
			r = b_tree.node[r].parent;
		}
	}
	//update coordinate x

	vector<Coor_S> C;
	Coor_S coor;
	coor.x = 0;
	coor.y = 0;
	C.push_back(coor);
	coor.x = INT_MAX;
	coor.y = 0;
	C.push_back(coor);

	for(int i=0; i<b_tree.node.size(); i++)
		b_tree.node[i].visited = 0;

	r = b_tree.root;
	while(r != -1){
		if(!b_tree.node[r].visited){
			int max_y;
			int i = 0;
			while(C[i].x <= b_tree.node[r].x) i++;
			max_y = C[i].y;
			while(C[i].x < (b_tree.node[r].x + b_tree.node[r].width)){
				if(max_y < C[i].y) max_y = C[i].y;
				i++;
			}
			b_tree.node[r].y = max_y;
			//maintain the upper y line
			Coor_S coor1, coor2, coor3, coor4;
			coor1.x = b_tree.node[r].x;
			coor1.y = b_tree.node[r].y;
			coor2.x = b_tree.node[r].x;
			coor2.y = b_tree.node[r].y + b_tree.node[r].height;
			coor3.x = b_tree.node[r].x + b_tree.node[r].width;
			coor3.y = b_tree.node[r].y + b_tree.node[r].height;
			coor4.x = b_tree.node[r].x + b_tree.node[r].width;
			coor4.y = b_tree.node[r].y;

			int j = 0;
			while(C[j].x < b_tree.node[r].x) j++;
			if(C[j].x == b_tree.node[r].x && C[j+1].x == b_tree.node[r].x){ 
				C.erase(C.begin() + j+1);
			}
			else{
				C.insert(C.begin()+  j,coor1);
			}
			C.insert(C.begin() + ++j,coor2);
			C.insert(C.begin() + ++j,coor3);
			j++;
			while(C[j].x < (b_tree.node[r].x + b_tree.node[r].width)) C.erase(C.begin() + j);
			if(C[j].x == (b_tree.node[r].x + b_tree.node[r].width)){
				C.erase(C.begin() + j);
			}
			else{
				if(C[j].y == coor4.y)
					C.insert(C.begin() + j,coor4);
				else{
					coor4.y = C[j].y;
					C.insert(C.begin() + j,coor4);
				}
			}
			// for(int i=0; i<C.size(); i++)
			// 	cout << "(" << C[i].x << "," << C[i].y << ")";
			// cout << endl;
		}
		b_tree.node[r].visited = 1;
		if(b_tree.node[r].l_child == -1 && b_tree.node[r].r_child == -1) r = b_tree.node[r].parent;
		else if(check_visited(b_tree,b_tree.node[r].l_child)) r = b_tree.node[r].l_child;
		else if(check_visited(b_tree,b_tree.node[r].r_child)) r = b_tree.node[r].r_child;
		else r = b_tree.node[r].parent;
	}
	//update coordinate y

	// for(int i=0; i<b_tree.node.size(); i++)
	// 	cout << "(" << b_tree.node[i].x << "," << b_tree.node[i].y << ")" << endl;
	//output the coordinate(x,y)

	int max_x = 0;
	int max_y = 0;
	for(int i=0; i<b_tree.node.size(); i++){
		max_x = max(max_x,b_tree.node[i].x + b_tree.node[i].width);
		max_y = max(max_y,b_tree.node[i].y + b_tree.node[i].height);
	}
	b_tree.chip_width = max_x;
	b_tree.chip_height = max_y;
	// cout << max_x << endl;
	// cout << max_y << endl;
	return max_x * max_y;
}
//-------------------------------------------------------------------------------------------------------//
int Floorplan::HPWL(Btree_S b_tree, data_C data){
	int result = 0;
	for(int j=0; j<data.nets.size(); j++){
		int min_x = INT_MAX;
		int min_y = INT_MAX;
		int max_x = 0;
		int max_y = 0;
		for(int i=0; i<data.nets[j].net.size(); i++){
			if(data.nets[j].net[i]){
				if(i < data.block_vectors.size()){
					min_x = min(min_x,b_tree.node[i].x + b_tree.node[i].width/2);
					min_y = min(min_y,b_tree.node[i].y + b_tree.node[i].height/2);
					max_x = max(max_x,b_tree.node[i].x + b_tree.node[i].width/2);
					max_y = max(max_y,b_tree.node[i].y + b_tree.node[i].height/2);
				}
				else{
					min_x = min(min_x,data.termianls[i-data.block_vectors.size()].x);
					min_y = min(min_y,data.termianls[i-data.block_vectors.size()].y);
					max_x = max(max_x,data.termianls[i-data.block_vectors.size()].x);
					max_y = max(max_y,data.termianls[i-data.block_vectors.size()].y);
				}
			}
		}
		result += (max_x - min_x) + (max_y - min_y);

		// for(int i=0; i <n.net.size(); i++)
		// 	cout << n.net[i];
		// cout << (max_x - min_x) + (max_y - min_y) << endl;
	}
	return result;
}
//-------------------------------------------------------------------------------------------------------//
int Floorplan::cost(Btree_S& b_tree, data_C data, double alpha){
	return alpha * area(b_tree) + (1 - alpha) * HPWL(b_tree,data);
}
//-------------------------------------------------------------------------------------------------------//
Btree_S Floorplan::rotate(Btree_S b_tree, int i){
	Btree_S new_tree = b_tree;
	new_tree.node[i].width = b_tree.node[i].height;
	new_tree.node[i].height = b_tree.node[i].width;
	return new_tree;
}
//-------------------------------------------------------------------------------------------------------//
Btree_S Floorplan::move(Btree_S b_tree, int i, int j){
	Btree_S new_tree = b_tree;
	while(new_tree.node[i].l_child != -1 || new_tree.node[i].r_child != -1){
		while(new_tree.node[i].l_child != -1){
			new_tree = swap(new_tree,i,new_tree.node[i].l_child);
		}
		if(new_tree.node[i].r_child != -1)
			new_tree = swap(new_tree,i,new_tree.node[i].r_child);
	}//swap node_i to the bottom
	
	if(new_tree.node[new_tree.node[i].parent].l_child != -1){
		if(new_tree.node[new_tree.node[i].parent].l_child == i)
			new_tree.node[new_tree.node[i].parent].l_child = -1;
		else
			new_tree.node[new_tree.node[i].parent].r_child = -1;
	}
	else
		new_tree.node[new_tree.node[i].parent].r_child = -1;
	//cut node_i connection with its parent
	
	int l = j;
	while(new_tree.node[l].l_child != -1)
		l = new_tree.node[l].l_child;	
	if(i != l){
		new_tree.node[l].l_child = i;
		new_tree.node[i].parent = l;
	}
	//find the left bottom insert position
	while(new_tree.node[i].parent != j){
		new_tree = swap(new_tree,i,new_tree.node[i].parent);
	}
	new_tree = swap(new_tree,i,new_tree.node[i].parent);
	//swap node_i up to insert position j
	return new_tree;
}
//-------------------------------------------------------------------------------------------------------//
Btree_S Floorplan::swap(Btree_S b_tree, int i, int j){
	Btree_S new_tree = b_tree;
	if(b_tree.node[j].parent == i){
		new_tree.node[i].r_child = b_tree.node[j].r_child;
		new_tree.node[i].l_child = b_tree.node[j].l_child;
		new_tree.node[i].parent = j;
		new_tree.node[j].parent = b_tree.node[i].parent;
		if(b_tree.node[i].l_child == j){
			new_tree.node[j].l_child = i;
			new_tree.node[j].r_child = b_tree.node[i].r_child;
			if(b_tree.node[i].r_child != -1)
				new_tree.node[b_tree.node[i].r_child].parent = j;
		}
		else{
			new_tree.node[j].l_child = b_tree.node[i].l_child;
			new_tree.node[j].r_child = i;
			if(b_tree.node[i].l_child != -1)
				new_tree.node[b_tree.node[i].l_child].parent = j;
		}

		if(b_tree.node[i].parent != -1){
			if(b_tree.node[b_tree.node[i].parent].l_child == i)
				new_tree.node[b_tree.node[i].parent].l_child = j;
			else
				new_tree.node[b_tree.node[i].parent].r_child = j;
		}
		if(b_tree.node[j].l_child != -1)
			new_tree.node[b_tree.node[j].l_child].parent = i;
		if(b_tree.node[j].r_child != -1)
			new_tree.node[b_tree.node[j].r_child].parent = i;
	}
	else if(b_tree.node[i].parent == j){
		new_tree.node[j].r_child = b_tree.node[i].r_child;
		new_tree.node[j].l_child = b_tree.node[i].l_child;
		new_tree.node[j].parent = i;
		new_tree.node[i].parent = b_tree.node[j].parent;
		if(b_tree.node[j].l_child == i){
			new_tree.node[i].l_child = j;
			new_tree.node[i].r_child = b_tree.node[j].r_child;
			if(b_tree.node[j].r_child != -1)
				new_tree.node[b_tree.node[j].r_child].parent = i;
		}
		else{
			new_tree.node[i].l_child = b_tree.node[j].l_child;
			new_tree.node[i].r_child = j;
			if(b_tree.node[j].l_child != -1)
				new_tree.node[b_tree.node[j].l_child].parent = i;
		}

		if(b_tree.node[j].parent != -1){
			if(b_tree.node[b_tree.node[j].parent].l_child == j)
				new_tree.node[b_tree.node[j].parent].l_child = i;
			else
				new_tree.node[b_tree.node[j].parent].r_child = i;
		}
		if(b_tree.node[i].l_child != -1)
			new_tree.node[b_tree.node[i].l_child].parent = j;
		if(b_tree.node[i].r_child != -1)
			new_tree.node[b_tree.node[i].r_child].parent = j;
	}
	else{
		new_tree.node[i].r_child = b_tree.node[j].r_child;
		new_tree.node[i].l_child = b_tree.node[j].l_child;
		new_tree.node[i].parent = b_tree.node[j].parent;
		new_tree.node[j].r_child = b_tree.node[i].r_child;
		new_tree.node[j].l_child = b_tree.node[i].l_child;
		new_tree.node[j].parent = b_tree.node[i].parent;

		if(b_tree.node[i].parent != -1){
			if(b_tree.node[b_tree.node[i].parent].l_child == i)
				new_tree.node[b_tree.node[i].parent].l_child = j;
			else
				new_tree.node[b_tree.node[i].parent].r_child = j;
		}
		if(b_tree.node[j].parent != -1){
			if(b_tree.node[b_tree.node[j].parent].l_child == j)
				new_tree.node[b_tree.node[j].parent].l_child = i;
			else
				new_tree.node[b_tree.node[j].parent].r_child = i;
		}

		if(b_tree.node[i].l_child != -1)
			new_tree.node[b_tree.node[i].l_child].parent = j;
		if(b_tree.node[i].r_child != -1)
			new_tree.node[b_tree.node[i].r_child].parent = j;

		if(b_tree.node[j].l_child != -1)
			new_tree.node[b_tree.node[j].l_child].parent = i;
		if(b_tree.node[j].r_child != -1)
			new_tree.node[b_tree.node[j].r_child].parent = i;
	}
	
	if(b_tree.root == i || b_tree.root == j)
		new_tree.root = (b_tree.root == i) ? j :i;
	return new_tree;
}
//-------------------------------------------------------------------------------------------------------//
void Floorplan::dump(Btree_S& b_tree, data_C data, const char* NetFileName, double alpha, clock_t t){
	ofstream output;
    output.open(NetFileName);
    output << cost(b_tree,data,alpha) << endl;
    output << HPWL(b_tree,data) << endl;
    output << area(b_tree) << endl;
    output << b_tree.chip_width << " " << b_tree.chip_height << endl;
    output << ((float)t)/CLOCKS_PER_SEC << endl;
    
    Node_S* node = new Node_S;
    for(int i=0; i<b_tree.node.size(); i++){
    	output << data.name_vectors[i] << " " << b_tree.node[i].x << " " << b_tree.node[i].y << " " << b_tree.node[i].x + b_tree.node[i].width << " " << b_tree.node[i].y + b_tree.node[i].height << endl;
    }
	return;
}
//-------------------------------------------------------------------------------------------------------//