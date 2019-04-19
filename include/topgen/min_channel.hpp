/*
 *  MINChannel.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_MIN_CHANNEL_HPP
#define TOPGEN_MIN_CHANNEL_HPP

#include <string.h>
#include <topgen/min_node.hpp>
#include <topgen/exception.hpp>
#include <topgen/node/channel_type_node.hpp>

namespace topgen {

    class CMINChannel {
    public:
        CMINChannel(int id, channelType type, CMINNode * down_node, minNodeStructType * down_port, CMINNode * up_node, minNodeStructType * up_port);
        CMINChannel(int id, channelType type, CMINNode * up_node, minNodeStructType * up_port);
        CMINChannel(int id, channelType type, CMINNode * node, int port);
        virtual ~CMINChannel();

        CChannelType_Node * Create();

        // raw attributes
        minNodeStructType * m_pUpPort;
        minNodeStructType * m_pDownPort;

        CMINNode * m_pUpNode;
        CMINNode * m_pDownNode;

        int m_iID;
        channelType m_eType;
        int m_iPort;

        // strings
        char m_sType[TOPGEN_MAX_STRING_LENGTH];
        char m_sName[TOPGEN_MAX_STRING_LENGTH];
        char m_sIdentifier[TOPGEN_MAX_STRING_LENGTH];
        char m_sUpElement[TOPGEN_MAX_STRING_LENGTH];
        char m_sUpPort[TOPGEN_MAX_STRING_LENGTH];
        char m_sDownElement[TOPGEN_MAX_STRING_LENGTH];
        char m_sDownPort[TOPGEN_MAX_STRING_LENGTH];
    };

} // namespace topology

#endif /* TOPGEN_MIN_CHANNEL_HPP */
