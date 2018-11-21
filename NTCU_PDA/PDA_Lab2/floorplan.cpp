#include "floorplan.h"

using namespace std;

//-------------------------------------------------------------------------------------------------------//
void Floorplan::S_M(data_C data, float alpha){
	
	polish_exp.resize(data.block_vectors.size()*2-1);
	int j=0;
	for(int i=0; i<polish_exp.size(); i++){
		if(i == 0 || i%2 == 1){
			polish_exp[i].op = 0;
			polish_exp[i].rotation = 1;
			polish_exp[i].min_space = -1;
			polish_exp[i].height = data.blocks[j].y;
			polish_exp[i].width = data.blocks[j].x;
			j++;
		}
		else{
			polish_exp[i].op = 1;
			polish_exp[i].cut_dir = 0;
		}
	}
	cout << cost(polish_exp,data.nets,alpha);
	//initial solution of polish_expression

	// while(1){
	// 	int MT = 0;
	// 	int uphill = 0;
	// 	int reject = 0;
	// 	float cost_bef = cost(polish_exp,data.nets,alpha);

	// 	srand(5);
	// 	int x = rand() %2 +1;
	// 	switch(x){
	// 		case '1':
	// 			int y; 
	// 			y = rand() % data.block_vectors.size();
	// 			y = y*2;
	// 			swap(polish_exp,y);

	// 			break;
	// 		case '2':
	// 			break;
	// 		case '3':
	// 			break;
	// 	}

	// }



	// for(int i=0; i<polish_exp.size(); i++){
	// 	cout << polish_exp[i] << endl;
	// }
	return;
}
//-------------------------------------------------------------------------------------------------------//
int Floorplan::cost(vector<Polish_S> &polish_exp, vector<net_S> nets, float alpha){
	vector<Polish_S> stack_p;
	int i = 0;
	while(i != polish_exp.size()){
		if(polish_exp[i].op == 0){
			stack_p.push_back(polish_exp[i]);
		}
		else{
			int height, width, area_min, area_temp;
			Polish_S operon1 = stack_p[stack_p.size()-1];
			Polish_S operon2 = stack_p[stack_p.size()-2];
			Polish_S npolish;
			if(polish_exp[i].cut_dir == 0){
				area_min = (operon1.width + operon2.width) * max(operon1.height,operon2.height);
				// cout << "operon1.width" << operon1.width;
				// cout << "operon2.width" << operon2.width;
				// cout << "operon1.height" << operon1.height;
				// cout << "operon2.height" << operon2.height;
				// cout << "1:"<< area_min;
				npolish.width = operon1.width + operon2.width;
				npolish.height = max(operon1.height,operon2.height);
				if(operon1.rotation == 1){
					area_temp = (operon1.height + operon2.width) * max(operon1.width,operon2.height);
					if(area_min > area_temp){
						area_min = area_temp;
						npolish.width = operon1.height + operon2.width;
						npolish.height = max(operon1.width,operon2.height);
					}
				}
				if(operon2.rotation == 1){
					area_temp = (operon1.width + operon2.height) * max(operon1.height,operon2.width);
					if(area_min > area_temp){
						area_min = area_temp;
						npolish.width = operon1.width + operon2.height;
						npolish.height = max(operon1.height,operon2.width);
					}
				}
				if(operon1.rotation == 1 && operon2.rotation == 1){
					area_temp = (operon1.height + operon2.height) * max(operon1.width,operon2.width);
					if(area_min > area_temp){
						area_min = area_temp;
						npolish.width = operon1.height + operon2.height;
						npolish.height = max(operon1.width,operon2.width);
					}
				}
			}
			else{
				area_min = (operon1.height + operon2.height) * max(operon1.width,operon2.width);
				npolish.width = max(operon1.width,operon2.width);
				npolish.height = operon1.height + operon2.height;
				if(operon1.rotation == 1){
					area_temp = (operon1.width + operon2.height) * max(operon1.height,operon2.width);
					if(area_min > area_temp){
						npolish.width = max(operon1.height,operon2.width);
						npolish.height = operon1.width + operon2.height;
					}
				}
				if(operon2.rotation == 1){
					area_temp = (operon1.height + operon2.width) * max(operon1.width,operon2.height);
					if(area_min > area_temp){
						npolish.width = max(operon1.width,operon2.height);
						npolish.height = operon1.height + operon2.width;
					}
				}
				if(operon1.rotation == 1 && operon2.rotation == 1){
					area_temp = (operon1.width + operon2.width) * max(operon1.height,operon2.height);
					if(area_min > area_temp){
						area_min = area_temp;
						npolish.width = max(operon1.height,operon2.height);
						npolish.height = operon1.width + operon2.width;
					}
				}
			}
			stack_p.pop_back();
			stack_p.pop_back();
			npolish.min_space = area_min;
			npolish.rotation = 0;
			npolish.op = 0;
			polish_exp[i].min_space = area_min;
			polish_exp[i].height = npolish.height;
			polish_exp[i].width = npolish.width;
			stack_p.push_back(npolish);
		}
		//cout << polish_exp[i].min_space << endl;
		i++;
	}
	return stack_p.back().min_space;
}
//-------------------------------------------------------------------------------------------------------//
void Floorplan::swap(vector<Polish_S>& polish_exp, int index){
	Polish_S temp;
	temp = polish_exp[index-1];
	polish_exp[index-1] = polish_exp[index+1];
	polish_exp[index+1] = temp;
	return;
}
//-------------------------------------------------------------------------------------------------------//