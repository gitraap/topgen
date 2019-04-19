/*
 *  HUNode.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include <stdlib.h>
#include "topgen/hu_node.hpp"
#include "topgen/custom_hu.hpp"

namespace topgen {

    CHUNode::CHUNode(int id, int side, int sw, int ports) {
        m_iID = id;
        m_iSide = side;
        m_iSwitch = sw;
        m_iPorts = ports;
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];

        if (m_iSide == -1) {
            sprintf(m_sName, "ia%d", m_iID);
            m_iPorts = 1;
        } else {
            sprintf(m_sName, "s%d", m_iID);
            m_iPorts = ports;
        }
        m_pNeighbors = new CHUNode*[m_iPorts];
        for (int p = 0; p < m_iPorts; p++)
            m_pNeighbors[p] = NULL;
    }

    CHUNode::~CHUNode() {
        delete [] m_sName;
        delete [] m_pNeighbors;
    }

    void CHUNode::SetNeighbor(int port, CHUNode * node) {
        if (m_pNeighbors[port] == NULL)
            m_pNeighbors[port] = node;
        else {
            char * errmsg = new char[200];
            sprintf(errmsg, "CHUNode::SetNeighbor(): node id %d port %d of %d ports already assigned.\n",
                m_iID, port, m_iPorts);
            throw new CException(errmsg);
        }
    }

    CHUNode * CHUNode::Neighbor(int port) {
        return m_pNeighbors[port];
    }

    int CHUNode::GetID() {
        return m_iID;
    }

    int CHUNode::GetPorts() {
        return m_iPorts;
    }

    char * CHUNode::GetName() {
        return m_sName;
    }

    int CHUNode::GetSide() {
        return m_iSide;
    }
} // namespace topology
