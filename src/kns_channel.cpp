/*
 *  KNSChannel.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/kns_channel.hpp"
#include "topgen/node/channel_type_node.hpp"
#include "topgen/exception.hpp"

namespace topgen {

    CKNSChannel::CKNSChannel(channelType type, int id,
        CElementType_Node * downNode, int downPort,
        CInputAdapterType_Node * upNode, int upPort) {
        m_eType = type;
        m_iID = id;

        m_pDownNode = downNode;
        m_iDownPort = downPort;

        m_pUpNode = NULL;
        m_pUpIA = upNode;
        m_iUpPort = upPort;

        m_iDimension = -1;
        m_pPosition = NULL;
        m_iVirtID = id;

        sprintf(m_sName, "ch%d", id);
        sprintf(m_sIdentifier, "%i", m_iID);
    }

    CKNSChannel::CKNSChannel(channelType type, int id,
        CElementType_Node * downNode, int downPort,
        CElementType_Node * upNode, int upPort) {
        m_eType = type;
        m_iID = id;

        m_pDownNode = downNode;
        m_iDownPort = downPort;

        m_pUpNode = upNode;
        m_pUpIA = NULL;
        m_iUpPort = upPort;

        m_iDimension = -1;
        m_pPosition = NULL;
        m_iVirtID = id;

        sprintf(m_sName, "ch%d", id);
        sprintf(m_sIdentifier, "%i", m_iID);
    }

    CKNSChannel::CKNSChannel(channelType type, int id,
        CElementType_Node * downNode, int downPort,
        CElementType_Node * upNode, int upPort,
        int dim, int * pos, int len, int virtid) {
        m_eType = type;
        m_iID = id;

        m_pDownNode = downNode;
        m_iDownPort = downPort;

        m_pUpNode = upNode;
        m_pUpIA = NULL;
        m_iUpPort = upPort;

        m_iDimension = dim;
        m_pPosition = new int[len];
        for (int i = 0; i < len; i++)
            m_pPosition[i] = pos[i];
        m_iPositionLength = len;
        m_iVirtID = virtid;

        sprintf(m_sName, "ch%d", id);
        sprintf(m_sIdentifier, "%i", m_iID);
    }

    CKNSChannel::~CKNSChannel() {
        if (m_pPosition)
            delete [] m_pPosition;
    }

    CChannelType_Node * CKNSChannel::Create() {
        int u_swid, u_swport, d_swid, d_swport;

        switch (m_eType) {
            case nodeSTD_kns:
                sprintf(m_sType, "node-kns");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->m_sName);
                u_swid = m_pUpNode->m_iIdentifier;
                sprintf(m_sUpPort, "%d", m_iUpPort);
                u_swport = m_iUpPort;
                sprintf(m_sDownElement, "%s", m_pDownNode->m_sName);
                d_swid = m_pDownNode->m_iIdentifier;
                sprintf(m_sDownPort, "%d", m_iDownPort);
                d_swport = m_iDownPort;
                sprintf(m_sDimension, "%d", m_iDimension);
                sprintf(m_sPosition, "%s", "");
                sprintf(m_sVirtID, "%d", m_iVirtID);
                break;

            case iaSTD_kns:
                sprintf(m_sType, "ia-kns");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpIA->m_sName);
                u_swid = m_pUpIA->m_iID;
                sprintf(m_sUpPort, "%d", m_iUpPort);
                u_swport = m_iUpPort;
                sprintf(m_sDownElement, "%s", m_pDownNode->m_sName);
                d_swid = m_pDownNode->m_iIdentifier;
                sprintf(m_sDownPort, "%d", m_iDownPort);
                d_swport = m_iDownPort;
                sprintf(m_sDimension, "%d", m_iDimension);
                sprintf(m_sPosition, "%s", "");
                sprintf(m_sVirtID, "%d", m_iVirtID);
                break;

            case virtSTD_kns:
                sprintf(m_sType, "virt-kns");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->m_sName);
                u_swid = m_pUpNode->m_iIdentifier;
                sprintf(m_sUpPort, "%d", m_iUpPort);
                u_swport = m_iUpPort;
                sprintf(m_sDownElement, "%s", m_pDownNode->m_sName);
                d_swid = m_pDownNode->m_iIdentifier;
                sprintf(m_sDownPort, "%d", m_iDownPort);
                d_swport = m_iDownPort;
                sprintf(m_sDimension, "%d", m_iDimension);
                if (m_pPosition) {
                    for (int i = 0; i < m_iPositionLength; i++) {
                        if (i > 0) sprintf(m_sPosition, ",");
                        sprintf(m_sPosition, "%d", m_pPosition[i]);
                    }
                } else {
                    sprintf(m_sPosition, "%s", "");
                }
                sprintf(m_sVirtID, "%d", m_iVirtID);
                break;

            default:
                throw new CException("CKNSChannel:_LoadChannel(): Unknown channel type");
        }

        CChannelType_Node * new_channel = new CChannelType_Node(m_sType,
            m_sIdentifier,
            m_iID,
            m_sUpElement, m_sUpPort,
            u_swid, u_swport,
            m_sDownElement, m_sDownPort,
            d_swid, d_swport);

        return new_channel;
    }

} // namespace topology
