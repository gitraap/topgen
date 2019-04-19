/*
 *  ChannelType_Node.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_CHANNEL_TYPE_NODE_HPP
#define TOPGEN_CHANNEL_TYPE_NODE_HPP

#include <stdio.h>

namespace topgen {

//#define RANGE_MAX 32

    class CChannelType_Node {
    public:
        CChannelType_Node(const char * type, char * id, int ch_id, char * upElement, char * upPort, int upelement_id, int uport_id, char * downElement, char * downPort, int downelement_id, int dport_id);
        virtual ~CChannelType_Node();
        
        static const int RANGE_MAX = 32;

        char m_sIdentifier[RANGE_MAX];
        char m_sType[RANGE_MAX];
        char m_sName[RANGE_MAX];
        char m_sUpElement[RANGE_MAX];
        char m_sUpPort[RANGE_MAX];
        char m_sDownElement[RANGE_MAX];
        char m_sDownPort[RANGE_MAX];

        int m_iIdentifier;
        int m_iUpElement;
        int m_iUpPort;
        int m_iDownElement;
        int m_iDownPort;
    };

} // namespace topology

#endif /* TOPGEN_CHANNEL_TYPE_NODE_HPP */
