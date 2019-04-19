/*
 *  HUChannel.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/hu_channel.hpp"
#include "topgen/node/channel_type_node.hpp"

namespace topgen {

    CHUChannel::CHUChannel(int id, channelType type, CHUNode * down_node, int down_port, CHUNode * up_node, int up_port) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = down_node;
        m_iDownPort = down_port;
        m_pUpNode = up_node;
        m_iUpPort = up_port;
        m_iIA = -1;
    }

    CHUChannel::CHUChannel(int id, channelType type, CHUNode * node, int port, int ia) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = node;
        m_iDownPort = port;
        m_pUpNode = NULL;
        m_iUpPort = -1;
        m_iIA = ia;
    }

    CHUChannel::~CHUChannel() {
        // nothing to do here
    }

    CChannelType_Node * CHUChannel::Create() {
        CChannelType_Node * new_channel = NULL;
        int u_swid, u_swport, d_swid, d_swport;

        switch (m_eType) {
            case nodeSTD_customhu:
                sprintf(m_sType, "sw-customhu");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->GetName());
                u_swid = m_pUpNode->GetID();
                sprintf(m_sUpPort, "%d", m_iUpPort);
                u_swport = m_iUpPort;
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                d_swid = m_pDownNode->GetID();
                sprintf(m_sDownPort, "%d", m_iDownPort);
                d_swport = m_iDownPort;
                break;

            case iaSTD_customhu:
                sprintf(m_sType, "nic-customhu");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "ia%d", m_iIA);
                u_swid = m_iIA;
                sprintf(m_sUpPort, "%d", 0);
                u_swport = 0;
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                d_swid = m_pDownNode->GetID();
                sprintf(m_sDownPort, "%d", m_iDownPort);
                d_swport = m_iDownPort;
                break;

            default:
                throw new CException("CHUChannel: Print(): Illegal channel type");
        }

        new_channel = new CChannelType_Node(m_sType,
            m_sIdentifier,
            m_iID,
            m_sUpElement, m_sUpPort,
            u_swid, u_swport,
            m_sDownElement, m_sDownPort,
            d_swid, d_swport);

        return new_channel;
    }

} // namespace topology
