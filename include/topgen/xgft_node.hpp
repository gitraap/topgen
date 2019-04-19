/*
 *  XGFTNode.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_XGFT_NODE_HPP
#define TOPGEN_XGFT_NODE_HPP

#include <stdio.h>
#include <topgen/topology.hpp>

namespace topgen {

    class CXGFTNode {
    public:
        CXGFTNode(int id, int level, int node, int height, int ports);
        virtual ~CXGFTNode();

        void WriteTuple(int * children, int * parents, double * randoms);
        int * Tuple();
        double Sum();
        void SetNeighbor(int port, CXGFTNode * node);
        CXGFTNode * Neighbor(int port);
        int GetID();
        int GetPorts();
        char * GetName();
        int GetLevel();
        void CheckPorts();

    private:
        int m_iID;
        int m_iLevel;
        int m_iNode; // node inside the level
        int m_iHeight;
        int m_iPorts;
        int * m_pTuple; // position inside the level (for connecting nodes)
        int * m_pParents;
        int * m_pChildren;
        char * m_sName;
        CXGFTNode ** m_pNeighbors;
        double m_dSum;
    };

} // namespace topology

#endif /* TOPGEN_XGFT_NODE_HPP */
