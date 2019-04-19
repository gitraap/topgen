/*
 *  HUNode.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_HU_NODE_HPP
#define TOPGEN_HU_NODE_HPP

#include <stdio.h>
#include <topgen/topology.hpp>

namespace topgen {

    class CHUNode {
    public:
        CHUNode(int id, int side, int sw, int ports);
        virtual ~CHUNode();

        void SetNeighbor(int port, CHUNode * node);
        CHUNode * Neighbor(int port);
        int GetID();
        int GetSide();
        int GetSwitch();
        int GetPorts();
        char * GetName();

    private:
        int m_iID;
        int m_iSide;
        int m_iSwitch;
        int m_iPorts;
        char * m_sName;
        CHUNode ** m_pNeighbors;
    };

} // namespace topology

#endif /* TOPGEN_HU_NODE_HPP */
