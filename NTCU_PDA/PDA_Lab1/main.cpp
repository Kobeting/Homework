#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <limits>
#include <stack>
#include <queue>
#include <map>
#include "parser.h"
#include "dm.h"
#include "algo.h"
using namespace std;

//-------------------------------------------------------------------------------------------------------//
int main(int argc, char** argv)
{
    /*Preliminary Information*/
    cout << "     ##############################################################" << endl;
    cout << "     #                      [PDA 2018] Lab1                       #" << endl; 
    cout << "     #------------------------------------------------------------#" << endl;
    cout << "     #    1. BellmanFord Algorithm                                #" << endl; 
    cout << "     #    2. Topoloical Sort Algorithm                            #" << endl; 
    cout << "     #------------------------------------------------------------#" << endl;
    cout << "     # Compiled in " << sizeof(void*)*8 << " bits on " //<< COMPILETIME 
         << "        #"<<endl;
    #ifdef __GNUC__
    cout << "     # with GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__
         << "                                             #"<<endl;
    #endif
    cout << "     ###############################################################" << endl;
    cout << endl;
    /*Begin Code Execution*/
    clock_t start,end;
    start = clock();
    graphParser_C *pParser = new graphParser_C(argv[1]);
    pParser->dump();
    dmMgr_C* pDMmgr = new dmMgr_C(pParser->getGraphData());
    pDMmgr->dump();
    //Shortest Path
    shortestPath_C* pShortestPath = new shortestPath_C(pDMmgr);
    pShortestPath->BellmanFord();
    pShortestPath->dump();
    //Cirtical Path Delay
    CriticalPath_C* pCriticalPath = new CriticalPath_C(pDMmgr);
    pCriticalPath->topologicalSort();
    pCriticalPath->dump();
    end = clock();
}
