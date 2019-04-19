/*
 *  ChannelType_Node.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include "topgen/topology.hpp"
#include "topgen/node/channel_type_node.hpp"

namespace topgen {

    CChannelType_Node::CChannelType_Node(const char * type, char * id, int ch_id, char * upElement, char * upPort, int upelement_id, int uport_id, char * downElement, char * downPort, int downelement_id, int dport_id) {
        snprintf(m_sIdentifier, RANGE_MAX, "%s", id);
        snprintf(m_sType, RANGE_MAX, "%s", type);
        snprintf(m_sName, RANGE_MAX, "c%s", id);
        snprintf(m_sUpElement, RANGE_MAX, "%s", upElement);
        snprintf(m_sUpPort, RANGE_MAX, "%s", upPort);
        snprintf(m_sDownElement, RANGE_MAX, "%s", downElement);
        snprintf(m_sDownPort, RANGE_MAX, "%s", downPort);

        m_iIdentifier = ch_id;
        m_iUpElement = upelement_id;
        m_iUpPort = uport_id;
        m_iDownElement = downelement_id;
        m_iDownPort = dport_id;
    }

    CChannelType_Node::~CChannelType_Node() {

    }

} // namespace topology
