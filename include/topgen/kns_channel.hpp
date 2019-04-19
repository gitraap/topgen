/*
 *  KNSChannel.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_KNS_CHANNEL_HPP
#define TOPGEN_KNS_CHANNEL_HPP

#include <string.h>
#include <topgen/dir_node.hpp>
#include <topgen/node/channel_type_node.hpp>

namespace topgen {

    class CChannelType_Node;

    class CKNSChannel {
    public:
        CKNSChannel(channelType type, int id,
                CElementType_Node * downNode, int downPort,
                CInputAdapterType_Node * upNode, int upPort);

        CKNSChannel(channelType type, int id,
                CElementType_Node * downNode, int downPort,
                CElementType_Node * upNode, int upPort);

        CKNSChannel(channelType type, int id,
                CElementType_Node * downNode, int downPort,
                CElementType_Node * upNode, int upPort,
                int dim, int * pos, int len, int virtid);

        virtual ~CKNSChannel();

        CChannelType_Node * Create();
        void PrintVRMLChannel(FILE * file);

        // raw values
        int m_iDimension;
        int * m_pPosition;
        int m_iPositionLength;
        int m_iVirtID; // unique ID
        channelType m_eType;
        int m_iID;
        CInputAdapterType_Node * m_pUpIA;
        CElementType_Node * m_pUpNode;
        CElementType_Node * m_pDownNode;
        int m_iUpPort;
        int m_iDownPort;

        // strings
        char m_sType[TOPGEN_MAX_STRING_LENGTH];
        char m_sName[TOPGEN_MAX_STRING_LENGTH];
        char m_sIdentifier[TOPGEN_MAX_STRING_LENGTH];
        char m_sUpElement[TOPGEN_MAX_STRING_LENGTH];
        char m_sUpPort[TOPGEN_MAX_STRING_LENGTH];
        char m_sDownElement[TOPGEN_MAX_STRING_LENGTH];
        char m_sDownPort[TOPGEN_MAX_STRING_LENGTH];
        char m_sDimension[TOPGEN_MAX_STRING_LENGTH];
        char m_sPosition[TOPGEN_MAX_STRING_LENGTH];
        char m_sVirtID[TOPGEN_MAX_STRING_LENGTH];
    };

} // namespace topology

#endif /* TOPGEN_KNS_CHANNEL_HPP */
