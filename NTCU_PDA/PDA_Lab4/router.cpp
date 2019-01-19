#include "router.h"

using namespace std;

//-------------------------------------------------------------------------------------------------------//
void Router::initialize(data_C data){
	
	int width = estimate_width(data);
	col.clear();
	col.resize(width,0);
	row.clear();
	row.resize(width,0);
	row_next.clear();
	row_next.resize(width,0);
	row_temp.clear();
	row_temp.resize(width,0);

	output_row.clear();
	output_col.clear();

	init_status_table(data);
}
//-------------------------------------------------------------------------------------------------------//
int Router::estimate_width(data_C data){
	int pin_max = 0;
	int column_size = data.pins.size();
	for(int ColumnIter=0; ColumnIter<column_size; ColumnIter++){
		pin_max = max(data.pins[ColumnIter].t,pin_max);
		pin_max = max(data.pins[ColumnIter].b,pin_max);
	}

	vector<int> overlap(column_size,0);

	nets.clear();
	nets.resize(pin_max+1);

	for(int PinIter=1; PinIter<=pin_max; PinIter++){
		int ColumnIter = 0;
		int start, end;
		while(data.pins[ColumnIter].t != PinIter && data.pins[ColumnIter].b != PinIter && ColumnIter < column_size) ColumnIter++;
		start = ColumnIter;
		ColumnIter = data.pins.size() - 1;
		while(data.pins[ColumnIter].t != PinIter && data.pins[ColumnIter].b != PinIter && ColumnIter >= 0) ColumnIter--;
		end = ColumnIter;

		if(end >= 0 && start < column_size){
			nets[PinIter].l = start;
			nets[PinIter].r = end;
			nets[PinIter].valid = 1;
			for(int i=start; i<=end; i++){
				overlap[i] += 1;
			}
		}
		else
			nets[PinIter].valid = 0;
	}
	int capacity = 0;
	for(int i=0; i<overlap.size();i++)
		capacity = max(overlap[i],capacity);
	return capacity;
}
//-------------------------------------------------------------------------------------------------------//
void Router::init_status_table(data_C data){
	int pin_max = 0;
	int column_size = data.pins.size();
	for(int ColumnIter=0; ColumnIter<column_size; ColumnIter++){
		pin_max = max(data.pins[ColumnIter].t,pin_max);
		pin_max = max(data.pins[ColumnIter].b,pin_max);
	}
	status_table.clear();
	status_table.resize(pin_max + 1);
	for(int PinIter=0; PinIter<status_table.size(); PinIter++){
		status_table[PinIter].resize(column_size);
	}

	for(int ColumnIter=0; ColumnIter<column_size; ColumnIter++){
		for(int PinIter=0; PinIter<status_table.size(); PinIter++){
			bool falling = 0;
			bool raising = 0;
			for(int x=ColumnIter+1; x<column_size && x<=ColumnIter+SNC; x++){
				if(data.pins[x].t == PinIter) raising = 1;
				if(data.pins[x].b == PinIter) falling = 1;
			}
			if(raising && falling) status_table[PinIter][ColumnIter] = 0;
			else if(raising) status_table[PinIter][ColumnIter] = 1;
			else if(falling) status_table[PinIter][ColumnIter] = -1;
			else status_table[PinIter][ColumnIter] = 0;
		}
	}
}
//-------------------------------------------------------------------------------------------------------//
int Router::channel_route(data_C data){
	int ColumnIter = 0;
	while(ColumnIter < data.pins.size() || !all_route_complete()){
		if(ColumnIter != 0){
			row = row_next;
			col.clear();
			col.resize(row.size(),0);
			row_next.clear();
			row_next.resize(row.size(),0);
			row_temp.clear();
			row_temp.resize(row.size(),0);
		}

		bool pin_t = 1;
		bool pin_b = 1;
		if(ColumnIter < data.pins.size()){
			if(data.pins[ColumnIter].b != 0) pin_b &= make_connect(0,data.pins[ColumnIter].b);
			if(data.pins[ColumnIter].t != 0) pin_t &= make_connect(1,data.pins[ColumnIter].t);
		}
		//step 1
		
		collap_split(ColumnIter);
		//step 2

		add_jog(ColumnIter);
		//step 4
		
		for(int RowIter=0; RowIter<row.size(); RowIter++){
			if(row_temp[RowIter] != 0) row[RowIter] = row_temp[RowIter];
		}
		//backup resotre
		
		if(!pin_b) widen_channel(0,data.pins[ColumnIter].b);
		if(!pin_t) widen_channel(1,data.pins[ColumnIter].t);
		//step 5

		for(int RowIter=0; RowIter<row_next.size(); RowIter++){
			if(row_next_check(row_next[RowIter],ColumnIter) == 1) row_next[RowIter] = 0;
		}
		//check nextrow

		ColumnIter++;
		//step 6

		output_row.push_back(row);
		output_col.push_back(col);
		//store output
	}

	for(int ColumnIter=0; ColumnIter < output_col.size(); ColumnIter++){
		vector<int>& col = output_col[ColumnIter];
		if(ColumnIter < data.pins.size()){
			if(data.pins[ColumnIter].b != 0) col.insert(col.begin(),data.pins[ColumnIter].b);
			else col.insert(col.begin(),0);
			if(data.pins[ColumnIter].t != 0) col.push_back(data.pins[ColumnIter].t);
			else col.push_back(0);
		}
		else{
			col.insert(col.begin(),0);
			col.push_back(0);
		}
	}
	return output_col[0].size() - 2;
}
//-------------------------------------------------------------------------------------------------------//
bool Router::row_next_check(int pin, int column){
	if(pin == 0) return 0;
	if(nets[pin].r > column) return 0;
	int track_num = 0;
	for(int RowIter=0; RowIter<row_next.size(); RowIter++){
		if(pin == row_next[RowIter]) track_num++;
	}
	if(track_num > 1) return 0;
	return 1;
}
//-------------------------------------------------------------------------------------------------------//
bool Router::all_route_complete(){
	for(int RowIter=0; RowIter<row_next.size(); RowIter++)
		if(row_next[RowIter] != 0) return 0;
	return 1;
}
//-------------------------------------------------------------------------------------------------------//
bool Router::make_connect(bool position, int pin){
	int y;
	if(position){
		y = row.size() - 1;
		while(row[y] != 0 && row[y] != pin){
			if(--y < 0 || col[y] != 0) return 0;
		}
		for(int i=row.size()-1; i>=y; i--){
			col[i] = pin;
		}
		row[y] = pin;
	}
	else{
		y = 0;
		while(row[y] != 0 && row[y] != pin){
			if(++y >= row.size() || col[y] != 0) return 0;
		}
		for(int i=0; i<=y; i++){
			col[i] = pin;
		}
		row[y] = pin;
	}
	return 1;
}
//-------------------------------------------------------------------------------------------------------//
void Router::collap_split(int column){
	for(int y=0; y<row.size(); y++){
		if(row[y] != 0){
			int collaping_index = row[y];
			int finding = y + 1;
			while(finding < row.size() && row[finding] != collaping_index){
				if(col[finding] != 0 && col[finding] != collaping_index) finding = row.size();
				finding++;
			}
			if(finding < row.size()){
				for(int i=y; i<=finding; i++){
					col[i] = collaping_index;
				}
				if(check_status(column,collaping_index) == 1){
					row_temp[y] = collaping_index;
					row[y] = 0;
				}
				else{
					row_temp[finding] = collaping_index;
					row[finding] = 0;
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------//
void Router::add_jog(int column){
	for(int LowIter=0, UpIter=row.size()-1; LowIter<=UpIter; LowIter++, UpIter--){
		if(LowIter != UpIter){
			if(row[LowIter] != 0 && !per_route_complete(column,row[LowIter])){
				if(check_status(column,row[LowIter]) == 1){
					int pin = row[LowIter];
					int finding = LowIter;
					while(finding < row.size()){
						if(col[++finding] != 0) break;
					}
					while(finding > LowIter){
						if(row[--finding] == 0) break;
					}
					if(finding - LowIter >= MJL){
						for(int RowIter=LowIter; RowIter<=finding; RowIter++){
							col[RowIter] = pin;
						}
						row[finding] = pin;
						row_next[finding] = pin;
					}
					else{
						row_next[LowIter] = pin;
					}
				}
				else if(check_status(column,row[LowIter]) == -1){
					int pin = row[LowIter];
					int finding = LowIter;
					while(finding >= 0){
						if(col[--finding] != 0) break;
					}
					while(finding < LowIter){
						if(row[++finding] == 0) break;
					}
					if(LowIter - finding >= MJL){
						for(int RowIter=LowIter; RowIter>=finding; RowIter--){
							col[RowIter] = pin;
						}
						row[finding] = pin;
						row_next[finding] = pin;
					}
					else{
						row_next[LowIter] = pin;
					}
				}
				else{
					row_next[LowIter] = row[LowIter];
				}
			}
			if(row[UpIter] != 0 && !per_route_complete(column,row[UpIter])){
				if(check_status(column,row[UpIter]) == 1){
					int pin = row[UpIter];
					int finding = UpIter;
					while(finding < row.size()){
						if(col[++finding] != 0) break;
					}
					while(finding > UpIter){
						if(row[--finding] == 0) break;
					}
					if(finding - UpIter >= MJL){
						for(int RowIter=UpIter; RowIter<=finding; RowIter++){
							col[RowIter] = pin;
						}
						row[finding] = pin;
						row_next[finding] = pin;
					}
					else{
						row_next[UpIter] = pin;
					}
				}
				else if(check_status(column,row[UpIter]) == -1){
					int pin = row[UpIter];
					int finding = UpIter;
					while(finding >= 0){
						if(col[--finding] != 0) break;
					}
					while(finding < UpIter){
						if(row[++finding] == 0) break;
					}
					if(UpIter - finding >= MJL){
						for(int RowIter=UpIter; RowIter>=finding; RowIter--){
							col[RowIter] = pin;
						}
						row[finding] = pin;
						row_next[finding] = pin;
					}
					else{
						row_next[UpIter] = pin;
					}
				}
				else{
					row_next[UpIter] = row[UpIter];
				}
			}
		}
		else{
			if(row[LowIter] != 0 && !per_route_complete(column,row[LowIter])){
				if(check_status(column,row[LowIter]) == 1){
					int pin = row[LowIter];
					int finding = LowIter;
					while(finding < row.size()){
						if(col[++finding] != 0) break;
					}
					while(finding > LowIter){
						if(row[--finding] == 0) break;
					}
					if(finding - LowIter >= MJL){
						for(int RowIter=LowIter; RowIter<=finding; RowIter++){
							col[RowIter] = pin;
						}
						row[finding] = pin;
						row_next[finding] = pin;
					}
					else{
						row_next[LowIter] = pin;
					}
				}
				else if(check_status(column,row[LowIter]) == -1){
					int pin = row[LowIter];
					int finding = LowIter;
					while(finding >= 0){
						if(col[--finding] != 0) break;
					}
					while(finding < LowIter){
						if(row[++finding] == 0) break;
					}
					if(LowIter - finding >= MJL){
						for(int RowIter=LowIter; RowIter>=finding; RowIter--){
							col[RowIter] = pin;
						}
						row[finding] = pin;
						row_next[finding] = pin;
					}
					else{
						row_next[LowIter] = pin;
					}
				}
				else{
					row_next[LowIter] = row[LowIter];
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------//
bool Router::per_route_complete(int column, int pin){
	if(nets[pin].r >= column) return 0;
	int track_num = 0;
	for(int RowIter=0; RowIter<row.size(); RowIter++){
		if(pin == row[RowIter]) track_num++;
	}
	if(track_num > 1) return 0;
	return 1;
}
//-------------------------------------------------------------------------------------------------------//
int Router::check_status(int column, int pin){
	return status_table[pin][column];
}
//-------------------------------------------------------------------------------------------------------//
void Router::widen_channel(bool position, int pin){
	int y;
	if(position){
		y = row.size() - 1;
		while(col[y] == 0 && y > row.size()/2) y--;
		for(int RowIter=row.size()-1; RowIter>y; RowIter--){
			col[RowIter] = pin;
		}
		row.insert(row.begin()+y+1,pin);
		col.insert(col.begin()+y+1,pin);
		
		row_next.insert(row_next.begin()+y+1,pin);
		widen_previous_channel(y+1);
	}
	else{
		y = 0;
		while(col[y] == 0 && y < row.size()/2) y++;
		for(int RowIter=0; RowIter<y; RowIter++){
			col[RowIter] = pin;
		}
		row.insert(row.begin()+y,pin);
		col.insert(col.begin()+y,pin);

		row_next.insert(row_next.begin()+y,pin);
		widen_previous_channel(y);
	}
}
//-------------------------------------------------------------------------------------------------------//
void Router::widen_previous_channel(int y){
	for(int ColumnIter=0; ColumnIter<output_row.size(); ColumnIter++){
		vector<int>& row = output_row[ColumnIter];
		row.insert(row.begin()+y,0);
	}
	for(int ColumnIter=0; ColumnIter<output_col.size(); ColumnIter++){
		vector<int>& col = output_col[ColumnIter];
		if(y == col.size())
			col.insert(col.begin()+y,col[y-1]);
		else if(y == 0)
			col.insert(col.begin()+y,col[0]);
		else if(y > 0 && col[y] == col[y-1])
			col.insert(col.begin()+y,col[y]);
		else
			col.insert(col.begin()+y,0);
	}
}
//-------------------------------------------------------------------------------------------------------//
void Router::output(const char* FileName){

	// for(int RowIter=output_col[0].size()-1; RowIter>=0; RowIter--){
	// 	for(int ColumnIter=0; ColumnIter<output_col.size(); ColumnIter++){
	// 		cout << setw(2) << right << output_col[ColumnIter][RowIter];
	// 	}
	// 	cout << endl;
		
	// }
	// cout << endl << endl;
	// for(int RowIter=output_row[0].size()-1; RowIter>=0; RowIter--){
	// 	for(int ColumnIter=0; ColumnIter<output_row.size(); ColumnIter++){
	// 		cout << setw(2) << right << output_row[ColumnIter][RowIter];
	// 	}
	// 	cout << endl;
	// }

	ofstream output;
	output.open(FileName);

	int pin_max = nets.size()-1;
	
	for(int PinIter=1; PinIter<pin_max+1; PinIter++){
		if(nets[PinIter].valid){
			output << ".begin " << PinIter << endl;
			for(int ColumnIter=0; ColumnIter<output_col.size(); ColumnIter++){
				for(int RowIter=0; RowIter<output_col[0].size(); RowIter++){
					if(output_col[ColumnIter][RowIter] == PinIter){
						output << ".V " << ColumnIter << " " << RowIter << " ";
						while(RowIter+1<output_col[0].size() && output_col[ColumnIter][RowIter+1] == PinIter) RowIter++;
						output << RowIter << endl;
					}
				}
			}
			for(int RowIter=0; RowIter<output_row[0].size(); RowIter++){
				for(int ColumnIter=0; ColumnIter<output_row.size(); ColumnIter++){
					if(output_row[ColumnIter][RowIter] == PinIter){
						if(ColumnIter + 1 < output_row.size() && output_row[ColumnIter+1][RowIter] == PinIter){
							output << ".H " << ColumnIter << " " << RowIter + 1 << " ";
							while(ColumnIter + 1 < output_row.size() && output_row[ColumnIter+1][RowIter] == PinIter) ColumnIter++;
							output << ColumnIter << endl;
						}
					}
				}
			}
			output << ".end" << endl;
		}
	}
}
//-------------------------------------------------------------------------------------------------------//
void Router::dump(data_C data){
	for(int ColumnIter=0; ColumnIter<output_row.size(); ColumnIter++){
		output_row[ColumnIter].insert(output_row[ColumnIter].begin(),0);
		output_row[ColumnIter].push_back(0);
	}
	for(int RowIter=output_row[0].size()-1; RowIter>=0; RowIter--){
		for(int ColumnIter=0; ColumnIter<output_row.size(); ColumnIter++){
			if(ColumnIter == 0){
				if(output_row[ColumnIter][RowIter] != output_row[ColumnIter+1][RowIter])
					output_row[ColumnIter][RowIter] = 0;
			}
			else if(ColumnIter == output_row.size()-1){
				if(output_row[ColumnIter][RowIter] != output_row[ColumnIter-1][RowIter])
					output_row[ColumnIter][RowIter] = 0;
			}
			else{
				if(output_row[ColumnIter][RowIter] != output_row[ColumnIter-1][RowIter] && 
					output_row[ColumnIter][RowIter] != output_row[ColumnIter+1][RowIter])
					output_row[ColumnIter][RowIter] = 0;
			}
		}
	}

	for(int PinIter=0; PinIter<data.pins.size(); PinIter++){
		cout << setw(2) << right << data.pins[PinIter].t << " ";
	}
	cout << endl;

	for(int RowIter=output_col[0].size()-1; RowIter>=0; RowIter--){
		for(int ColumnIter=0; ColumnIter<output_col.size(); ColumnIter++){
			if(output_col[ColumnIter][RowIter] == 0 && output_row[ColumnIter][RowIter] == 0){
				cout << "   ";
			}
			else if(output_col[ColumnIter][RowIter] == output_row[ColumnIter][RowIter]){
				if(ColumnIter == 0)
					cout << " o-";
				else if(ColumnIter == output_col.size()-1)
					cout << "-o ";
				else if(output_row[ColumnIter][RowIter] == output_row[ColumnIter+1][RowIter] &&
					output_row[ColumnIter][RowIter] == output_row[ColumnIter-1][RowIter])
					cout << "-o-";
				else if(output_row[ColumnIter][RowIter] == output_row[ColumnIter+1][RowIter])
					cout << " o-";
				else if(output_row[ColumnIter][RowIter] == output_row[ColumnIter-1][RowIter])
					cout << "-o ";
			}
			else if(output_col[ColumnIter][RowIter] != 0 && output_row[ColumnIter][RowIter] != 0){
				cout << "-+-";
			}
			else if(output_col[ColumnIter][RowIter] != 0){
				cout << " | ";
			}
			else if(output_row[ColumnIter][RowIter] != 0){
				cout << "---";
			}
		}
		cout << endl;
	}

	for(int PinIter=0; PinIter<data.pins.size(); PinIter++){
		cout << setw(2) << right << data.pins[PinIter].b << " ";
	}
}
//-------------------------------------------------------------------------------------------------------//
