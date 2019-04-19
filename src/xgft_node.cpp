/*
 *  XGFTNode.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include <stdlib.h>
#include "topgen/xgft_node.hpp"
#include "topgen/xgft.hpp"

namespace topgen {

    CXGFTNode::CXGFTNode(int id, int level, int node, int height, int ports) {
        m_iID = id;
        m_iLevel = level;
        m_iNode = node;
        m_iHeight = height;
        m_pTuple = NULL;
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];

        if (m_iLevel == 0) {
            sprintf(m_sName, "ia%d", m_iID);
            m_iPorts = 1;
        } else {
            sprintf(m_sName, "s%d", m_iID);
            m_iPorts = ports;
        }
        m_pNeighbors = new CXGFTNode*[m_iPorts];
        for (int p = 0; p < m_iPorts; p++)
            m_pNeighbors[p] = NULL;
    }

    CXGFTNode::~CXGFTNode() {
        delete [] m_sName;
        delete [] m_pNeighbors;
        if (m_pTuple != NULL)
            delete [] m_pTuple;
    }

    void CXGFTNode::WriteTuple(int * children, int * parents, double * randoms) {
        m_pChildren = children;
        m_pParents = parents;
        m_pTuple = new int[m_iHeight + 1]; // we ignore the cell 0
        m_dSum = 0.0;
        int module = m_iNode;
        for (int i = 1; i <= m_iLevel; i++) {
            m_pTuple[i] = module % parents[i - 1];
            module = module / parents[i - 1];
            m_dSum += (m_pTuple[i] * randoms[i]);
        }
        for (int i = m_iLevel + 1; i <= m_iHeight; i++) {
            m_pTuple[i] = module % children[i - 1];
            module = module / children[i - 1];
            m_dSum += (m_pTuple[i] * randoms[i]);
        }
    }

    int * CXGFTNode::Tuple() {
        return m_pTuple;
    }

    double CXGFTNode::Sum() {
        return m_dSum;
    }

    void CXGFTNode::SetNeighbor(int port, CXGFTNode * node) {
        if (m_pNeighbors[port] == NULL)
            m_pNeighbors[port] = node;
        else {
            char * errmsg = new char[256];
            sprintf(errmsg, "CXGFTNode::SetNeighbor(): node id %d port %d of %d ports already assigned.\n",
                m_iID, port, m_iPorts);
            throw new CException(errmsg);
        }
    }

    CXGFTNode * CXGFTNode::Neighbor(int port) {
        return m_pNeighbors[port];
    }

    int CXGFTNode::GetID() {
        return m_iID;
    }

    int CXGFTNode::GetPorts() {
        return m_iPorts;
    }

    char * CXGFTNode::GetName() {
        return m_sName;
    }

    int CXGFTNode::GetLevel() {
        return m_iLevel;
    }

    void CXGFTNode::CheckPorts() {
        if (m_pNeighbors != NULL) {
            //down ports
            if (m_iLevel >= 2) {
                for (int i = 0; i < m_pChildren[m_iLevel - 1]; i++) {
                    if (m_pNeighbors[i] == NULL) {
                        char * errmsg = new char[256];
                        sprintf(errmsg, "CXGFTNode::CheckPorts(): NodeID %d level %d, port %d (down) is NULL.", m_iID, m_iLevel, i);
                        throw new CException(errmsg);
                    }
                }
            }

            //up ports
            if (m_iLevel < m_iHeight - 1) {
                for (int i = m_pChildren[m_iLevel]; i < m_iPorts; i++) {
                    if (m_pNeighbors[i] == NULL) {
                        char * errmsg = new char[256];
                        sprintf(errmsg, "CXGFTNode::CheckPorts(): NodeID %d level %d, port %d (up) is NULL.", m_iID, m_iLevel, i);
                        throw new CException(errmsg);
                    }
                }
            }
        }

    }

} // namespace topology
