/*
 *  DFlyChannel.cpp
 *
 *  Created by Juan Villar and Jesus Escudero-Sahuquillo on 01/11/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include <assert.h>
#include "topgen/dfly_channel.hpp"
#include "topgen/node/channel_type_node.hpp"

namespace topgen {

CDFlyChannel::CDFlyChannel(int id, channelType type, CDFlyNode * down_node, dflyNodeStruct_t * down_port, CDFlyNode * up_node, dflyNodeStruct_t * up_port)
{
	m_iID = id;
	m_eType = type;
	m_pDownNode = down_node;
	m_pDownPort = down_port;
	m_pUpNode = up_node;
	m_pUpPort = up_port;
        
        m_sType[0] = '\0';
        m_sName[0] = '\0';
        m_sIdentifier[0] = '\0';
        m_sUpElement[0] = '\0';
        m_sUpPort[0] = '\0';
        m_sDownElement[0] = '\0';
        m_sDownPort[0] = '\0';
}

CDFlyChannel::CDFlyChannel(int id, channelType type, CDFlyNode * up_node, dflyNodeStruct_t * up_port)
{
	m_iID = id;
	m_eType = type;
	m_pDownNode = NULL;
	m_pDownPort = NULL;
	m_pUpNode = up_node;
	m_pUpPort = up_port;
        
        m_sType[0] = '\0';
        m_sName[0] = '\0';
        m_sIdentifier[0] = '\0';
        m_sUpElement[0] = '\0';
        m_sUpPort[0] = '\0';
        m_sDownElement[0] = '\0';
        m_sDownPort[0] = '\0';
}

CDFlyChannel::~CDFlyChannel()
{

}

CChannelType_Node * CDFlyChannel::Create()
{
  CChannelType_Node * new_channel = NULL;
  long int up_switch_id, up_switch_port;
  long int down_switch_id, down_switch_port;
  
  switch (m_eType)
    {
    case nodeSTD_dfly:
        snprintf(m_sType, MAX_STRING_LENGTH, "ext");
        snprintf(m_sName, MAX_STRING_LENGTH, "c%d", m_iID);//uninitialised value(s) Why?
        snprintf(m_sIdentifier, MAX_STRING_LENGTH, "%d", m_iID);//
        snprintf(m_sUpElement, MAX_STRING_LENGTH, "%s", m_pUpNode->GetName());
        up_switch_id = m_pUpNode->GetIdentifier();
        snprintf(m_sUpPort, MAX_STRING_LENGTH, "%d", m_pUpPort->port);
        up_switch_port = m_pUpPort->port;
        snprintf(m_sDownElement, MAX_STRING_LENGTH, "%s", m_pDownNode->GetName());
        down_switch_id = m_pDownNode->GetIdentifier();
        snprintf(m_sDownPort, MAX_STRING_LENGTH, "%d", m_pDownPort->port);
        down_switch_port = m_pDownPort->port;
        break;


    case iaSTD_dfly:
        snprintf(m_sType, MAX_STRING_LENGTH, "ext");
        snprintf(m_sName, MAX_STRING_LENGTH, "c%d", m_iID);//
        snprintf(m_sIdentifier, MAX_STRING_LENGTH, "%d", m_iID);//
        snprintf(m_sUpElement, MAX_STRING_LENGTH, "%s", m_pUpNode->GetName());
        up_switch_id = m_pUpNode->GetIdentifier();
        snprintf(m_sUpPort, MAX_STRING_LENGTH, "%d", m_pUpPort->port);
        up_switch_port = m_pUpPort->port;
        snprintf(m_sDownElement, MAX_STRING_LENGTH, "ia%d", m_pUpNode->GetNetPort(up_switch_port));
        down_switch_id = m_pUpNode->GetNetPort(up_switch_port);
        snprintf(m_sDownPort, MAX_STRING_LENGTH, "%d", 0);
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

}  // namespace topology
