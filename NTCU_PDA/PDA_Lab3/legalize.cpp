#include "legalize.h"

using namespace std;

//-----------------------------------------------------------------------------------------------//
bool my_operater(Node_S a, Node_S b){
	return a.plNodeX < b.plNodeX;
}
//-----------------------------------------------------------------------------------------------//
bool row_sort(Row_S a, Row_S b){
	return a.rowCoord < b.rowCoord;
}
//-----------------------------------------------------------------------------------------------//
bool my_resumeOrder(Node_S a, Node_S b){
	return a.index < b.index;
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::legalize(rplData_S *pData){
	bug_here_delete_it_will_cause_segfail.clear();
	
	initial(pData);

	for(unsigned int nNodeIter=0; nNodeIter < plNodes.size(); ++nNodeIter){
		if(plNodes[nNodeIter].plNodeFixed){
			for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
				if(check_overlap(Rows[nRowIter],plNodes[nNodeIter]))
					slice_row(nRowIter,plNodes[nNodeIter]);
			}
		}
	}
	sort(Rows.begin(),Rows.end(),row_sort);
	
	for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter)
		Rows[nRowIter].space = Rows[nRowIter].rowMax - Rows[nRowIter].rowMin;

	int row_size = Rows.size();
	
	for(unsigned int nNodeIter=0; nNodeIter < plNodes.size(); ++nNodeIter){
		
		if(!plNodes[nNodeIter].plNodeFixed){
			int cost_best = INT_MAX;
			// cout << nNodeIter << endl;
			int row_mid = find_row(plNodes[nNodeIter]);
			int row_best = row_mid;
			int nRowIter = row_mid;
			while(nRowIter < row_size){
				if(space_cheak(plNodes[nNodeIter],nRowIter)){
					int cost = place_row(nRowIter,nNodeIter);
					if(cost < cost_best){
						cost_best = cost;
						row_best = nRowIter;
					}
				}
				nRowIter++;
				if(cost_best < Rows[nRowIter].rowCoord - plNodes[nNodeIter].plNodeY) break;
			}

			nRowIter = row_mid - 1;
			while(nRowIter >= 0){
				if(space_cheak(plNodes[nNodeIter],nRowIter)){
					int cost = place_row(nRowIter,nNodeIter);
					if(cost < cost_best){
						cost_best = cost;
						row_best = nRowIter;
					}
				}
				nRowIter--;
				if(cost_best < plNodes[nNodeIter].plNodeY - Rows[nRowIter].rowCoord) break;
			}
			place_row_final(row_best,nNodeIter);
		}
	}
	
	for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
		if(!Rows[nRowIter].clusters.empty())
			collapse(nRowIter);
	}

	for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
		for(unsigned int c=0; c < Rows[nRowIter].clusters.size(); ++c){
			const Cluster_S& cluster = Rows[nRowIter].clusters[c];
			int x = cluster.x;
			for(unsigned int i=0; i < cluster.nodes.size(); ++i){
				plNodes[cluster.nodes[i]].plNodeX = x;
				plNodes[cluster.nodes[i]].plNodeY = Rows[nRowIter].rowCoord;
				x += plNodes[cluster.nodes[i]].width;
			}
		}
	}
	
	sort(plNodes.begin(),plNodes.end(),my_resumeOrder);
	dump();
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::initial(rplData_S *pData){
	Rows.clear();
	Rows.resize(pData->vRow.size());
	for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
		Rows[nRowIter].rowCoord = pData->vRow[nRowIter]->rowCoord;
		Rows[nRowIter].rowHeight = pData->vRow[nRowIter]->rowHeight;
		Rows[nRowIter].rowMin = pData->vRow[nRowIter]->rowSubRowOrig;
		Rows[nRowIter].rowMax = Rows[nRowIter].rowMin + pData->vRow[nRowIter]->numSite * pData->vRow[nRowIter]->rowSiteSpace;
	}

	plNodes.clear();
	rplNode_S* tNode(0);
	rplPlNode_S* tPlNode(0);
	for(unsigned int nNodeIter=0; nNodeIter < pData->vNode.size(); ++nNodeIter){
		Node_S Node;
		tNode = pData->vNode[nNodeIter];
		tPlNode = pData->vPlNode[nNodeIter];
		Node.index = nNodeIter;
		strcpy(Node.nodeName,tNode->nodeName);
		Node.width = tNode->width;
		Node.height = tNode->height;
		Node.plNodeX = tPlNode->plNodeX;
		Node.plNodeY = tPlNode->plNodeY;
		Node.plNodeFixed = tPlNode->plNodeFixed;
		plNodes.push_back(Node);
	}
	
	sort(plNodes.begin(),plNodes.end(),my_operater);
}
//-----------------------------------------------------------------------------------------------//
int Legalize_C::place_row(unsigned int r, unsigned int nNodeIter){
	// cout << "Node:" << nNodeIter << "  Place Row:" << r << endl;
	vector<Cluster_S> clusters_orig = Rows[r].clusters;
	vector<Cluster_S>& clusters = Rows[r].clusters;
	Cluster_S cluster;
	
	if(clusters.empty() || clusters.back().x + clusters.back().w <= safty_add(r,nNodeIter)){
		init_cluster(cluster);
		clusters.push_back(cluster);
		clusters.back().x = safty_add(r,nNodeIter);
		clusters.back().n_first = nNodeIter;
		add_cell(clusters.back(),nNodeIter);
	}
	else{
		add_cell(clusters.back(),nNodeIter);
		collapse(r);
	}
	int x = Rows[r].clusters.back().x;
	for(unsigned int i = 0; i < Rows[r].clusters.back().nodes.size(); ++i){
		const unsigned int n = Rows[r].clusters.back().nodes[i];
		x += plNodes[n].width;
	}
	Rows[r].clusters = clusters_orig;
	return abs(x - plNodes[nNodeIter].plNodeX) + abs(Rows[r].rowCoord - plNodes[nNodeIter].plNodeY);
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::place_row_final(unsigned int r, unsigned int nNodeIter){
	vector<Cluster_S>& clusters = Rows[r].clusters;
	Cluster_S cluster;

	if(clusters.empty() || clusters.back().x + clusters.back().w <= safty_add(r,nNodeIter)){
		init_cluster(cluster);
		clusters.push_back(cluster);
		clusters.back().x = safty_add(r,nNodeIter);
		clusters.back().n_first = nNodeIter;
		add_cell(clusters.back(),nNodeIter);
	}
	else{
		add_cell(clusters.back(),nNodeIter);
		collapse(r);
	}
	Rows[r].space -= plNodes[nNodeIter].width;
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::init_cluster(Cluster_S& cluster){
	cluster.e = 0;
	cluster.w = 0;
	cluster.q = 0;
}
//-----------------------------------------------------------------------------------------------//
int Legalize_C::safty_add(unsigned int r, unsigned int nNodeIter){
	// cout << "add cell" << endl;
	const Node_S& node = plNodes[nNodeIter];
	int x;
	if(node.plNodeX > Rows[r].rowMax)
		x = Rows[r].rowMax - node.width;
	else if(node.plNodeX < Rows[r].rowMin)
		x = Rows[r].rowMin;
	else
		x = node.plNodeX;
	return x;
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::add_cell(Cluster_S& cluster, unsigned int nNodeIter){
	// cout << "add cell" << endl;
	const Node_S& node = plNodes[nNodeIter];
	cluster.n_last = nNodeIter;
	cluster.e += 1;
	cluster.q += 1 * (node.plNodeX - cluster.w);
	cluster.w += node.width;
	cluster.nodes.push_back(nNodeIter);
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::add_cluster(unsigned int r){
	// cout << "add cluster" << endl;
	Cluster_S& cluster = Rows[r].clusters.back();
	Cluster_S& cluster_pre = Rows[r].clusters[Rows[r].clusters.size()-2];

	cluster_pre.nodes.insert(cluster_pre.nodes.end(),cluster.nodes.begin(),cluster.nodes.end());
	cluster_pre.n_last = cluster.n_last;
	cluster_pre.e += cluster.e;
	cluster_pre.q += cluster.q - (cluster.e * cluster_pre.w);
	cluster_pre.w += cluster.w;
	Rows[r].clusters.pop_back();
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::collapse(unsigned int r){
	Cluster_S& cluster = Rows[r].clusters.back();
	cluster.x = cluster.q / cluster.e;
	if(cluster.x < Rows[r].rowMin) cluster.x = Rows[r].rowMin;
	if(cluster.x > Rows[r].rowMax - cluster.w) cluster.x = Rows[r].rowMax - cluster.w;
	if(Rows[r].clusters.size() > 1){
		Cluster_S& cluster_pre = Rows[r].clusters[Rows[r].clusters.size()-2];
		if(cluster_pre.x + cluster_pre.w > cluster.x){
			add_cluster(r);
			collapse(r);
		}
	}
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::slice_row(unsigned int r, Node_S node){
	Row_S n_row;
	n_row = Rows[r];
	Rows[r].rowMax = node.plNodeX;
	n_row.rowMin = node.plNodeX + node.width;
	Rows.insert(Rows.begin()+r+1,n_row);
}
//-----------------------------------------------------------------------------------------------//
bool Legalize_C::check_overlap(Row_S row, Node_S node){
	bool y_overlap = 0;
	bool x_overlap = 0;
	y_overlap |= (node.plNodeY <= row.rowCoord) && (node.plNodeY + node.height >= row.rowCoord + row.rowHeight);
	y_overlap |= (node.plNodeY >= row.rowCoord) && (node.plNodeY < row.rowCoord + row.rowHeight);
	y_overlap |= (node.plNodeY + node.height > row.rowCoord) && (node.plNodeY + node.height <= row.rowCoord + row.rowHeight);
	x_overlap |= (node.plNodeX <= row.rowMin) && (node.plNodeX + node.width >= row.rowMax);
	x_overlap |= (node.plNodeX >= row.rowMin) && (node.plNodeX < row.rowMax);
	x_overlap |= (node.plNodeX + node.width > row.rowMin) && (node.plNodeX + node.width <= row.rowMax);
	return x_overlap && y_overlap;
}
//-----------------------------------------------------------------------------------------------//
int Legalize_C::find_row(Node_S node){
	int nearest = 0;
	int upp = Rows.size()-1;
	while(Rows[nearest].rowCoord <= node.plNodeY && nearest < upp)
		nearest++;
	return nearest;
}
//-----------------------------------------------------------------------------------------------//
bool Legalize_C::space_cheak(Node_S node, unsigned int r){
	return Rows[r].space >= node.width;
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::dump(){
	ofstream output;
	output.open("output.pl");
	for(unsigned int nNodeIter=0; nNodeIter < plNodes.size(); ++nNodeIter){
		output << plNodes[nNodeIter].nodeName << " " << plNodes[nNodeIter].plNodeX << " " <<
			plNodes[nNodeIter].plNodeY << " : N ";
		if(plNodes[nNodeIter].plNodeFixed)
			output << "/FIXED";
		output << endl;
	}
}
//-----------------------------------------------------------------------------------------------//