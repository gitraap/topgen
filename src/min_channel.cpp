/*
 *  MINChannel.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/min_channel.hpp"
#include "topgen/node/channel_type_node.hpp"

namespace topgen {

    CMINChannel::CMINChannel(int id, channelType type, CMINNode * down_node, minNodeStructType * down_port, CMINNode * up_node, minNodeStructType * up_port) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = down_node;
        m_pDownPort = down_port;
        m_pUpNode = up_node;
        m_pUpPort = up_port;
    }

    CMINChannel::CMINChannel(int id, channelType type, CMINNode * up_node, minNodeStructType * up_port) {
        if (type == nodeSTD_min)
            throw new CException("CMINChannel: CMINChannel(): This constructor does not support 'node' channel type");

        m_iID = id;
        m_eType = type;
        m_pDownNode = NULL;
        m_pDownPort = NULL;
        m_pUpNode = up_node;
        m_pUpPort = up_port;
    }

    CMINChannel::CMINChannel(int id, channelType type, CMINNode * parent_node, int port) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = parent_node;
        m_iPort = port;
        m_pDownPort = NULL;
        m_pUpNode = NULL;
        m_pUpPort = NULL;
    }

    CMINChannel::~CMINChannel() {

    }

    CChannelType_Node * CMINChannel::Create() {
        CChannelType_Node * new_channel = NULL;
        int up_switch_id, up_switch_port;
        int down_switch_id, down_switch_port;

        switch (m_eType) {
            case nodeSTD_min:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->GetName());
                up_switch_id = m_pUpNode->GetIdentifier();
                sprintf(m_sUpPort, "%d", m_pUpPort->port);
                up_switch_port = m_pUpPort->port;
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                down_switch_id = m_pDownNode->GetIdentifier();
                sprintf(m_sDownPort, "%d", m_pDownPort->port);
                down_switch_port = m_pDownPort->port;
                break;

            case node2T_min:
                up_switch_id = m_pUpNode->GetSwitchID(m_pUpPort->port);
                up_switch_port = m_pUpNode->GetSwitchPort(m_pUpPort->port);
                down_switch_id = m_pDownNode->GetSwitchID(m_pDownPort->port);
                down_switch_port = m_pDownNode->GetSwitchPort(m_pDownPort->port);

                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);

                if (up_switch_id == TOPGEN_LEFT) {
                    sprintf(m_sUpElement, "%s-l", m_pUpNode->GetName());
                } else {
                    sprintf(m_sUpElement, "%s-r", m_pUpNode->GetName());
                }
                sprintf(m_sUpPort, "%d", up_switch_port);

                if (down_switch_id == TOPGEN_LEFT) {
                    sprintf(m_sDownElement, "%s-l", m_pDownNode->GetName());
                } else {
                    sprintf(m_sDownElement, "%s-r", m_pDownNode->GetName());
                }
                sprintf(m_sDownPort, "%d", down_switch_port);
                break;

            case inode2T_min:
                sprintf(m_sType, "int");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s-r", m_pDownNode->GetName());
                up_switch_id = m_pDownNode->GetIdentifier();
                sprintf(m_sUpPort, "%d", m_iPort);
                up_switch_port = m_iPort;
                sprintf(m_sDownElement, "%s-l", m_pDownNode->GetName());
                down_switch_id = m_pDownNode->GetIdentifier();
                sprintf(m_sDownPort, "%d", m_iPort);
                down_switch_port = m_iPort;
                break;

            case iaSTD_min:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->GetName());
                up_switch_id = m_pUpNode->GetIdentifier();
                sprintf(m_sUpPort, "%d", m_pUpPort->port);
                up_switch_port = m_pUpPort->port;
                sprintf(m_sDownElement, "ia%lu", m_pUpPort->decimal);
                down_switch_id = m_pUpPort->decimal;
                sprintf(m_sDownPort, "%d", 0);
                down_switch_port = 0;
                break;

            case ia2T_min:
                up_switch_id = m_pUpNode->GetSwitchID(m_pUpPort->port);
                up_switch_port = m_pUpNode->GetSwitchPort(m_pUpPort->port);

                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);

                if (up_switch_id == TOPGEN_LEFT) {
                    sprintf(m_sUpElement, "%s-l", m_pUpNode->GetName());
                } else {
                    sprintf(m_sUpElement, "%s-r", m_pUpNode->GetName());
                }
                sprintf(m_sUpPort, "%d", up_switch_port);

                sprintf(m_sDownElement, "ia%lu", m_pUpPort->decimal);
                down_switch_id = m_pUpPort->decimal;
                sprintf(m_sDownPort, "%d", 0);
                down_switch_port = 0;
                break;

            default:
                throw new CException("CChannel: Print(): Illegal channel type");
        }

        new_channel = new CChannelType_Node(m_sType,
            m_sIdentifier,
            m_iID,
            m_sUpElement, m_sUpPort,
            up_switch_id, up_switch_port,
            m_sDownElement, m_sDownPort,
            down_switch_id, down_switch_port);
        return new_channel;
    }

} // namespace topology
