#include "legalize.h"

using namespace std;

//-----------------------------------------------------------------------------------------------//
bool my_operater(Node_S a, Node_S b){
	return a.plNodeX < b.plNodeX;
}
//-----------------------------------------------------------------------------------------------//
bool my_resumeOrder(Node_S a, Node_S b){
	return a.index < b.index;
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::legalize(rplData_S *pData){
	initial(pData);

	unsigned int closestRowIndex, closestDist, thisDist, segment;

	for(unsigned int nNodeIter=0; nNodeIter < plNodes.size(); ++nNodeIter){
		if(plNodes[nNodeIter].plNodeFixed){
			for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
				if(check_bound(Rows[nRowIter],plNodes[nNodeIter]))
					update_freespace(Rows[nRowIter],plNodes[nNodeIter].plNodeX,plNodes[nNodeIter].width);
			}
		}
	}

	for(unsigned int nNodeIter=0; nNodeIter < plNodes.size(); ++nNodeIter){
		if(!plNodes[nNodeIter].plNodeFixed){
			closestDist = INT_MAX;
			closestRowIndex = -1;

			for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
				segment = 0;
				while(segment < Rows[nRowIter].free_space.size()){
					if(is_fit(Rows[nRowIter].free_space[segment],plNodes[nNodeIter])){
						thisDist = distNode(Rows[nRowIter],plNodes[nNodeIter],segment);
						if(closestDist > thisDist) closestRowIndex = nRowIter;
						closestDist = min(closestDist,thisDist);
						break;
					}
					segment++;
				}
			}
			
			if(closestRowIndex != -1){
				segment = 0;
				while(segment < Rows[closestRowIndex].free_space.size()){
					if(is_fit(Rows[closestRowIndex].free_space[segment],plNodes[nNodeIter])){
						plNodes[nNodeIter].plNodeX = Rows[closestRowIndex].free_space[segment].l;
						plNodes[nNodeIter].plNodeY = Rows[closestRowIndex].rowCoord;
						update_freespace(Rows[closestRowIndex],Rows[closestRowIndex].free_space[segment].l,plNodes[nNodeIter].width);
						break;
					}
					segment++;
				}
			}
		}
	}

	//output all free space
	// for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
	// 	cout << nRowIter << " : ";
	// 	segment = 0;
	// 	while(segment < Rows[nRowIter].free_space.size()){
	// 		cout << Rows[nRowIter].free_space[segment].l << "," << Rows[nRowIter].free_space[segment].h << " ";
	// 		segment++;
	// 	}
	// 	cout << endl;
	// }

	sort(plNodes.begin(),plNodes.end(),my_resumeOrder);
	dump();
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::initial(rplData_S *pData){
	Rows.clear();
	Rows.resize(pData->vRow.size());
	for(unsigned int nRowIter=0; nRowIter < Rows.size(); ++nRowIter){
		Seg_S seg;
		seg.l = pData->vRow[nRowIter]->rowSubRowOrig;
		seg.h = seg.l + pData->vRow[nRowIter]->rowSiteSpace * pData->vRow[nRowIter]->numSite;
		Rows[nRowIter].free_space.push_back(seg);
		Rows[nRowIter].rowCoord = pData->vRow[nRowIter]->rowCoord;
		Rows[nRowIter].rowHeight = pData->vRow[nRowIter]->rowHeight;
	}

	// for(unsigned int nRowIter=0; nRowIter < pData->vRow.size(); ++nRowIter){
	// 	cout << nRowIter << " : " << Rows[nRowIter].free_space.front().l << "," << Rows[nRowIter].free_space.front().h << endl;
	// }

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
int Legalize_C::distNode(Row_S row, Node_S node, int segment){
	return sqrt(pow(node.plNodeX - row.free_space[segment].l,2) + pow(node.plNodeY - row.rowCoord,2));
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::update_freespace(Row_S& row, int plNodeX, int width){
	for(unsigned int nSegIter=0; nSegIter < row.free_space.size(); ++nSegIter){
		if(row.free_space[nSegIter].l <= plNodeX && row.free_space[nSegIter].h >= (plNodeX + width)){
			Seg_S seg1,seg2;
			if(row.free_space[nSegIter].h != plNodeX + width){
				seg2.l = plNodeX + width;
				seg2.h = row.free_space[nSegIter].h;
				row.free_space.insert(row.free_space.begin()+nSegIter+1,seg2);
			}
			if(row.free_space[nSegIter].l != plNodeX){
				seg1.l = row.free_space[nSegIter].l;
				seg1.h = plNodeX;
				row.free_space.insert(row.free_space.begin()+nSegIter+1,seg1);
			}
			row.free_space.erase(row.free_space.begin()+nSegIter);
			break;
		}
	}
}
//-----------------------------------------------------------------------------------------------//
bool Legalize_C::check_bound(Row_S row, Node_S node){
	bool overlap = 0;
	overlap |= (node.plNodeY <= row.rowCoord) && (node.plNodeY + node.height >= row.rowCoord + row.rowHeight);
	overlap |= (node.plNodeY >= row.rowCoord) && (node.plNodeY < row.rowCoord + row.rowHeight);
	overlap |= (node.plNodeY + node.height > row.rowCoord) && (node.plNodeY + node.height <= row.rowCoord + row.rowHeight);
	return overlap;
}
//-----------------------------------------------------------------------------------------------//
bool Legalize_C::is_fit(Seg_S segment, Node_S node){
	return segment.h - segment.l >= node.width;
}
//-----------------------------------------------------------------------------------------------//
void Legalize_C::dump(){
	ofstream output;
	output.open("output.pl");
	for(unsigned int nNodeIter=0; nNodeIter < plNodes.size(); ++nNodeIter){
		output << plNodes[nNodeIter].nodeName << "\t" << plNodes[nNodeIter].plNodeX << "\t" <<
			plNodes[nNodeIter].plNodeY << "\t:\tN\t";
		if(plNodes[nNodeIter].plNodeFixed)
			output << "/FIXED";
		output << endl;
	}
}
//-----------------------------------------------------------------------------------------------//