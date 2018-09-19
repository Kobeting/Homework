#include "dm.h"




//-----------------------------------------------------------------------------------------------------------//
void node_C::getAdjNode(vector<node_C*>& p_rvNode)
{
    p_rvNode.resize(0);
    for(unsigned int nEdgeIter=0;nEdgeIter<m_vInEdge.size();++nEdgeIter)
    {
        p_rvNode.push_back(m_vInEdge[nEdgeIter]->getSrcNode());
    }
    for(unsigned int nEdgeIter=0;nEdgeIter<m_vOutEdge.size();++nEdgeIter)
    {
        p_rvNode.push_back(m_vOutEdge[nEdgeIter]->getTgtNode());
    }
}
//-----------------------------------------------------------------------------------------------------------//
void node_C::getAllEdge(vector<edge_C*>& p_rvEdge)
{
    p_rvEdge.resize(0);
    for(unsigned int nEdgeIter=0;nEdgeIter<m_vInEdge.size();++nEdgeIter)
    {
        p_rvEdge.push_back(m_vInEdge[nEdgeIter]);
    }
    for(unsigned int nEdgeIter=0;nEdgeIter<m_vOutEdge.size();++nEdgeIter)
    {
        p_rvEdge.push_back(m_vOutEdge[nEdgeIter]);
    }
}
//-----------------------------------------------------------------------------------------------------------//
int node_C::getCapacity(node_C* p_pNode)
{
    vector<edge_C*> vEdge;
    getAllEdge(vEdge);
    edge_C* pEdge(0);
    int nCapacity = 0;
    for(unsigned int nEdgeIter=0;nEdgeIter<vEdge.size();++nEdgeIter)
    {
        pEdge = vEdge[nEdgeIter];
        
        if((p_pNode==pEdge->getSrcNode())||(p_pNode==pEdge->getTgtNode()))
        {
            nCapacity = pEdge->getCapacity(this);
        }
    }
    return nCapacity;
}
//-----------------------------------------------------------------------------------------------------------//
edge_C* node_C::getEdge(node_C* p_pNode)
{
    vector<edge_C*> vEdge;
    getAllEdge(vEdge);
    edge_C* pEdge(0);
    for(unsigned int nEdgeIter=0;nEdgeIter<vEdge.size();++nEdgeIter)
    {
        pEdge = vEdge[nEdgeIter];
        
        //if((p_pNode==pEdge->getSrcNode())&&(pEdge->getCapacity(this)>0))
        if((p_pNode==pEdge->getSrcNode()))
        {
            return pEdge;
        }
        //if((p_pNode==pEdge->getTgtNode())&&(pEdge->getCapacity(this)>0))
        if((p_pNode==pEdge->getTgtNode()))
        {
            return pEdge;
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------------------//
