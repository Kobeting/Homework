#include <iostream>

class dmMgr_C;

//-----------------------------------------------------------------------------------------------------------//
class shortestPath_C
{
    public:
        shortestPath_C(dmMgr_C* p_pDmMgr) {m_pDmMgr = p_pDmMgr;}
        bool BellmanFord();
        void Relax(node_C* u, node_C* v, int w);
        void dump();

    private:
        dmMgr_C* m_pDmMgr;
};
//-----------------------------------------------------------------------------------------------------------//
class CriticalPath_C
{
	public:
		CriticalPath_C(dmMgr_C* p_pDmMgr) {m_pDmMgr = p_pDmMgr;}
		bool topologicalSort();
		void dump();
	
	private:
		dmMgr_C* m_pDmMgr;
			
};
//-----------------------------------------------------------------------------------------------------------//
