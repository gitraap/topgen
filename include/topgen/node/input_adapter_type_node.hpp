/*
 *  InputAdapterType_Node.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_INPUT_ADAPTER_TYPE_NODE_HPP
#define TOPGEN_INPUT_ADAPTER_TYPE_NODE_HPP

#include <stdio.h>

namespace topgen {

    class CInputAdapterType_Node {
    public:
        CInputAdapterType_Node(const char * type, const char * name, int id, const char * value);
        virtual ~CInputAdapterType_Node();

        void WriteTuple(int len, int * coordinates);

        char * m_sType;
        char * m_sName;
        char * m_sValue;
        int m_iID;

        // KNS and XGFT
        int m_iLength;
        int * m_pCoordinates;
    };

} // namespace topology

#endif /* TOPGEN_INPUT_ADAPTER_TYPE_NODE_HPP */
