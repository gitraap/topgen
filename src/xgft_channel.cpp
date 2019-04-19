/*
 *  XGFTChannel.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/xgft_channel.hpp"
#include "topgen/node/channel_type_node.hpp"

namespace topgen {

    CXGFTChannel::CXGFTChannel(int id, channelType type, CXGFTNode * down_node, int down_port, CXGFTNode * up_node, int up_port) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = down_node;
        m_iDownPort = down_port;
        m_pUpNode = up_node;
        m_iUpPort = up_port;
        m_iIA = -1;
    }

    CXGFTChannel::CXGFTChannel(int id, channelType type, CXGFTNode * node, int port, int ia) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = node;
        m_iDownPort = port;
        m_pUpNode = NULL;
        m_iUpPort = -1;
        m_iIA = ia;
    }

    CXGFTChannel::~CXGFTChannel() {
        // nothing to do here
    }

    CChannelType_Node * CXGFTChannel::Create() {
        CChannelType_Node * new_channel = NULL;
        int up_switch_id, up_switch_port;
        int down_switch_id, down_switch_port;

        switch (m_eType) {
            case nodeSTD_xgft:
                sprintf(m_sType, "sw-xgft");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->GetName());
                up_switch_id = m_pUpNode->GetID();
                sprintf(m_sUpPort, "%d", m_iUpPort);
                up_switch_port = m_iUpPort;
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                down_switch_id = m_pDownNode->GetID();
                sprintf(m_sDownPort, "%d", m_iDownPort);
                down_switch_port = m_iDownPort;
                break;

            case iaSTD_xgft:
                sprintf(m_sType, "nic-xgft");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "ia%d", m_iIA);
                up_switch_id = m_iIA;
                sprintf(m_sUpPort, "%d", 0);
                up_switch_port = 0;
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                down_switch_id = m_pDownNode->GetID();
                sprintf(m_sDownPort, "%d", m_iDownPort);
                down_switch_port = m_iDownPort;
                break;

            default:
                throw new CException("CXGFTChannel: Print(): Illegal channel type");
        }

        new_channel = new CChannelType_Node(m_sType,
            m_sIdentifier, m_iID,
            m_sUpElement, m_sUpPort,
            up_switch_id, up_switch_port,
            m_sDownElement, m_sDownPort,
            down_switch_id, down_switch_port);

        return new_channel;
    }

} // namespace topology
