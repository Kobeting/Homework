#include <cassert>
#include <fstream>
#include <queue>
#include "dm.h"
#include "algo.h"
//-----------------------------------------------------------------------------------------------------------//
bool shortestPath_C::BellmanFord()
{
	// dmMgr_C dm = new dmMgr_C;
	node_C* pNode(0);
    vector<edge_C*> vInEdge;
    vector<node_C*> m_vNode;
    m_pDmMgr->getNode(m_vNode);
    m_pDmMgr->getSrcNode()->setCost(0);

    // for(unsigned int nNodeIter=0; nNodeIter<m_vNode.size(); ++nNodeIter){
    //     pNode = m_vNode[nNodeIter];
    //     printf("Complete! %d \n",pNode->getCost());
    // }
    // printf("Fuck!\n");
	for(unsigned int i=0; i<m_vNode.size()-1; ++i){
        for(unsigned int nNodeIter=0; nNodeIter<m_vNode.size(); ++nNodeIter){
            pNode = m_vNode[nNodeIter];
            pNode->getInEdge(vInEdge);
            for(unsigned int nEdgeIter=0; nEdgeIter<vInEdge.size(); ++nEdgeIter){
                Relax(vInEdge[nEdgeIter]->getSrcNode(), vInEdge[nEdgeIter]->getTgtNode(), vInEdge[nEdgeIter]->getWeight());
            }
        }
    }
    // for(unsigned int nNodeIter=0; nNodeIter<m_vNode.size(); ++nNodeIter){
    //     pNode = m_vNode[nNodeIter];
    //     printf("Complete! %d \n",pNode->getCost());
    // }
    return true;
}
//-----------------------------------------------------------------------------------------------------------//
void shortestPath_C::Relax(node_C* u, node_C* v, int w)
{
    if(u->getCost()+w < 0) return;
    if(u->getCost()+w < v->getCost()){
        v->setCost(u->getCost()+w);
        v->setParent(u);
    }
}
//-----------------------------------------------------------------------------------------------------------//
void shortestPath_C::dump()
{
	ofstream output;
    output.open("short.txt");

    node_C* pNode(0);
    vector<node_C*> m_vNode;
    m_pDmMgr->getNode(m_vNode);
    for(unsigned int nNodeIter=0; nNodeIter<m_vNode.size(); ++nNodeIter){
        pNode = m_vNode[nNodeIter];
        output << pNode->getName() << ":" << pNode->getCost() << endl;
    }
    output.close();
}
//-----------------------------------------------------------------------------------------------------------//
bool CriticalPath_C::topologicalSort()
{
    node_C* pNode(0);
    vector<node_C*> m_vNode;
    vector<edge_C*> vOutEdge;
    m_pDmMgr->getNode(m_vNode);

    queue<node_C*> q;
    q.push(m_pDmMgr->getSrcNode());

    while(q.size() != 0){
        pNode = q.front();
        q.pop();
        pNode->getOutEdge(vOutEdge);
        for(unsigned int nEdgeIter=0; nEdgeIter<vOutEdge.size(); ++nEdgeIter){
            vOutEdge[nEdgeIter]->getTgtNode()->setTopoCost(vOutEdge[nEdgeIter]->getTgtNode()->getTopoCost()+1);
            vector<edge_C*> vInEdge;
            vOutEdge[nEdgeIter]->getTgtNode()->getInEdge(vInEdge);
            if(vOutEdge[nEdgeIter]->getTgtNode()->getTopoCost() == vInEdge.size()){
                q.push(vOutEdge[nEdgeIter]->getTgtNode());
                int MaxDelay = 0;
                for(unsigned int inEdgeIter=0; inEdgeIter<vInEdge.size(); ++inEdgeIter){
                    MaxDelay = vInEdge[inEdgeIter]->getSrcNode()->getDelay()+vInEdge[inEdgeIter]->getWeight() > MaxDelay ? vInEdge[inEdgeIter]->getSrcNode()->getDelay()+vInEdge[inEdgeIter]->getWeight() : MaxDelay;
                }
                vOutEdge[nEdgeIter]->getTgtNode()->setDelay(MaxDelay);
            }
        }
    }
    // for(unsigned int nNodeIter=0; nNodeIter<m_vNode.size(); ++nNodeIter){
    //     pNode = m_vNode[nNodeIter];
    //     printf("Complete! %d \n",pNode->getDelay());
    // }
    return true;
}
//-----------------------------------------------------------------------------------------------------------//
void CriticalPath_C::dump()
{	
	ofstream output;
    output.open("delay.txt");

    node_C* pNode(0);
    vector<node_C*> m_vNode;
    m_pDmMgr->getNode(m_vNode);

    int MaxDelay = 0;
    for(unsigned int nNodeIter=0; nNodeIter<m_vNode.size(); ++nNodeIter){
        pNode = m_vNode[nNodeIter];
        MaxDelay = MaxDelay > pNode->getDelay() ? MaxDelay : pNode->getDelay();
    }
    output << "MaxDelay:" << MaxDelay;
    output.close();
} 
//-----------------------------------------------------------------------------------------------------------//
