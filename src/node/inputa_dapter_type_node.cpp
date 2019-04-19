/*
 *  InputAdapterType_Node.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include "topgen/topology.hpp"
#include "topgen/node/input_adapter_type_node.hpp"

namespace topgen {

    CInputAdapterType_Node::CInputAdapterType_Node(const char * type, const char * name, int id, const char * value) {
        m_sType = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sType, "%s", type);

        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sName, "%s", name);

        m_iID = id;

        m_sValue = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sValue, "%s", value);

        // KNS or XGFT
        m_iLength = 0;
        m_pCoordinates = NULL;
    }

    CInputAdapterType_Node::~CInputAdapterType_Node() {
        delete [] m_sType;
        delete [] m_sName;
        delete [] m_sValue;
        if (m_pCoordinates != NULL)
            delete [] m_pCoordinates;
    }

    void CInputAdapterType_Node::WriteTuple(int length, int * coordinates) {
        m_iLength = length;
        m_pCoordinates = new int[length];
        for (int i = 0; i < length; i++)
            m_pCoordinates[i] = coordinates[i];
    }

} // namespace topology
