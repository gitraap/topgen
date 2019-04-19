/*
 *  DIRChannel.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/dir_channel.hpp"
#include "topgen/node/channel_type_node.hpp"
#include "topgen/exception.hpp"

namespace topgen {

    CDIRChannel::CDIRChannel(channelType type, int id, CDIRNode * downNode, int downPort, CDIRNode * upNode, int upPort) {
        m_eType = type;
        m_iID = id;

        m_pDownNode = downNode;
        m_iDownPort = downPort;
        m_pUpNode = upNode;
        m_iUpPort = upPort;

        snprintf(m_sName, TOPGEN_MAX_STRING_LENGTH, "c%d", id);
        snprintf(m_sIdentifier, TOPGEN_MAX_STRING_LENGTH, "%i", m_iID);
    }

    CDIRChannel::~CDIRChannel() {
    }

    void CDIRChannel::Print(FILE * file) {
        fprintf(file, "\t\t<declare type=\"%s\" name=\"%s\">\n", m_sType, m_sName);
        fprintf(file, "\t\t\t<parameter name=\"upElement\">%s</parameter>\n", m_sUpElement);
        fprintf(file, "\t\t\t<parameter name=\"upPort\">%s(%d)</parameter>\n", m_sUpPort, m_iUpPort);
        fprintf(file, "\t\t\t<parameter name=\"downElement\">%s</parameter>\n", m_sDownElement);
        fprintf(file, "\t\t\t<parameter name=\"downPort\">%s(%d)</parameter>\n", m_sDownPort, m_iDownPort);
        fprintf(file, "\t\t</declare>\n");
    }

    CChannelType_Node * CDIRChannel::Create() {
        CChannelType_Node * new_channel = NULL;
        int u_swid = 0, u_swport = 0, d_swid = 0, d_swport = 0;

        switch (m_eType) {
            case nodeSTD_mesh:
            case nodeSTD_cube:
            case nodeSTD_torus:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "%s", m_pUpNode->GetName());
                sprintf(m_sUpPort, "%d", m_iUpPort);
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                sprintf(m_sDownPort, "%d", m_iDownPort);

                d_swid = m_pDownNode->GetIdentifier();
                d_swport = m_iDownPort;
                u_swid = m_pUpNode->GetIdentifier();
                u_swport = m_iUpPort;

                break;

            case iaSTD_mesh:
            case iaSTD_torus:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sUpElement, "ia%d", m_pDownNode->GetIdentifier());
                sprintf(m_sUpPort, "%d", 0);
                sprintf(m_sDownElement, "%s", m_pDownNode->GetName());
                sprintf(m_sDownPort, "%d", m_pDownNode->GetNicPort());

                d_swid = m_pDownNode->GetID();
                d_swport = m_iDownPort;
                u_swid = m_pDownNode->GetIdentifier();
                u_swport = 0;

                break;

            case node2T_torus:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);

                d_swid = m_pDownNode->GetSwitchID(m_iDownPort);
                d_swport = m_pDownNode->GetSwitchPort(m_iDownPort);
                sprintf(m_sDownElement, "s%d", d_swid);
                sprintf(m_sDownPort, "%d", d_swport);

                u_swid = m_pUpNode->GetSwitchID(m_iUpPort);
                u_swport = m_pUpNode->GetSwitchPort(m_iUpPort);
                sprintf(m_sUpElement, "s%d", u_swid);
                sprintf(m_sUpPort, "%d", u_swport);
                break;

            case inode2T_torus:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                sprintf(m_sDownElement, "s%d", m_pDownNode->GetIdentifier() + TOPGEN_TOP);
                sprintf(m_sDownPort, "%d", 3);
                sprintf(m_sUpElement, "s%d", m_pDownNode->GetIdentifier() + TOPGEN_BOTTOM);
                sprintf(m_sUpPort, "%d", 3);
                break;

            case ia2T_torus_top:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                d_swid = m_pDownNode->GetIdentifier() + TOPGEN_TOP;
                d_swport = 4;
                sprintf(m_sDownElement, "s%d", d_swid);
                sprintf(m_sDownPort, "%d", d_swport);
                sprintf(m_sUpElement, "ia%d", d_swid);
                sprintf(m_sUpPort, "%d", 0);
                break;

            case ia2T_torus_bottom:
                sprintf(m_sType, "ext");
                sprintf(m_sName, "c%d", m_iID);
                sprintf(m_sIdentifier, "%d", m_iID);
                d_swid = m_pDownNode->GetIdentifier() + TOPGEN_BOTTOM;
                d_swport = 4;
                sprintf(m_sDownElement, "s%d", d_swid);
                sprintf(m_sDownPort, "%d", d_swport);
                sprintf(m_sUpElement, "ia%d", d_swid);
                sprintf(m_sUpPort, "%d", 0);
                break;

            default:
                throw new CException("CChannel:_LoadChannel(): Illegal channel type");
        }

        new_channel = new CChannelType_Node(m_sType,
            m_sIdentifier,
            m_iID,
            m_sUpElement, m_sUpPort,
            u_swid, u_swport,
            m_sDownElement, m_sDownPort,
            d_swid, d_swport);

//#ifdef DEBUG_CHANNEL_CREATION
//        Print(stdout);
//#endif
        return new_channel;
    }

} // namespace topology
