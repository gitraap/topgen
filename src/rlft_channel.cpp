/*
 * RLFTChannel.cpp
 *
 *  Created on: 21/05/2013
 *      Author: jesus
 */

#include "topgen/rlft_channel.hpp"
#include "topgen/node/channel_type_node.hpp"

namespace topgen {

    CRLFTChannel::CRLFTChannel(int id, channelType type, CRLFTNode * down_node, rlftNodeStructType * down_port, CRLFTNode * up_node, rlftNodeStructType * up_port) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = down_node;
        m_pDownPort = down_port;
        m_pUpNode = up_node;
        m_pUpPort = up_port;
    }

    CRLFTChannel::CRLFTChannel(int id, channelType type, CRLFTNode * up_node, rlftNodeStructType * up_port) {
        if (type == nodeSTD_min)
            throw new CException("CRLFTChannel: CRLFTChannel(): This constructor does not support 'node' channel type");

        m_iID = id;
        m_eType = type;
        m_pDownNode = NULL;
        m_pDownPort = NULL;
        m_pUpNode = up_node;
        m_pUpPort = up_port;
    }

    CRLFTChannel::CRLFTChannel(int id, channelType type, CRLFTNode * parent_node, int port) {
        m_iID = id;
        m_eType = type;
        m_pDownNode = parent_node;
        m_iPort = port;
        m_pDownPort = NULL;
        m_pUpNode = NULL;
        m_pUpPort = NULL;
    }

    CRLFTChannel::~CRLFTChannel() {

    }

    CChannelType_Node * CRLFTChannel::Create() {
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

} /* namespace topology */
