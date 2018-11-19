#include "parser.h"

using namespace std;

//-------------------------------------------------------------------------------------------------------//
bool Parser::parser(const char* BlockFileName, const char* NetFileName, data_C data)
{
	char buffer[80];
	ifstream input;
    input.open(BlockFileName, ios::in);

	if (!input){
        printf("File %s Can Not Be Opened! Exit\n", BlockFileName);
        return 0;
    }
    else{
    	printf("[Parser] - File %s Open Success.\n", BlockFileName);
        printf("[Parser] - Parsing! ... \n");
    }
    int num_block;
    int num_terminal;

    input.getline(buffer,80);
    sscanf(buffer,"Outline: %d %d", &data.outline_x, &data.outline_y);
	input.getline(buffer,80);
    sscanf(buffer,"NumBlocks: %d", &num_block);
    input.getline(buffer,80);
    sscanf(buffer,"NumTerminals: %d", &num_terminal);
    input.getline(buffer,80);

    while(num_block != 0){
        num_block--;
    	block_S nblock;
    	input.getline(buffer,80);
    	sscanf(buffer,"%s %d %d", &nblock.name, &nblock.x, &nblock.y);
        data.name_vectors.push_back(nblock.name);
    	data.blocks.push_back(nblock);
    }
    input.getline(buffer,80);
    while(num_terminal != 0){
        num_terminal--;
        termianl_S nterminal;
        input.getline(buffer,80);
        sscanf(buffer,"%s terminal %d %d", &nterminal.name, &nterminal.x, &nterminal.y);
        data.name_vectors.push_back(nterminal.name);
        data.termianls.push_back(nterminal);
    }
    input.close();

    input.open(NetFileName, ios::in);
    if (!input){
        printf("File %s Can Not Be Opened! Exit\n", NetFileName);
        return 0;
    }
    else{
        printf("[Parser] - File %s Open Success.\n", NetFileName);
        printf("[Parser] - Parsing! ... \n");
    }
    
    int num_nets;
    int deg_net;
    char block_name[32];
    input.getline(buffer,80);
    sscanf(buffer,"NumNets: %d", &num_nets);
    while(num_nets-- != 0){
        data.nets.resize(data.nets.size()+1);
        data.nets.back().net.resize(data.name_vectors.size(),0);
        input.getline(buffer,80);
        sscanf(buffer,"NetDegree: %d", &deg_net);
        while(deg_net-- != 0){
            input.getline(buffer,80);
            sscanf(buffer,"%s", &block_name);
            for(int i=0; i<data.name_vectors.size(); i++){
                if(data.name_vectors[i].compare(block_name) == 0){
                    data.nets.back().net[i] = 1;
                    break;
                }
            }
        }
    }
    printf("Parsing Complete!\n");
    input.close();
    
    // for( net_S net : data.nets){
    //     for(int i=0; i<net.net.size(); i++){
    //         cout << net.net[i];
    //     }
    //     cout << endl;
    // }

    return 1;
}
//-------------------------------------------------------------------------------------------------------//
