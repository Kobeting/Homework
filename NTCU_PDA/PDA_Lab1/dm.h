#ifndef _DMMGR_H_
#define _DMMGR_H_
#include "parser.h"
#include <climits>

class node_C;

//-----------------------------------------------------------------------------------------------------------//
class edge_C
{
    public:
        edge_C(string p_strName,node_C* p_pSrcNode, node_C* p_pTgtNode,int p_nWeight)
        {
            m_strName = p_strName;
            m_pSrcNode = p_pSrcNode;
            m_pTgtNode = p_pTgtNode;
            m_nWeight = p_nWeight;
            m_nFlow = 0;
        }
        string getName() {return m_strName;}
        node_C* getSrcNode() {return m_pSrcNode;}
        node_C* getTgtNode() {return m_pTgtNode;}
        int getWeight() {return m_nWeight;}
        int getCapacity(node_C*);
        void addFlow(node_C*,int p_nFlow);
        void setFlow(int p_nFlow) {m_nFlow = p_nFlow;}
        int getFlow() {return m_nFlow;}
    private:
        string m_strName;
        node_C* m_pSrcNode;
        node_C* m_pTgtNode;
        int m_nWeight;//can be regarded as capacity as well
        int m_nFlow;
};
//-----------------------------------------------------------------------------------------------------------//
class node_C
{
    public:
        node_C(string p_strName)
        {
            m_nCost = INT_MAX;
            m_strName = p_strName;
            m_pParentNode = 0;
            m_bVisit = false;
        }
        string getName() {return m_strName;}
        void addInEdge(edge_C* p_pInEdge) {m_vInEdge.push_back(p_pInEdge);}
        void addOutEdge(edge_C* p_pOutEdge) {m_vOutEdge.push_back(p_pOutEdge);}
        void getOutEdge(vector<edge_C*>& p_rOutEdge) {p_rOutEdge = m_vOutEdge;}
	void getInEdge(vector<edge_C*>& p_rInEdge) {p_rInEdge = m_vInEdge;}
        int getCost() {return m_nCost;}
        node_C* getParent() {return m_pParentNode;}

        void setCost(int p_nCost) {m_nCost = p_nCost;}
        void setParent(node_C* p_pParent) {m_pParentNode = p_pParent;}
        bool isVisit() {return m_bVisit;}
        void setVisit() {m_bVisit = true;}
        void setUnvisit() {m_bVisit = false;}
        void getAdjNode(vector<node_C*>& p_rvNode);
        void getAllEdge(vector<edge_C*>& p_rvEdge);
        int  getCapacity(node_C*);
        edge_C* getEdge(node_C*);
    private:
        string m_strName;
        vector<edge_C*> m_vOutEdge;
        vector<edge_C*> m_vInEdge;
        int m_nCost;
        bool m_bVisit;
        node_C* m_pParentNode;
};
//-----------------------------------------------------------------------------------------------------------//
class dmMgr_C
{
    public:
        dmMgr_C(graphData_S* p_pGraphData)
        {
            m_pGraphData = p_pGraphData; 
            establishLink();
        }
        void establishLink();
        void getNode(vector<node_C*>& p_rNode) {p_rNode = m_vNode;}
        void getEdge(vector<edge_C*>& p_rEdge) {p_rEdge = m_vEdge;}
        node_C* getSrcNode() {return m_pSrcNode;}
        node_C* getTgtNode() {return m_pTgtNode;}
        void dump();
    private:
        node_C* m_pSrcNode;
        node_C* m_pTgtNode;
        vector<node_C*> m_vNode;
        vector<edge_C*> m_vEdge;
        graphData_S* m_pGraphData;
};
//-----------------------------------------------------------------------------------------------------------//



#endif
