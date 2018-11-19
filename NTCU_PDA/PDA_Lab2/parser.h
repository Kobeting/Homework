#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

//-------------------------------------------------------------------------------------------------------//
struct termianl_S
{
    public:
        char name[32];
        int x;
        int y;
};
//-------------------------------------------------------------------------------------------------------//
struct block_S
{
    public:
        char name[32];
        int x;
        int y;
};
//-------------------------------------------------------------------------------------------------------//
struct net_S
{
    public:
        vector<int> net;
};
//-------------------------------------------------------------------------------------------------------//
class data_C
{
    public:
        int outline_x;
        int outline_y;

        vector<string> name_vectors;
        vector<block_S> blocks;
        vector<termianl_S> termianls;
        vector<net_S> nets;
};
//-------------------------------------------------------------------------------------------------------//
class Parser
{
    public:
        bool parser(const char* BlockFileName, const char* NetFileName, data_C data);
};